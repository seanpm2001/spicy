// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/field.h>
#include <hilti/ast/function.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/function.h>

namespace hilti::type {

/** AST node for a `union` type. */
class Union : public UnqualifiedType {
public:
    auto fields() const { return childrenOfType<declaration::Field>(); }

    declaration::FieldPtr field(const ID& id) const {
        for ( const auto& f : fields() ) {
            if ( f->id() == id )
                return f;
        }

        return {};
    }

    hilti::node::Set<declaration::Field> fields(const ID& id) const {
        hilti::node::Set<declaration::Field> x;
        for ( const auto& f : fields() ) {
            if ( f->id() == id )
                x.insert(f);
        }

        return x;
    }

    auto hasFinalizer() const { return field("~finally") != nullptr; }

    Nodes typeParameters() const final {
        Nodes params;
        for ( const auto& f : fields() )
            params.emplace_back(f->type());

        return params;
    }

    hilti::node::Set<type::function::Parameter> parameters() const final {
        return childrenOfType<type::function::Parameter>();
    }

    void addField(DeclarationPtr f) {
        assert(f->isA<declaration::Field>());
        addChild(std::move(f));
    }

    static auto create(ASTContext* ctx, const declaration::Parameters& params, Declarations fields,
                       const Meta& meta = {}) {
        for ( auto&& p : params )
            p->setIsTypeParameter();

        return NodeDerivedPtr<Union>(new Union(node::flatten(params, std::move(fields)), -1, meta));
    }

    static auto create(ASTContext* ctx, const Declarations& fields, const Meta& meta = {}) {
        return create(ctx, declaration::Parameters{}, fields, meta);
    }

    union AnonymousUnion {};
    static auto create(ASTContext* ctx, AnonymousUnion _, Declarations fields, const Meta& meta = {}) {
        return NodeDerivedPtr<Union>(new Union(std::move(fields), ++_anon_union_counter, meta));
    }

    static auto create(ASTContext* ctx, Wildcard _, const Meta& meta = {}) {
        return NodeDerivedPtr<Union>(new Union(Wildcard(), meta));
    }

protected:
    Union(const Nodes& children, int64_t anon_union, const Meta& meta)
        : UnqualifiedType(children, meta), _anon_union(anon_union) {}

    Union(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isMutable() const final { return true; }
    bool _isParameterized() const final { return true; }

    bool _isResolved(ResolvedState* rstate) const final {
        const auto& cs = children();

        return std::all_of(cs.begin(), cs.end(), [&](const auto& c) {
            if ( auto f = c->template tryAs<declaration::Field>() )
                return f->isResolved(rstate);

            else if ( auto p = c->template tryAs<type::function::Parameter>() )
                return p->isResolved(rstate);

            return true;
        });
    }

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Union>();
        if ( ! n )
            return false;

        // Anonymous unions only compare successfully to themselves.
        if ( _anon_union >= 0 || n->_anon_union >= 0 )
            return _anon_union == n->_anon_union;

        return UnqualifiedType::isEqual(other);
    }

    HILTI_NODE(Union)

private:
    int64_t _anon_union = -1;

    static int64_t _anon_union_counter;
};

} // namespace hilti::type
