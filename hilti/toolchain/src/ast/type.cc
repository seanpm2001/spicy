// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/all.h> // TODO: Remove, just to trigger compilation of headers during development.
#include <hilti/ast/type.h>
#include <hilti/ast/types/auto.h>

using namespace hilti;

UnqualifiedType::~UnqualifiedType() {}

std::string UnqualifiedType::_render() const {
    std::vector<std::string> x;

    if ( isWildcard() )
        x.emplace_back("wildcard");

    x.emplace_back(type::isResolved(*this) ? "(resolved)" : "(not resolved)");

    return util::join(x);
}

std::string QualifiedType::_render() const { return _is_constant ? "(const)" : "(non-const)"; }

bool type::isResolved(const UnqualifiedType& t) {
    ResolvedState rstate;
    return detail::isResolved(t, &rstate);
}

bool type::detail::isResolved(const hilti::UnqualifiedType& t, ResolvedState* rstate) {
    if ( ! rstate )
        return type::isResolved(t);

    if ( t._isParameterized() ) {
        if ( rstate->find(t.identity()) != rstate->end() )
            return true;

        rstate->insert(t.identity());
    }

    return t._isResolved(rstate);
}

void type::detail::applyPruneWalk(UnqualifiedType* t) {
    // We prune at the types that have an ID, as only they can create cycles.
    if ( t->typeID() ) {
        t->setPruneWalk(true);
        return;
    }

    for ( auto&& c : t->children() ) {
        if ( auto x = c->tryAs<hilti::UnqualifiedType>() )
            applyPruneWalk(x.get());
    }
}

QualifiedTypePtr QualifiedType::createAuto(ASTContext* ctx, const Meta& m) {
    // Note: We allow (i.e., must support) `ctx` being null.
    return QualifiedTypePtr(new QualifiedType({type::Auto::create(ctx, m)}, false, true, m));
}
