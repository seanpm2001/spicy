// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/statement.h>

namespace hilti::statement {

/** AST node for a block statement. */
class Block : public Statement {
public:
    auto statements() const { return childrenOfType<Statement>(); }

    void add(StatementPtr s) { addChild(std::move(s)); }

    static auto create(ASTContext* ctx, Statements stmts = {}, Meta meta = {}) {
        return NodeDerivedPtr<Block>(new Block(std::move(stmts), std::move(meta)));
    }

    static auto create(ASTContext* ctx, Meta meta = {}) { return create(ctx, {}, std::move(meta)); }

protected:
    Block(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<Block>() && Statement::isEqual(other); }

    HILTI_NODE(Block)
};

} // namespace hilti::statement
