// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/all.h> // TODO: Remove, just to trigger compilation of headers during development.
#include <hilti/ast/type.h>
#include <hilti/ast/types/all.h>

using namespace hilti;

UnqualifiedType::~UnqualifiedType() {}

std::string UnqualifiedType::_render() const {
    std::vector<std::string> x;

    if ( isWildcard() )
        x.emplace_back("wildcard");

    return util::join(x);
}

std::string QualifiedType::_render() const {
    std::vector<std::string> x;

    if ( isWildcard() )
        x.emplace_back("wildcard");

    x.emplace_back(type::isResolved(this) ? "(resolved)" : "(not resolved)");
    x.emplace_back(_is_constant ? "(const)" : "(non-const)");

    return util::join(x);
}

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
    return QualifiedTypePtr(new QualifiedType({type::Auto::create(ctx, m)}, false, true, Side::RHS, m));
}


namespace {

class Unifier : public visitor::PreOrder {
public:
    Unifier(ASTContext* ctx, const NodePtr& scope_node) : ctx(ctx), scope_node(scope_node) {}

    ASTContext* ctx;
    const NodePtr& scope_node;

    type::Unified u;

    void operator()(QualifiedType* t) final { u.is_constant = t->isConstant(); }
    void operator()(type::Auto* t) final {
        static uint64_t cnt = 0;
        u.serial += ID(util::fmt("auto%" PRIu64, ++cnt));
    }
    void operator()(type::Bool* t) final { u.serial += ID("bool"); }
    void operator()(type::SignedInteger* t) final { u.serial += ID(util::fmt("int%" PRIu64, t->width())); }
    void operator()(type::UnsignedInteger* t) final { u.serial += ID(util::fmt("uint%" PRIu64, t->width())); }
    void operator()(type::Enum* t) final { u.serial += ID("enum"); } // TODO: Serialize labels
    void operator()(type::String* t) final { u.serial += ID("string"); }
    void operator()(type::Void* t) final { u.serial += ID("void"); }
    void operator()(type::Any* t) final { u.serial += ID("any"); }
    void operator()(type::Time* t) final { u.serial += ID("time"); }
    void operator()(type::Tuple* t) final { u.serial += ID(util::fmt("tuple")); } // TODO: Serialize elements
    void operator()(type::OperandList* t) final { u.serial += ID("operand-list"); }
    void operator()(type::Library* t) final { u.serial += ID(util::fmt("library-%s", t->cxxName())); }
    void operator()(type::Exception* t) final {
        u.serial += ID(util::fmt("exception-XXX")); // TODO: Serialize base type
    }
    void operator()(type::Name* t) final {
        if ( t->declaration() && t->declaration()->canonicalID() )
            u.serial += ID(util::fmt("name-%s", t->declaration()->canonicalID()));
        else {
            static uint64_t cnt = 0;
            u.serial += ID(util::fmt("name-unknown-%" PRIu64, ++cnt));
        }
    }
};
} // namespace

void QualifiedType::unify(ASTContext* ctx, const NodePtr& scope_root) {
    if ( _unified )
        return;

    NodePtr root = this->shared_from_this();
    _unified = visitor::visit(Unifier(ctx, scope_root ? scope_root : root), root, [](const auto& v) { return v.u; });

    for ( const auto& c : children() )
        unifyTypes(ctx, c);
}

namespace {
struct TypeUnifier : visitor::PreOrder {
    TypeUnifier(ASTContext* ctx) : ctx(ctx) {}
    ASTContext* ctx;

    void operator()(QualifiedType* t) final { t->unify(ctx); }
};
} // namespace

void QualifiedType::unifyTypes(ASTContext* ctx, const NodePtr& root) { hilti::visitor::visit(TypeUnifier(ctx), root); }
