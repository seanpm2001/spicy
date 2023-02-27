// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/statement.h>

namespace hilti::statement {

/** AST node for a `yield` statement. */
class Yield : public Statement {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) {
        return NodeDerivedPtr<Yield>(new Yield({}, std::move(meta)));
    }

protected:
    Yield(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<Yield>() && Statement::isEqual(other); }

    HILTI_NODE(Yield)
};

} // namespace hilti::statement
