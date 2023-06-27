// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/** AST node for an expression that's being coerced from one type to another. */
class Coerced : public Expression {
public:
    auto expression() const { return child<Expression>(0); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(1); }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const QualifiedTypePtr& target,
                       const Meta& meta = {}) {
        return NodeDerivedPtr<Coerced>(new Coerced({expr, target}, meta));
    }

protected:
    Coerced(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Coerced>() && Expression::isEqual(other); }

    HILTI_NODE(Coerced)
};

} // namespace hilti::expression
