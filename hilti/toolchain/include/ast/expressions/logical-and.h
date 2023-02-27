// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/bool.h>

namespace hilti::expression {

/** AST node for a logical "and" expression. */
class LogicalAnd : public Expression {
public:
    auto op0() const { return child<Expression>(0); }
    auto op1() const { return child<Expression>(1); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(2); }
    bool isLhs() const final { return false; }
    bool isTemporary() const final { return true; }

    static auto create(ASTContext* ctx, const ExpressionPtr& op0, const ExpressionPtr& op1, const Meta& meta = {}) {
        return NodeDerivedPtr<LogicalAnd>(new LogicalAnd({op0, op1, type::Bool::create(ctx, meta)}, meta));
    }

protected:
    LogicalAnd(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<LogicalAnd>() && Expression::isEqual(other); }

    HILTI_NODE(LogicalAnd)
};

} // namespace hilti::expression
