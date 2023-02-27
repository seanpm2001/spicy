// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/property.h>
#include <hilti/ast/forward.h>
#include <hilti/ast/node.h>
#include <hilti/ast/statements/block.h>

namespace hilti {

/** Base class for Module nodes. */
class Module : public Node, public node::WithDocString {
public:
    ~Module() override;

    const auto& id() const { return _id; }
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

    node::Properties properties() const override {
        auto p = node::Properties{{"id", _id}};
        return Node::properties() + p;
    }

    static auto create(ASTContext* ctx, ID id, const Declarations& decls, Statements stmts, const Meta& meta = {}) {
        Nodes nodes = {statement::Block::create(ctx, std::move(stmts), meta)};
        for ( auto d : decls )
            nodes.push_back(std::move(d));

        return NodeDerivedPtr<Module>(new Module(std::move(nodes), std::move(id), meta));
    }

    static auto create(ASTContext* ctx, ID id = {}, const Meta& meta = {}) {
        return create(ctx, std::move(id), {}, {}, meta);
    }

    static auto create(ASTContext* ctx, ID id, const Declarations& decls, const Meta& meta = {}) {
        return create(ctx, std::move(id), decls, {}, meta);
    }

protected:
    Module(Nodes children, ID id, Meta meta = {}) : Node(std::move(children), std::move(meta)), _id(std::move(id)) {}

    std::string _render() const override;

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Module>();
        if ( ! n )
            return false;

        return Node::isEqual(other) && _id == n->_id;
    }

    HILTI_NODE(Module);

private:
    ID _id;
};

} // namespace hilti
