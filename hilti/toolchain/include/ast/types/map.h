// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>
#include <hilti/ast/types/tuple.h>

#include "ast/forward.h"
#include "ast/type.h"

namespace hilti::type {

namespace map {

/** AST node for a map iterator type. */
class Iterator : public UnqualifiedType {
public:
    QualifiedTypePtr keyType() const {
        if ( auto t = child<type::Tuple>(0) )
            return t->elements()[0]->type();
        else
            return child<QualifiedType>(0);
    }

    QualifiedTypePtr valueType() const {
        if ( auto t = child<type::Tuple>(0) )
            return t->elements()[1]->type();
        else
            return child<QualifiedType>(0);
    }

    QualifiedTypePtr dereferencedType() const final { return valueType(); }
    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& ktype, const QualifiedTypePtr& vtype, Meta meta = {}) {
        return NodeDerivedPtr<Iterator>(new Iterator({ktype, vtype}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Iterator>(new Iterator(Wildcard(), std::move(m)));
    }

protected:
    Iterator(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Iterator(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isIterator() const final { return true; }
    bool _isMutable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isRuntimeNonTrivial() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final {
        return type::detail::isResolved(keyType(), rstate) && type::detail::isResolved(valueType(), rstate);
    }

    bool isEqual(const Node& other) const override { return other.isA<Iterator>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Iterator)
};

} // namespace map

/** AST node for a `map` type. */
class Map : public UnqualifiedType {
public:
    QualifiedTypePtr keyType() const { return child<map::Iterator>(0)->keyType(); }
    QualifiedTypePtr valueType() const { return child<map::Iterator>(0)->valueType(); }

    QualifiedTypePtr elementType() const final { return iteratorType()->dereferencedType(); }
    UnqualifiedTypePtr iteratorType() const final { return child<map::Iterator>(0); }
    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& ktype, const QualifiedTypePtr& vtype,
                       const Meta& meta = {}) {
        return NodeDerivedPtr<Map>(new Map({map::Iterator::create(ctx, ktype, vtype, meta)}, meta));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Map>(new Map(Wildcard(), std::move(m)));
    }

protected:
    Map(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Map(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isMutable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isRuntimeNonTrivial() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final { return type::detail::isResolved(iteratorType(), rstate); }

    bool isEqual(const Node& other) const override { return other.isA<Map>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Map)
};

} // namespace hilti::type
