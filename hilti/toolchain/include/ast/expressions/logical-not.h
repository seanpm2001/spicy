// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/bool.h>

namespace hilti::expression {

/** AST node for a logical "not" expression. */
class LogicalNot : public Expression {
public:
    auto expression() const { return child<Expression>(0); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(1); }
    bool isLhs() const final { return false; }
    bool isTemporary() const final { return true; }

    static auto create(ASTContext* ctx, const ExpressionPtr& expression, const Meta& meta = {}) {
        return NodeDerivedPtr<LogicalNot>(new LogicalNot({expression, type::Bool::create(ctx, meta)}, meta));
    }

protected:
    LogicalNot(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<LogicalNot>() && Expression::isEqual(other); }

    HILTI_NODE(LogicalNot)
};

} // namespace hilti::expression
