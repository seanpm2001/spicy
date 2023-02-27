// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/operator.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/** AST node for an expression representing an unresolved operator usage. */
class UnresolvedOperator : public Expression {
public:
    auto kind() const { return _kind; }
    bool areOperandsResolved() const { return expression::isResolved(children<Expression>(1, -1)); }

    /** Implements interface for use with `OverloadRegistry`. */
    hilti::node::Range<Expression> operands() const { return children<Expression>(1, -1); }

    // Dummy implementations as the node will be rejected in validation anyway.
    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }
    bool isLhs() const final { return false; }
    bool isTemporary() const final { return true; }

    node::Properties properties() const final {
        auto p = node::Properties{{"kind", to_string(_kind)}};
        return Expression::properties() + p;
    }

    static auto create(ASTContext* ctx, operator_::Kind kind, Expressions operands, const Meta& meta = {}) {
        return ExpressionPtr(
            new UnresolvedOperator(node::flatten(QualifiedType::createAuto(ctx, meta), std::move(operands)), kind,
                                   meta));
    }

    static auto create(ASTContext* ctx, operator_::Kind kind, hilti::node::Range<Expression> operands,
                       const Meta& meta = {}) {
        return ExpressionPtr(
            new UnresolvedOperator(node::flatten(QualifiedType::createAuto(ctx, meta), operands), kind, meta));
    }

protected:
    UnresolvedOperator(Nodes children, operator_::Kind kind, Meta meta)
        : Expression(std::move(children), std::move(meta)), _kind(kind) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<UnresolvedOperator>();
        if ( ! n )
            return false;

        return Expression::isEqual(other) && _kind == n->_kind;
    }

    HILTI_NODE(UnresolvedOperator)

private:
    operator_::Kind _kind;
};

} // namespace hilti::expression
