// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/optional.h>

namespace hilti::ctor {

/** AST node for a `optional` ctor. */
class Optional : public Ctor {
public:
    QualifiedTypePtr type() const final {
        if ( auto e = child(0) )
            return child<QualifiedType>(0);
        else
            return child<Expression>(1)->type();
    }

    QualifiedTypePtr dereferencedType() const { return type()->type()->as<type::Optional>()->dereferencedType(); }
    ExpressionPtr value() const { return child<Expression>(1); }

    /** Constructs a set value. */
    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const Meta& meta = {}) {
        return NodeDerivedPtr<Optional>(new Optional(
            {
                nullptr,
                expr,
            },
            meta));
    }

    /** Constructs an unset value of type `t`. */
    static auto create(ASTContext* ctx, const QualifiedTypePtr& type, const Meta& meta = {}) {
        return NodeDerivedPtr<Optional>(new Optional(
            {
                type,
                nullptr,
            },
            meta));
    }

protected:
    Optional(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Optional>() && Ctor::isEqual(other); }

    HILTI_NODE(Optional)
};

} // namespace hilti::ctor
