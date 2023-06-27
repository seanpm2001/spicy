// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/**
 * AST node for an expression for which evaluation is deferred at runtime to
 * a later point when explicitly requested by the runtime system. Optionally,
 * that later evaluation can catch any exceptions and return a corresponding
 * ``result<T>``.
 */
class Deferred : public Expression {
public:
    auto expression() const { return child<Expression>(0); }
    bool catchException() const { return _catch_exception; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(1); }

    node::Properties properties() const final {
        auto p = node::Properties{{"catch_exception", _catch_exception}};
        return Expression::properties() + p;
    }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, bool catch_exception, const Meta& meta = {}) {
        return NodeDerivedPtr<Deferred>(
            new Deferred({expr, QualifiedType::createAuto(ctx, meta)}, catch_exception, meta));
    }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const Meta& meta = {}) {
        return create(ctx, expr, false, meta);
    }

protected:
    Deferred(Nodes children, bool catch_exception, Meta meta)
        : Expression(std::move(children), std::move(meta)), _catch_exception(catch_exception) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Deferred>();
        if ( ! n )
            return false;

        return Expression::isEqual(other) && _catch_exception == n->_catch_exception;
    }

    HILTI_NODE(Deferred)

private:
    bool _catch_exception;
};

} // namespace hilti::expression
