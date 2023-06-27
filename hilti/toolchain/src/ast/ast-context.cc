// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include "ast/ast-context.h"

#include <utility>

#include <hilti/ast/ast-context.h>
#include <hilti/ast/visitor.h>
#include <hilti/base/timing.h>
#include <hilti/compiler/plugin.h>

#include "ast/builder/builder.h"
#include "compiler/detail/visitors.h"

using namespace hilti;
using namespace hilti::util;

namespace hilti::logging::debug {
inline const DebugStream AstCache("ast-cache");
inline const DebugStream AstCodegen("ast-codegen");
inline const DebugStream AstDeclarations("ast-declarations");
inline const DebugStream AstDumpIterations("ast-dump-iterations");
inline const DebugStream AstFinal("ast-final");
inline const DebugStream AstOrig("ast-orig");
inline const DebugStream AstPrintTransformed("ast-print-transformed");
inline const DebugStream AstResolved("ast-resolved");
inline const DebugStream AstTransformed("ast-transformed");
inline const DebugStream Compiler("compiler");
} // namespace hilti::logging::debug

ASTRoot::~ASTRoot() = default;

std::string ASTRoot::_render() const { return ""; }

ASTContext::ASTContext(Context* context) : _context(context) {
    _root = ASTRoot::create(this);
    _root->getOrCreateScope(); // create the global scope
}

Result<module::UID> ASTContext::parseSource(const hilti::rt::filesystem::path& path,
                                            std::optional<hilti::rt::filesystem::path> process_extension) {
    return _parseSource(path, {}, std::move(process_extension));
}

Result<module::UID> ASTContext::importModule(const ID& id, const std::optional<ID>& scope,
                                             const hilti::rt::filesystem::path& parse_extension,
                                             const std::optional<hilti::rt::filesystem::path>& process_extension,
                                             std::vector<hilti::rt::filesystem::path> search_dirs) {
    auto parse_plugin = plugin::registry().pluginForExtension(parse_extension);

    if ( ! (parse_plugin && parse_plugin->get().parse) )
        return result::Error(fmt("no plugin provides support for importing *%s files", parse_extension.native()));

    auto filename = fmt("%s%s", util::tolower(id), parse_extension.native());

    if ( scope )
        filename = fmt("%s/%s", util::replace(scope->str(), ".", "/"), filename);

    std::vector<hilti::rt::filesystem::path> library_paths = std::move(search_dirs);

    if ( parse_plugin->get().library_paths )
        library_paths = util::concat(std::move(library_paths), (*parse_plugin->get().library_paths)(_context));

    library_paths = util::concat(_context->options().library_paths, library_paths);

    auto path = util::findInPaths(filename, library_paths);
    if ( ! path ) {
        HILTI_DEBUG(logging::debug::Compiler, fmt("Failed to find module '%s' in search paths:", filename));
        for ( const auto& p : library_paths )
            HILTI_DEBUG(logging::debug::Compiler, fmt("  %s", p));

        return result::Error(fmt("cannot find file"));
    }

    auto uid = _parseSource(*path, scope, process_extension);
    if ( ! uid )
        return uid;

    if ( uid->id != id )
        return result::Error(
            util::fmt("file %s does not contain expected module %s (but %s)", path->native(), id, uid->id));

    return uid;
}

Result<module::UID> ASTContext::_parseSource(const hilti::rt::filesystem::path& path, const std::optional<ID>& scope,
                                             std::optional<hilti::rt::filesystem::path> process_extension) {
    util::timing::Collector _("hilti/compiler/parser");

    std::ifstream in;
    in.open(path);

    if ( ! in )
        return result::Error(fmt("cannot open source file %s", path));

    auto plugin = plugin::registry().pluginForExtension(path.extension());

    if ( ! (plugin && plugin->get().parse) )
        return result::Error(fmt("no plugin provides support for importing *%s files", path.extension().native()));

    auto dbg_message = fmt("parsing file %s as %s code", path, plugin->get().component);

    if ( plugin->get().component != "HILTI" )
        dbg_message += fmt(" (%s)", plugin->get().component);

    HILTI_DEBUG(logging::debug::Compiler, dbg_message);

    auto builder = Builder(this);
    auto module = (*plugin->get().parse)(&builder, in, path);
    if ( ! module )
        return module.error();

    if ( module && ! (*module)->id() )
        return result::Error(fmt("module in %s does not have an ID", path.native()));

    if ( scope )
        (*module)->setScopePath(*scope);

    if ( process_extension )
        (*module)->setProcessExtension(*process_extension);

    return _addModuleToAST(std::move(*module));
}

module::UID ASTContext::_addModuleToAST(ModulePtr module) {
    auto uid = module->uid();

    _modules_by_uid[uid] = module;
    _modules_by_path[uid.path.native()] = module;
    _modules_by_id_and_scope[std::make_pair(uid.id, module->scopePath())] = module;

    _root->addChild(std::move(module));
    return uid;
}

template<typename PluginMember, typename... Args>
Result<Nothing> _runHook(bool* modified, const Plugin& plugin, PluginMember hook, const std::string& description,
                         const Args&... args) {
    if ( ! (plugin.*hook) )
        return Nothing();

    auto msg = fmt("[%s] %s", plugin.component, description);

    HILTI_DEBUG(logging::debug::Compiler, msg);
    if ( (*(plugin.*hook))(args...) ) {
        *modified = true;
        HILTI_DEBUG(logging::debug::Compiler, "  -> modified");
    }

    if ( logger().errors() )
        return result::Error("aborting due to errors during " + description);

    return Nothing();
}

Result<Nothing> ASTContext::processAST() {
    if ( _resolved )
        return Nothing();

    _rebuild_scopes = true;

    for ( const auto& plugin : plugin::registry().plugins() ) {
        if ( auto rc = _validate(plugin, true); ! rc )
            return rc;

        if ( auto rc = _resolve(plugin); ! rc )
            return rc;

        if ( auto rc = _validate(plugin, false); ! rc )
            return rc;

        if ( auto rc = _optimize(plugin); ! rc )
            return rc;

        if ( auto rc = _validate(plugin, false); ! rc )
            return rc;

        // TODO: Transform
    }

    return Nothing();
}

Result<Nothing> ASTContext::_resolve(const Plugin& plugin) {
    HILTI_DEBUG(logging::debug::Compiler, fmt("resolving units with plugin %s", plugin.component))

    logging::DebugPushIndent _(logging::debug::Compiler);

    _dumpAST(logging::debug::AstOrig, plugin, "Original AST", 0);
    _saveIterationAST(plugin, "AST before first iteration", 0);

    int round = 1;
    bool modified = true;

    auto builder = Builder(this);

    while ( modified ) {
        HILTI_DEBUG(logging::debug::Compiler, fmt("processing ASTs, round %d", round));
        logging::DebugPushIndent _(logging::debug::Compiler);

        for ( const auto& i : hilti::visitor::PreOrder().walk(_root) ) {
            assert(i); // walk() should not give us null pointer children.
            i->clearErrors();
        }

        if ( _rebuild_scopes ) {
            HILTI_DEBUG(logging::debug::Compiler, "building scopes");

            for ( const auto& i : hilti::visitor::PreOrder().walk(_root) )
                i->clearScope();

            if ( auto rc = _runHook(&modified, plugin, &Plugin::ast_build_scopes, "building scopes", &builder, _root);
                 ! rc )
                return rc.error();
        }

        modified = false;

        if ( auto rc = _runHook(&modified, plugin, &Plugin::ast_normalize, "normalizing", &builder, _root); ! rc )
            return rc.error();

        if ( auto rc = _runHook(&modified, plugin, &Plugin::ast_coerce, "coercing", &builder, _root); ! rc )
            return rc.error();

        if ( auto rc = _runHook(&modified, plugin, &Plugin::ast_resolve, "resolving", &builder, _root); ! rc )
            return rc.error();

        _dumpAST(logging::debug::AstResolved, plugin, "AST after resolving", round);
        _saveIterationAST(plugin, "AST after resolving", round);

        if ( ++round >= 50 )
            logger().internalError("hilti::Unit::compile() didn't terminate, AST keeps changing");
    }

    _dumpAST(logging::debug::AstFinal, plugin, "Final AST", round);
    _dumpDeclarations(plugin);
    _saveIterationAST(plugin, "Final AST", round);

#if 0
    // TODO
    if ( _driver_options.dump_code )
        dumpUnit(*u); // may be overwritten again later after optimization
#endif

    HILTI_DEBUG(logging::debug::Compiler, "finalized AST");
    _resolved = true;

#if 0
    // TODO: Where to call these now?
    hookNewASTPostCompilation();

    if ( auto rc = hookCompilationFinished(plugin); ! rc )
    return augmentError(rc.error());
#endif

    return Nothing();
}

Result<Nothing> ASTContext::_optimize(const Plugin& plugin) { return Nothing(); }

Result<Nothing> ASTContext::_validate(const Plugin& plugin, bool pre_resolve) {
    if ( _context->options().skip_validation )
        return Nothing();

    bool modified = false; // not used

    if ( pre_resolve )
        _runHook(&modified, plugin, &Plugin::ast_validate_pre, "validating (pre)", _context->astContext().get(), _root);
    else
        _runHook(&modified, plugin, &Plugin::ast_validate_post, "validating (post)", _context->astContext().get(), _root);

    return _collectErrors();
}

void ASTContext::_dumpAST(const logging::DebugStream& stream, const Plugin& plugin, const std::string& prefix,
                          int round) {
    if ( ! logger().isEnabled(stream) )
        return;

    std::string r;

    if ( round > 0 )
        r = fmt(" (round %d)", round);

    HILTI_DEBUG(stream, fmt("# [%s] %s%s", plugin.component, prefix, r));
    detail::renderNode(root(), stream, true);
}

void ASTContext::_dumpAST(std::ostream& stream, const Plugin& plugin, const std::string& prefix, int round) {
    std::string r;

    if ( round > 0 )
        r = fmt(" (round %d)", round);

    stream << fmt("# [%s] %s%s\n", plugin.component, prefix, r);
    detail::renderNode(root(), stream, true);
}

void ASTContext::dumpAST(const logging::DebugStream& stream, const std::string& prefix) {
    if ( ! logger().isEnabled(stream) )
        return;

    HILTI_DEBUG(stream, fmt("# %s\n", prefix));
    detail::renderNode(root(), stream, true);
}

void ASTContext::_dumpDeclarations(const Plugin& plugin) {
    if ( ! logger().isEnabled(logging::debug::AstDeclarations) )
        return;

    logger().debugSetIndent(logging::debug::AstDeclarations, 0);
    HILTI_DEBUG(logging::debug::AstDeclarations, fmt("# [%s]", plugin.component));

    auto nodes = visitor::RangePreOrder(root());
    for ( auto i = nodes.begin(); i != nodes.end(); ++i ) {
        auto decl = (*i)->tryAs<Declaration>();
        if ( ! decl )
            continue;

        logger().debugSetIndent(logging::debug::AstDeclarations, i.depth() - 1);
        HILTI_DEBUG(logging::debug::AstDeclarations,
                    fmt("- %s \"%s\" (%s)", ID((*i)->typename_()).local(), decl->id(), decl->canonicalID()));
    }
}

void ASTContext::_saveIterationAST(const Plugin& plugin, const std::string& prefix, int round) {
    if ( ! logger().isEnabled(logging::debug::AstDumpIterations) )
        return;

    std::ofstream out(fmt("ast-%s-%d.tmp", plugin.component, round));
    _dumpAST(out, plugin, prefix, round);
}

void ASTContext::_saveIterationAST(const Plugin& plugin, const std::string& prefix, const std::string& tag) {
    if ( ! logger().isEnabled(logging::debug::AstDumpIterations) )
        return;

    std::ofstream out(fmt("ast-%s-%s.tmp", plugin.component, tag));
    _dumpAST(out, plugin, prefix, 0);
}

std::vector<module::UID> ASTContext::dependencies(const module::UID& uid, bool recursive) const {
    // TODO
    HILTI_DEBUG(logging::debug::Compiler, fmt("Warning: AST dependencies not implemented yet"));
    return {};
}

#if 0
Result<Nothing> Unit::transformAST(const Plugin& plugin) {
    if ( ! _module )
        return Nothing();

    bool modified = false;
    runHook(&modified, plugin, _module->as<Module>(), _extension, &Plugin::ast_transform,
            fmt("transforming module %s", uniqueID()), context(), _module, this);

    return Nothing();
}

#endif

static node::ErrorPriority _recursiveValidateAST(const NodePtr& n, Location closest_location, node::ErrorPriority prio,
                                                 int level, std::vector<node::Error>* errors) {
    if ( n->location() )
        closest_location = n->location();

    if ( ! n->pruneWalk() ) {
        auto oprio = prio;
        for ( const auto& c : n->children() ) {
            if ( c )
                prio = std::max(prio, _recursiveValidateAST(c, closest_location, oprio, level + 1, errors));
        }
    }

    auto errs = n->errors();
    auto nprio = prio;
    for ( auto& err : errs ) {
        if ( ! err.location && closest_location )
            err.location = closest_location;

        if ( err.priority > prio )
            errors->push_back(err);

        nprio = std::max(nprio, err.priority);
    }

    return nprio;
}

static void _reportErrors(const std::vector<node::Error>& errors) {
    // We only report the highest priority error category.
    std::set<node::Error> reported;

    auto prios = {node::ErrorPriority::High, node::ErrorPriority::Normal, node::ErrorPriority::Low};

    for ( auto p : prios ) {
        for ( const auto& e : errors ) {
            if ( e.priority != p )
                continue;

            if ( reported.find(e) == reported.end() ) {
                logger().error(e.message, e.context, e.location);
                reported.insert(e);
            }
        }

        if ( reported.size() )
            break;
    }
}

Result<Nothing> ASTContext::_collectErrors() {
    std::vector<node::Error> errors;
    _recursiveValidateAST(_root, Location(), node::ErrorPriority::NoError, 0, &errors);

    if ( errors.size() ) {
        _reportErrors(errors);
        return result::Error("validation failed");
    }

    return Nothing();
}

#if 0

void Unit::_destroyModule() {
    if ( ! _module )
        return;

    _module->destroyChildren();
    _module = {};
}

Result<Nothing> Driver::_transformUnitsWithPlugin(const Plugin& plugin,
                                                  const std::vector<std::shared_ptr<Unit>>& units) {
    if ( ! plugin.ast_transform )
        return Nothing();

    HILTI_DEBUG(logging::debug::Compiler,
                fmt("transforming units with plugin %s: %s", plugin.component,
                    util::join(util::transform(units, [](const auto& u) { return u->uniqueID(); }), ", ")));

    logging::DebugPushIndent _(logging::debug::Compiler);

    for ( const auto& unit : units ) {
        if ( auto rc = unit->transformAST(plugin); ! rc )
            return rc;

        unit->setResolved(false);
        context()->cacheUnit(unit);

        _dumpAST(unit, logging::debug::AstTransformed, plugin, "Transformed AST", 0);
        _saveIterationAST(unit, plugin, "Transformed AST", 0);

        if ( logger().isEnabled(logging::debug::AstPrintTransformed) )
            hilti::print(std::cout, unit->module());

        if ( logger().errors() )
            return result::Error("aborting after errors");
    }

    return Nothing();
}

Result<Nothing> Driver::_optimizeUnits() {
    return Nothing();
#if 0 // TODO
    if ( ! _driver_options.global_optimizations )
        return Nothing();

    HILTI_DEBUG(logging::debug::Driver, "performing global transformations");

    Optimizer opt(_hlts);
    opt.run();

    return Nothing();
#endif
}

#endif
