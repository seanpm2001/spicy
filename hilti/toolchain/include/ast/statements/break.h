// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/statement.h>

namespace hilti::statement {

/** AST node for a `break` statement. */
class Break : public Statement {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) {
        return NodeDerivedPtr<Break>(new Break({}, std::move(meta)));
    }

protected:
    Break(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<Break>() && Statement::isEqual(other); }

    HILTI_NODE(Break)
};

} // namespace hilti::statement
