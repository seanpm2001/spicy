// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/** AST node for an expression wrapped to have a specific type. */
class TypeWrapped : public Expression {
public:
    auto expression() const { return child<Expression>(0); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(1); }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const QualifiedTypePtr& type,
                       const Meta& meta = {}) {
        return NodeDerivedPtr<TypeWrapped>(new TypeWrapped({expr, type}, meta));
    }

protected:
    TypeWrapped(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<TypeWrapped>() && Expression::isEqual(other); }

    HILTI_NODE(TypeWrapped)
};

} // namespace hilti::expression
