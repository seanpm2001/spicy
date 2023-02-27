// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

namespace vector {

/** AST node for a vector iterator type. */
class Iterator : public UnqualifiedType {
public:
    QualifiedTypePtr dereferencedType() const final { return child<QualifiedType>(0); }
    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& etype, Meta meta = {}) {
        return NodeDerivedPtr<Iterator>(new Iterator({etype}, std::move(meta)));
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
    bool _isResolved(ResolvedState* rstate) const final { return type::detail::isResolved(dereferencedType(), rstate); }

    bool isEqual(const Node& other) const final { return other.isA<Iterator>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Iterator)
};

} // namespace vector

/** AST node for a `vector` type. */
class Vector : public UnqualifiedType {
public:
    QualifiedTypePtr elementType() const final { return child<vector::Iterator>(0)->dereferencedType(); }
    UnqualifiedTypePtr iteratorType() const final { return child<UnqualifiedType>(0); }
    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& t, const Meta& meta = {}) {
        return NodeDerivedPtr<Vector>(new Vector({vector::Iterator::create(ctx, t, meta)}, meta));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Vector>(new Vector(Wildcard(), std::move(m)));
    }

protected:
    Vector(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Vector(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isMutable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isRuntimeNonTrivial() const final { return true; }

    bool isEqual(const Node& other) const final { return other.isA<Vector>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Vector)
};

} // namespace hilti::type
