// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <hilti/rt/util.h>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/property.h>
#include <hilti/ast/forward.h>
#include <hilti/ast/node.h>
#include <hilti/ast/statements/block.h>

namespace hilti {

namespace module {

// Globally unique ID for a module that can be used to name it unambiguously.
struct UID {
    ID id; /**< module name */
    hilti::rt::filesystem::path
        path; /**< path to module's source code on disk; will be set to a unique place-holder if no file exists */
    hilti::rt::filesystem::path parse_extension; /**< language extension determining how to *parse* this module, usually
                                                    derive from file name */
    hilti::rt::filesystem::path
        process_extension; /**< language extension determining how to process this module *after* parsing */

    UID(ID id, const hilti::rt::filesystem::path& path)
        : id(std::move(id)),
          path(util::normalizePath(path)),
          parse_extension(path.extension()),
          process_extension(path.extension()) {}

    UID(const ID& id, hilti::rt::filesystem::path parse_extension, const hilti::rt::filesystem::path& process_extension)
        : id(id), parse_extension(std::move(parse_extension)), process_extension(process_extension) {
        path = util::fmt("/tmp/hilti/%s.%" PRIu64 ".%s.%s", id, ++_no_file_counter, process_extension, parse_extension);
    }

    size_t hash() const {
        return rt::hashCombine(std::hash<std::string>{}(id.str()), std::hash<std::string>{}(path.native()),
                               std::hash<std::string>{}(parse_extension.native()),
                               std::hash<std::string>{}(process_extension.native()));
    }

    bool operator==(const UID& other) const {
        return id == other.id && path == other.path && parse_extension == other.parse_extension &&
               process_extension == other.process_extension;
    }

    bool operator!=(const UID& other) const { return ! (*this == other); }

    std::string str() const { return util::fmt("%s_%x", id, hash() % 0xfff); } // should be unique "enough"

    operator std::string() const { return str(); }
    explicit operator bool() const { return ! id.empty(); }

private:
    inline static uint64_t _no_file_counter = 0;
};

inline std::ostream& operator<<(std::ostream& stream, const UID& uid) { return stream << uid.str(); }

} // namespace module

/** Base class for Module nodes. */
class Module : public Node, public node::WithDocString {
public:
    ~Module() override;

    const auto& uid() const { return _uid; }
    const auto& id() const { return _uid.id; }
    const auto& scopePath() const { return _scope_path; }
    auto statements() const { return child<statement::Block>(0); }
    auto declarations() const { return childrenOfType<Declaration>(); }

    bool isEmpty() const {
        // We always have a block as children.
        return children().size() <= 1 && statements()->statements().empty();
    }

    /**
     * Removes any content from the module. The result is an empty module just
     * as if it had just been created. (The ID remains in place.)
     */
    void clear();

    /**
     * Returns a module's property declaration of a given name. If there's no
     * property declaration of that name, return an error. If there's more than
     * one of that name, it's undefined which one is returned.
     *
     * @param id name of the property to return
     */
    declaration::PropertyPtr moduleProperty(const ID& id) const;

    /**
     * Returns all of module's property declarations of a given name. If
     * there's no property declaration of that ID, return an empty container.
     *
     * @param id name of the property to return; leave unset for returning all
     */
    hilti::node::Set<declaration::Property> moduleProperties(const std::optional<ID>& id) const;

    /**
     * Adds a declaration to the module. It will be appended to the current
     * list of declarations.
     *
     * Note this is a mutating function. `Module` is an exception among AST
     * classes in that we allow to modify existing instances. Changes will be
     * reflected in all copies of this instance.
     */
    void add(DeclarationPtr d) { addChild(std::move(d)); }

    /**
     * Adds a top-level statement to the module. It will be appended to the
     * end of the current list of statements and execute at module initialize
     * time.
     *
     * Note this is a mutating function. `Module` is an exception among AST
     * classes in that we allow to modify existing instances. Changes will be
     * reflected in all copies of this instance.
     *
     */
    void add(StatementPtr s) { child<statement::Block>(0)->add(std::move(s)); }

    void setScopePath(const ID& scope) { _scope_path = scope; }
    void setProcessExtension(const hilti::rt::filesystem::path& ext) { _uid.process_extension = ext; }

    node::Properties properties() const override {
        auto p = node::Properties{{"id", _uid.id},
                                  {"path", _uid.path.native()},
                                  {"ext", _uid.process_extension.native()},
                                  {"scope", _scope_path}};
        return Node::properties() + p;
    }

    static auto create(ASTContext* ctx, const module::UID& uid, const ID& scope, const Declarations& decls,
                       Statements stmts, const Meta& meta = {}) {
        Nodes nodes = {statement::Block::create(ctx, std::move(stmts), meta)};
        for ( auto d : decls )
            nodes.push_back(std::move(d));

        return NodeDerivedPtr<Module>(new Module(std::move(nodes), uid, scope, meta));
    }

    static auto create(ASTContext* ctx, const module::UID& uid, const ID& scope = {}, const Meta& meta = {}) {
        return create(ctx, uid, scope, {}, {}, meta);
    }

    static auto create(ASTContext* ctx, const module::UID& uid, const ID& scope, const Declarations& decls,
                       const Meta& meta = {}) {
        return create(ctx, uid, scope, decls, {}, meta);
    }

protected:
    Module(Nodes children, module::UID uid, ID scope, Meta meta = {})
        : Node(std::move(children), std::move(meta)), _uid(std::move(uid)), _scope_path(std::move(scope)) {}

    std::string _render() const override;

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Module>();
        if ( ! n )
            return false;

        return Node::isEqual(other) && _uid == n->_uid && _scope_path == n->_scope_path;
    }

    HILTI_NODE(Module);

private:
    module::UID _uid;
    ID _scope_path;
};

} // namespace hilti

namespace std {
template<>
struct hash<hilti::module::UID> {
    size_t operator()(const hilti::module::UID& x) const { return x.hash(); }
};
} // namespace std
