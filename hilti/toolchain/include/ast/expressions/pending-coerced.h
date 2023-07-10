// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/** AST node for an expression that will be coerced from one type to another.
 *  The actual coercion expression will be generated later and replace the
 *  this node during the apply-coercions phase.
 */
class PendingCoerced : public Expression {
public:
    auto expression() const { return child<Expression>(0); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(1); }
    bool isLhs() const final { return expression()->isLhs(); }
    bool isTemporary() const final { return expression()->isTemporary(); }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const QualifiedTypePtr& type,
                       const Meta& meta = {}) {
        return NodeDerivedPtr<PendingCoerced>(new PendingCoerced({expr, type}, meta));
    }

protected:
    PendingCoerced(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<PendingCoerced>() && Expression::isEqual(other); }

    HILTI_NODE(PendingCoerced)
};

} // namespace hilti::expression
