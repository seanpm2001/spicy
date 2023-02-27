// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

#include "ast/forward.h"

namespace hilti::type {

/** AST node for a `strong_ref<T>` type. */
class StrongReference : public UnqualifiedType {
public:
    QualifiedTypePtr dereferencedType() const final { return child(0)->as<QualifiedType>(); }

    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& type, Meta meta = {}) {
        return NodeDerivedPtr<StrongReference>(new StrongReference({type}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<StrongReference>(new StrongReference(Wildcard(), std::move(m)));
    }

protected:
    StrongReference(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    StrongReference(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isReferenceType() const final { return true; }

    bool _isResolved(ResolvedState* rstate) const final { return type::detail::isResolved(dereferencedType(), rstate); }

    bool isEqual(const Node& other) const override {
        return other.isA<StrongReference>() && UnqualifiedType::isEqual(other);
    }

    HILTI_NODE(StrongReference)
};

/** AST node for a `weak_ref<T>` type. */
class WeakReference : public UnqualifiedType {
public:
    QualifiedTypePtr dereferencedType() const final { return child(0)->as<QualifiedType>(); }

    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& type, Meta meta = {}) {
        return NodeDerivedPtr<WeakReference>(new WeakReference({type}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<WeakReference>(new WeakReference(Wildcard(), std::move(m)));
    }

protected:
    WeakReference(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    WeakReference(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isReferenceType() const final { return true; }

    bool _isResolved(ResolvedState* rstate) const final { return type::detail::isResolved(dereferencedType(), rstate); }

    bool isEqual(const Node& other) const override {
        return other.isA<WeakReference>() && UnqualifiedType::isEqual(other);
    }

    HILTI_NODE(WeakReference)
};

/** AST node for a `value_ref<T>` type. */
class ValueReference : public UnqualifiedType {
public:
    QualifiedTypePtr dereferencedType() const final { return child(0)->as<QualifiedType>(); }

    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& type, Meta meta = {}) {
        return NodeDerivedPtr<ValueReference>(new ValueReference({type}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<ValueReference>(new ValueReference(Wildcard(), std::move(m)));
    }

protected:
    ValueReference(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    ValueReference(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isReferenceType() const final { return true; }

    bool _isResolved(ResolvedState* rstate) const final { return type::detail::isResolved(dereferencedType(), rstate); }

    bool isEqual(const Node& other) const override {
        return other.isA<ValueReference>() && UnqualifiedType::isEqual(other);
    }

    HILTI_NODE(ValueReference)
};

} // namespace hilti::type
