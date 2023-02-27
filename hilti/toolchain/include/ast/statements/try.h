// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/declarations/parameter.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/statement.h>
#include <hilti/base/logger.h>

namespace hilti::statement {

namespace try_ {

/** AST node for a `catch` block. */
class Catch final : public Node {
public:
    ~Catch() final;

    auto parameter() const { return child<declaration::Parameter>(0); }
    auto body() const { return child<hilti::Statement>(1); }

    static auto create(ASTContext* ctx, const DeclarationPtr& param, const StatementPtr& body, Meta meta = {}) {
        return NodeDerivedPtr<Catch>(new Catch({param, body}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, const StatementPtr& body, Meta meta = {}) {
        return NodeDerivedPtr<Catch>(new Catch({nullptr, body}, std::move(meta)));
    }

protected:
    Catch(Nodes children, Meta meta = {}) : Node(std::move(children), std::move(meta)) {
        if ( ! child(0)->isA<declaration::Parameter>() )
            logger().internalError("'catch' first child must be parameter");
    }

    std::string _render() const final;

    bool isEqual(const Node& other) const override { return other.isA<Catch>() && Node::isEqual(other); }

    HILTI_NODE(Catch);
};

using CatchPtr = std::shared_ptr<Catch>;
using Catches = std::vector<CatchPtr>;

} // namespace try_

/** AST node for a `try` statement. */
class Try : public Statement {
public:
    auto body() const { return child<hilti::Statement>(0); }
    auto catches() const { return children<try_::Catch>(1, -1); }

    static auto create(ASTContext* ctx, StatementPtr body, const try_::Catches& catches, Meta meta = {}) {
        return NodeDerivedPtr<Try>(new Try(node::flatten(std::move(body), catches), std::move(meta)));
    }

protected:
    Try(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<Try>() && Statement::isEqual(other); }

    HILTI_NODE(Try)
};

} // namespace hilti::statement
