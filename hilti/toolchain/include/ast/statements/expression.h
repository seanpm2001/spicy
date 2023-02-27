// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/statement.h>

namespace hilti::statement {

/** AST node for an expression statement. */
class Expression : public Statement {
public:
    auto expression() const { return child<hilti::Expression>(0); }

    static auto create(ASTContext* ctx, const ExpressionPtr& e, Meta meta = {}) {
        return NodeDerivedPtr<Expression>(new Expression({e}, std::move(meta)));
    }

protected:
    Expression(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final {
        return other.isA<::hilti::Expression>() && Statement::isEqual(other);
    }

    HILTI_NODE(Expression)
};

} // namespace hilti::statement
