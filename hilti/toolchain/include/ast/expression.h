// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <hilti/ast/forward.h>
#include <hilti/ast/node.h>
#include <hilti/ast/type.h>

namespace hilti {

namespace type {
using ResolvedState = std::unordered_set<uintptr_t>;
}

/** Base class for expression nodes. */
class Expression : public Node {
public:
    ~Expression() override;

    auto isConstant() const { return type()->isConstant(); }

    /** Returns the expression's HILTI type when evaluated. */
    virtual QualifiedTypePtr type() const = 0;

protected:
    Expression(Nodes children, Meta meta) : Node::Node(std::move(children), std::move(meta)) {}

    std::string _render() const override;
    bool isEqual(const Node& other) const override { return other.isA<Expression>() && Node::isEqual(other); }

    HILTI_NODE_BASE(Type);
};

namespace expression {

/**
 * Returns true if the type of an expression has been resolved.
 *
 * @param e expression to check
 * @param rstate internal parameter, leave unset
 */
bool isResolved(const Expression& e, type::ResolvedState* rstate = nullptr);

/**
 * Returns true if the type of an expression has been resolved.
 *
 * @param e expression to check
 * @param rstate internal parameter, leave unset
 */
inline bool isResolved(const ExpressionPtr& e, type::ResolvedState* rstate = nullptr) { return isResolved(*e, rstate); }

/**
 * Returns true if the types of all expressions in a vector have been resolved.
 *
 * @param exprs expressions expressions to check
 * @param rstate internal parameter, leave unset
 */
inline bool isResolved(const Expressions& exprs, type::ResolvedState* rstate = nullptr) {
    return std::all_of(exprs.begin(), exprs.end(), [&](const auto& e) { return isResolved(e, rstate); });
}

/**
 * Returns true if the types of all expressions in a range have been resolved.
 *
 * @param exprs expressions expressions to check
 * @param rstate internal parameter, leave unset
 */
inline bool isResolved(const hilti::node::Range<Expression>& exprs, type::ResolvedState* rstate = nullptr) {
    return std::all_of(exprs.begin(), exprs.end(), [&](const auto& e) { return isResolved(e, rstate); });
}

/**
 * Returns true if the types of all expressions in a set have been resolved.
 *
 * @param exprs expressions expressions to check
 * @param rstate internal parameter, leave unset
 */
inline bool isResolved(const hilti::node::Set<Expression>& exprs, type::ResolvedState* rstate = nullptr) {
    return std::all_of(exprs.begin(), exprs.end(), [&](const auto& e) { return isResolved(e, rstate); });
}

} // namespace expression
} // namespace hilti
