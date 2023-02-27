
// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

namespace set {

/** AST node for a set iterator type. */
class Iterator : public UnqualifiedType {
public:
    QualifiedTypePtr dereferencedType() const override { return child<QualifiedType>(0); }
    Nodes typeParameters() const override { return children(); }

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

    bool isEqual(const Node& other) const override { return other.isA<Iterator>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Iterator)
};

} // namespace set

/** AST node for a `set` type. */
class Set : public UnqualifiedType {
public:
    QualifiedTypePtr elementType() const final { return child<set::Iterator>(0)->dereferencedType(); }
    UnqualifiedTypePtr iteratorType() const final { return child<UnqualifiedType>(0); }
    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& t, const Meta& meta = {}) {
        return NodeDerivedPtr<Set>(new Set({set::Iterator::create(ctx, t, meta)}, meta));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Set>(new Set(Wildcard(), std::move(m)));
    }

protected:
    Set(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Set(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const override { return true; }
    bool _isMutable() const override { return true; }
    bool _isParameterized() const override { return true; }
    bool _isRuntimeNonTrivial() const override { return true; }

    bool isEqual(const Node& other) const override { return other.isA<Set>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Set)
};

} // namespace hilti::type
