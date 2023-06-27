// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <iomanip>
#include <sstream>

#include <hilti/ast/ctors/enum.h>
#include <hilti/ast/declarations/imported-module.h>
#include <hilti/ast/declarations/module.h>
#include <hilti/ast/declarations/type.h>
#include <hilti/ast/expressions/ctor.h>
#include <hilti/ast/module.h>
#include <hilti/ast/node.h>
#include <hilti/ast/type.h>
#include <hilti/ast/visitor.h>
#include <hilti/base/util.h>
#include <hilti/compiler/detail/visitors.h>

#include "ast/declarations/constant.h"
#include "ast/expressions/ctor.h"

using namespace hilti;

Node::~Node() = default;

std::string Node::render(bool include_location) const {
    auto f = [&](const node::Properties::value_type& x) {
        return util::fmt("%s=%s", x.first, std::quoted(node::to_string(x.second)));
    };

    std::vector<std::string> props;

    for ( const auto& x : properties() )
        props.push_back(f(x));

    std::string sprops;

    if ( ! props.empty() )
        sprops = util::fmt(" <%s>", util::join(props, " "));

    // Prettify the name a bit.
    auto name = util::demangle(typename_());
    name = util::replace(name, "hilti::", "");

    if ( util::startsWith(name, "detail::") )
        name = util::join(util::slice(util::split(name, "::"), 2), "::");

    auto location = (include_location && meta().location()) ? util::fmt(" (%s)", meta().location().render(true)) : "";
    auto prune = (pruneWalk() ? " (prune)" : "");
    auto no_inherit_scope = (inheritScope() ? "" : " (no-inherit-scope)");

#if 0
    // TODO: Move into the corresponding sub-classes.
    if ( auto x = this->tryAs<expression::ResolvedID>() )
        type = util::fmt(" (type: %s [@t:%p])", x->type(), x->type().identity());
#endif

    auto s = util::fmt("%s%s%s%s%s", name, sprops, prune, no_inherit_scope, location);

    if ( auto derived_render = _render(); ! derived_render.empty() )
        s += std::string(" ") + derived_render;

#if 0
    // TODO: Move into the corresponding sub-classes.
        if ( auto t = this->tryAs<declaration::Type>() )
            s += (type::isResolved(t->type()) ? " (resolved)" : " (not resolved)");

    else if ( auto m = this->tryAs<Module>() )
        s += fmtDoc(m->documentation());
#endif

    s += util::fmt(" [@%s:%p]", util::tolower(name.substr(0, 1)), identity());

    // Format errors last on the line since they are not properly delimited.
    if ( hasErrors() )
        for ( auto&& e : errors() ) {
            auto prio = "";
            if ( e.priority == node::ErrorPriority::Low )
                prio = " (low prio)";
            else if ( e.priority == node::ErrorPriority::High )
                prio = " (high prio)";

            s += util::fmt("  [ERROR] %s%s", e.message, prio);
        }

    return s;
}

void Node::print(std::ostream& out, bool compact) const {
    detail::printAST(const_cast<Node*>(this)->shared_from_this(), out, compact);
}

void Node::replaceChildren(Nodes children) {
    clearChildren();

    for ( auto&& c : children )
        addChild(std::move(c));
}

void Node::_destroyChildrenRecursively(Node* n) {
    for ( auto& c : n->children() ) {
        if ( c && ! c->pruneWalk() )
            _destroyChildrenRecursively(c.get());
    }

    n->clearChildren();
}

void Node::destroyChildren() { _destroyChildrenRecursively(this); }

NodePtr node::clone(ASTContext* ctx, const NodePtr& n) {
    auto clone = n->_clone(ctx);
    return clone;
}

NodePtr node::deepClone(ASTContext* ctx, const NodePtr& n) {
    // TODO: Deal with reference cycles.
    auto clone_ = clone(ctx, n);
    clone_->clearChildren();

    for ( const auto& c : n->children() )
        clone_->addChild(deepClone(ctx, c));

    return clone_;
}

// Helper looking up an ID inside a node's direct scope, applying visibility rules.
static std::pair<bool, Result<std::pair<DeclarationPtr, ID>>> _lookupID(const ID& id, const Node* n) {
    assert(n->scope());
    auto resolved = n->scope()->lookupAll(id);

    if ( resolved.empty() ) {
        auto err = result::Error(util::fmt("unknown ID '%s'", id));
        return std::make_pair(false, std::move(err));
    }

    if ( resolved.size() > 1 ) {
        auto err = result::Error(util::fmt("ID '%s' is ambiguous", id));
        return std::make_pair(true, std::move(err));
    }

    const auto& r = resolved.front();
    assert(r.node);
    const auto& d = r.node;

    if ( d->isA<declaration::Module>() || d->isA<declaration::ImportedModule>() ) {
        auto err = result::Error(util::fmt("cannot refer to module '%s' through an ID in this context", id));
        return std::make_pair(true, std::move(err));
    }

    if ( r.external && d->linkage() != declaration::Linkage::Public ) {
        bool ok = false;

        // We allow access to types (and type-derived constants) to
        // make it less cumbersome to define external hooks.

        if ( d->isA<declaration::Type>() )
            ok = true;

        if ( auto c = d->tryAs<declaration::Constant>() ) {
            if ( auto ctor = c->value()->tryAs<expression::Ctor>(); ctor && ctor->ctor()->isA<ctor::Enum>() )
                ok = true;
        }

        if ( ! ok ) {
            auto err = result::Error(util::fmt("'%s' has not been declared public", id));
            return std::make_pair(true, std::move(err));
        }
    }

    auto x = std::make_pair(resolved.front().node, ID(resolved.front().qualified));
    return std::make_pair(true, std::move(x));
}

Result<std::pair<DeclarationPtr, ID>> Node::lookupID(const ID& id, const std::string_view& what) const {
    for ( const auto* n = this; n; n = n->parent() ) {
        if ( ! n->scope() )
            continue;

        auto [stop, resolved] = _lookupID(id, n);
        if ( resolved )
            // Found it.
            return resolved;

        if ( stop )
            // Pass back error.
            return std::move(resolved);

        if ( ! n->inheritScope() ) {
            // Advance to module scope directly.
            while ( n->parent() && (! n->parent()->isA<Module>()) )
                n = n->parent();
        }
    }

    return result::Error(util::fmt("unknown ID '%s'", id));
}
