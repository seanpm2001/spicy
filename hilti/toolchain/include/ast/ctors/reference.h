// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/reference.h>

#include "ast/expression.h"
#include "ast/types/auto.h"
#include "ast/types/reference.h"

namespace hilti::ctor {

/** AST node for a `strong_ref<T>` constructor value (which can only be null). */
class StrongReference : public Ctor {
public:
    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }
    QualifiedTypePtr dereferencedType() const {
        return type()->type()->as<type::StrongReference>()->dereferencedType();
    }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& t, const Meta& meta = {}) {
        return CtorPtr(
            new StrongReference({QualifiedType::create(ctx, type::StrongReference::create(ctx, t, meta), true)}, meta));
    }

protected:
    StrongReference(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<StrongReference>() && Ctor::isEqual(other); }

    HILTI_NODE(StrongReference)
};

/** AST node for a `weak_ref<T>` constructor value (which can only be null). */
class WeakReference : public Ctor {
public:
    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }
    QualifiedTypePtr dereferencedType() const { return type()->type()->as<type::WeakReference>()->dereferencedType(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& t, const Meta& meta = {}) {
        return CtorPtr(
            new WeakReference({QualifiedType::create(ctx, type::WeakReference::create(ctx, t, meta), true)}, meta));
    }

protected:
    WeakReference(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<WeakReference>() && Ctor::isEqual(other); }

    HILTI_NODE(WeakReference)
};

/** AST node for a `value_ref<T>` constructor value. */
class ValueReference : public Ctor {
public:
    ExpressionPtr expression() const { return child<Expression>(0); }
    QualifiedTypePtr type() const final { return expression()->type(); }
    QualifiedTypePtr dereferencedType() const { return type()->type()->as<type::ValueReference>()->dereferencedType(); }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const Meta& meta = {}) {
        return NodeDerivedPtr<ValueReference>(new ValueReference({expr}, meta));
    }

protected:
    ValueReference(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<ValueReference>() && Ctor::isEqual(other); }

    HILTI_NODE(ValueReference)
};

} // namespace hilti::ctor
