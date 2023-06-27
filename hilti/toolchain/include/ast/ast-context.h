// Copyright (c) 2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <hilti/ast/declarations/module.h>
#include <hilti/ast/forward.h>
#include <hilti/ast/node.h>
#include <hilti/base/logger.h>

namespace hilti {

class Context;
class ASTContext;
struct Plugin;

/** Root node for the AST inside an AST context. This will always be present exactly once. */
class ASTRoot : public Node {
public:
    ~ASTRoot() override;

    static auto create(ASTContext* ctx) { return NodeDerivedPtr<ASTRoot>(new ASTRoot()); }

protected:
    ASTRoot() : Node({}, Meta(Location("<root>"))) {}

    std::string _render() const override;

    bool isEqual(const Node& other) const override { return other.isA<ASTRoot>(); }

    HILTI_NODE(ASTRoot);
};

// Container for AST-wide state.
class ASTContext : public std::enable_shared_from_this<ASTContext> {
public:
    enum ASTState { Modified, NotModified };

    ASTContext(Context* context);

    auto root() const { return _root; }

    Result<module::UID> parseSource(const hilti::rt::filesystem::path& path,
                                    std::optional<hilti::rt::filesystem::path> process_extension = {});

    Result<module::UID> importModule(const ID& id, const std::optional<ID>& scope,
                                     const hilti::rt::filesystem::path& parse_extension,
                                     const std::optional<hilti::rt::filesystem::path>& process_extension,
                                     std::vector<hilti::rt::filesystem::path> search_dirs);

    ModulePtr getModule(const module::UID& uid) const {
        if ( auto m = _modules_by_uid.find(uid); m != _modules_by_uid.end() )
            return m->second;
        else
            return nullptr;
    }

    Result<Nothing> processAST(); // top-level entry point for resolving/validating/optimizing AST

    bool isFullyResolved() const { return _resolved; }

    /**
     * Returns direct & indirect dependencies that a module imports. This
     * information will be available only once the AST has been resolved
     * successfully.
     *
     * @param uid UID of module to return dependencies for; the module must be known, otherwise an internal error is
     * reported
     * @param recursive if true, return the transitive closure of all
     * dependent units, vs just direct dependencies of the current unit
     * @return set of dependencies, or empty vector if not yet resolved
     */
    std::vector<module::UID> dependencies(const module::UID& uid, bool recursive = false) const;

    /**
     * Dumps the current total AST of all modules to a debug stream.
     *
     * @param stream debug stream to write to
     */
    void dumpAST(const hilti::logging::DebugStream& stream, const std::string& prefix);

private:
    Result<module::UID> _parseSource(const hilti::rt::filesystem::path& path, const std::optional<ID>& scope,
                                     std::optional<hilti::rt::filesystem::path> process_extension = {});
    module::UID _addModuleToAST(ModulePtr module);
    Result<Nothing> _buildScopes(const Plugin& plugin);
    Result<Nothing> _resolve(const Plugin& plugin);
    Result<Nothing> _optimize(const Plugin& plugin);
    Result<Nothing> _validate(const Plugin& plugin, bool pre_resolver);
    Result<Nothing> _collectErrors();

    void _saveIterationAST(const Plugin& plugin, const std::string& prefix, int round = 0);
    void _saveIterationAST(const Plugin& plugin, const std::string& prefix, const std::string& tag);

    void _dumpAST(const hilti::logging::DebugStream& stream, const Plugin& plugin, const std::string& prefix,
                  int round);
    void _dumpAST(std::ostream& stream, const Plugin& plugin, const std::string& prefix, int round);
    void _dumpDeclarations(const Plugin& plugin);

    Context* _context;
    NodeDerivedPtr<ASTRoot> _root;
    bool _rebuild_scopes = true;
    bool _resolved = false;

    std::unordered_map<module::UID, ModulePtr> _modules_by_uid;
    std::unordered_map<std::string, ModulePtr> _modules_by_path;
    std::map<std::pair<ID, ID>, ModulePtr> _modules_by_id_and_scope;
};

} // namespace hilti
