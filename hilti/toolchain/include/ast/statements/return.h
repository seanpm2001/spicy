// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/statement.h>

namespace hilti::statement {

/** AST node for a `return` statement. */
class Return : public Statement {
public:
    auto expression() const { return child<::hilti::Expression>(0); }

    static auto create(ASTContext* ctx, ExpressionPtr expr, Meta meta = {}) {
        return NodeDerivedPtr<Return>(new Return({expr}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, Meta meta = {}) { return create(ctx, nullptr, std::move(meta)); }

protected:
    Return(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<Return>() && Statement::isEqual(other); }

    HILTI_NODE(Return)
};

} // namespace hilti::statement
