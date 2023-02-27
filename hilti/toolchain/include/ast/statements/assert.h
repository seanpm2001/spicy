// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/statement.h>
#include <hilti/ast/type.h>

namespace hilti::statement {

namespace assert {
/**
 * Tag for `Assert` to constructor to create an assertion expecting an
 * exception to occur.
 */
struct Exception {};
} // namespace assert

/** AST node for an assert statement. */
class Assert : public Statement {
public:
    auto expression() const { return child<::hilti::Expression>(0); }
    auto exception() const { return child<UnqualifiedType>(1); }
    auto message() const { return child<::hilti::Expression>(2); }
    bool expectsException() const { return exception() != nullptr; }

    /**
     * Creates an assert statement that expects an expression to evaluate to true at runtime.
     *
     * @param e expression to evaluate at runtime
     * @param msg optional message to report an runtime if assertions fails
     * @param m meta information for AST node
     */
    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const ExpressionPtr& msg = nullptr, Meta meta = {}) {
        return NodeDerivedPtr<Assert>(new Assert({expr, nullptr, msg}, std::move(meta)));
    }

    /**
     * Creates an assert statement that expects an exception to occur when
     * the expression is evaluated.
     *
     * @param assert::Exception tag to select this constructor
     * @param e expression to evaluate at runtime
     * @param type exception type that's expected to be thrown when *e* is evaluated; unset of any exception
     * @param msg optional message to report an runtime if assertions fails
     * @param m meta information for AST node
     */
    static auto create(ASTContext* ctx, assert::Exception /*unused*/, const ExpressionPtr& expr,
                       const UnqualifiedTypePtr& excpt, const ExpressionPtr& msg = nullptr, const Meta& meta = {}) {
        return NodeDerivedPtr<Assert>(new Assert({expr, excpt, msg}, meta));
    }

protected:
    Assert(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<Assert>() && Statement::isEqual(other); }

    HILTI_NODE(Assert)
};

} // namespace hilti::statement
