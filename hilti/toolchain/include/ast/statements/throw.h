// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/statement.h>

namespace hilti::statement {

/** AST node for a `throw` statement. */
class Throw : public Statement {
public:
    auto expression() const { return child<::hilti::Expression>(0); }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, Meta meta = {}) {
        return NodeDerivedPtr<Throw>(new Throw({expr}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, Meta meta = {}) {
        return NodeDerivedPtr<Throw>(new Throw({nullptr}, std::move(meta)));
    }

protected:
    Throw(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<Throw>() && Statement::isEqual(other); }

    HILTI_NODE(Throw)
};

} // namespace hilti::statement
