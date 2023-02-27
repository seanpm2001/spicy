// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/declarations/local-variable.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/statement.h>

namespace hilti::statement {

/** AST node for a `for` statement. */
class For : public Statement {
public:
    auto local() const { return child<hilti::declaration::LocalVariable>(0); }
    auto sequence() const { return child<::hilti::Expression>(1); }
    auto body() const { return child<hilti::Statement>(2); }

    static auto create(ASTContext* ctx, const hilti::ID& id, const ExpressionPtr& seq, const StatementPtr& body,
                       Meta meta = {}) {
        auto local = declaration::LocalVariable::create(ctx, id, meta);
        return NodeDerivedPtr<For>(new For({local, seq, body}, std::move(meta)));
    }

protected:
    For(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<For>() && Statement::isEqual(other); }

    HILTI_NODE(For)
};

} // namespace hilti::statement
