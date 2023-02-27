// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/** AST node for a ternary expression. */
class Ternary : public Expression {
public:
    auto condition() const { return child<Expression>(0); }
    auto true_() const { return child<Expression>(1); }
    auto false_() const { return child<Expression>(2); }

    QualifiedTypePtr type() const final {
        // TODO(robin): Currently we enforce both having the same type; we
        // might need to coerce to target type though.
        return true_()->type();
    }

    bool isLhs() const final { return false; }
    bool isTemporary() const final { return true_()->isTemporary() || false_()->isTemporary(); }

    static auto create(ASTContext* ctx, const ExpressionPtr& cond, const ExpressionPtr& true_,
                       const ExpressionPtr& false_, const Meta& meta = {}) {
        return NodeDerivedPtr<Ternary>(new Ternary({cond, true_, false_}, meta));
    }

protected:
    Ternary(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Ternary>() && Expression::isEqual(other); }

    HILTI_NODE(Ternary)
};

} // namespace hilti::expression
