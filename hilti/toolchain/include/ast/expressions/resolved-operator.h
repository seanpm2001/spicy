// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/operator.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/**
 * Base class for an AST node for an expression representing a resolved operator usage.
 *
 * @note Typically, one derives from this only by using the `__BEGIN_OPERATOR` macro.
 */
class ResolvedOperator : public Expression {
public:
    const auto& operator_() const { return *_operator; }
    auto kind() const { return _operator->kind(); }

    // ResolvedOperator interface with common implementations.
    auto operands() const { return children<Expression>(1, -1); }
    auto result() const { return child<QualifiedType>(0); }
    auto op0() const { return child<Expression>(1); }
    auto op1() const { return child<Expression>(2); }
    auto op2() const { return child<Expression>(3); }
    auto hasOp0() const { return children().size() >= 2; }
    auto hasOp1() const { return children().size() >= 3; }
    auto hasOp2() const { return children().size() >= 4; }

#if 0
    void setOp0(const ExpressionPtr& e) { setChild(1, e); }
    void setOp1(const ExpressionPtr& e) { setChild(2, e); }
    void setOp2(const ExpressionPtr& e) { setChild(3, e); }
#endif

    QualifiedTypePtr type() const final { return result(); }

    node::Properties properties() const final {
        auto p = node::Properties{{"kind", to_string(_operator->kind())}};
        return Expression::properties() + p;
    }

protected:
    ResolvedOperator(const Operator* op, const QualifiedTypePtr& result, const Expressions& operands, Meta meta)
        : Expression(node::flatten(result, operands), std::move(meta)), _operator(op) {
        type::pruneWalk(child(0)->as<QualifiedType>());
    }

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<ResolvedOperator>();
        if ( ! n )
            return false;

        return Expression::isEqual(other) && _operator == n->_operator;
    }

private:
    const Operator* _operator = nullptr;
};

namespace resolved_operator {

#if 0
/**
 * Copies an existing resolved operator, replacing its 1st operand with a different expression.
 *
 * @param r original operator
 * @param e new operand
 * @return new resolved operator with the 1st operand replaced
 */
inline ExpressionPtr setOp0(const expression::ResolvedOperator& r, ExpressionPtr e) {
    auto x = r._clone().as<expression::ResolvedOperator>();
     x;
}

/**
 * Copies an existing resolved operator, replacing its 2nd operand with a different expression.
 *
 * @param r original operator
 * @param e new operand
 * @return new resolved operator with the 2nd operand replaced
 */
inline ExpressionPtr setOp1(const expression::ResolvedOperator& r, ExpressionPtr e) {
    auto x = r._clone().as<expression::ResolvedOperator>();
    x.setOp1(std::move(e));
    return x;
}

/**
 * Copies an existing resolved operator, replacing its 3rd operand with a different expression.
 *
 * @param r original operator
 * @param e new operand
 * @return new resolved operator with the 3rd operand replaced
 */
inline ExpressionPtr setOp2(const expression::ResolvedOperator& r, ExpressionPtr e) {
    auto x = r._clone().as<expression::ResolvedOperator>();
    x.setOp2(std::move(e));
    return x;
}
#endif

} // namespace resolved_operator
} // namespace hilti::expression
