// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/** AST node for a assignment expression. */
class Assign : public Expression {
public:
    auto target() const { return child<Expression>(0); }
    auto source() const { return child<Expression>(1); }

    QualifiedTypePtr type() const final { return target()->type(); }
    bool isLhs() const final { return target()->isLhs(); }
    bool isTemporary() const final { return isLhs(); }

    static auto create(ASTContext* ctx, const ExpressionPtr& target, const ExpressionPtr& src, const Meta& meta = {}) {
        return NodeDerivedPtr<Assign>(new Assign({target, src}, meta));
    }

protected:
    Assign(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Assign>() && Expression::isEqual(other); }

    HILTI_NODE(Assign)
};

} // namespace hilti::expression
