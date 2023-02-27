// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/declarations/local-variable.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/statement.h>
#include <hilti/base/logger.h>

namespace hilti::statement {

/** AST node for a `while` statement. */
class While : public Statement {
public:
    auto init() const { return child<declaration::LocalVariable>(0); }
    auto condition() const { return child<::hilti::Expression>(1); }
    auto body() const { return child<hilti::Statement>(2); }
    auto else_() const { return child<Statement>(3); }

    static auto create(ASTContext* ctx, const DeclarationPtr& init, const ExpressionPtr& cond, const StatementPtr& body,
                       const StatementPtr& else_ = nullptr, const Meta& meta = {}) {
        return NodeDerivedPtr<While>(new While({init, cond, body, else_}, meta));
    }

    static auto create(ASTContext* ctx, const ExpressionPtr& cond, const StatementPtr& body, const Meta& meta = {}) {
        return create(ctx, nullptr, cond, body, nullptr, meta);
    }
    static auto create(ASTContext* ctx, const ExpressionPtr& cond, const StatementPtr& body,
                       const StatementPtr& else_ = nullptr, const Meta& meta = {}) {
        return create(ctx, nullptr, cond, body, else_, meta);
    }

protected:
    While(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {
        if ( ! child(0)->isA<declaration::LocalVariable>() )
            logger().internalError("initialization for 'while' must be a local declaration");
    }

    bool isEqual(const Node& other) const final { return other.isA<While>() && Statement::isEqual(other); }

    HILTI_NODE(While)
};

} // namespace hilti::statement
