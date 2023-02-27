// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/statement.h>

#include "ast/forward.h"

namespace hilti::statement {

/** AST node for a `continue` statement. */
class Continue : public Statement {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) {
        return NodeDerivedPtr<Continue>(new Continue({}, std::move(meta)));
    }

protected:
    Continue(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<Continue>() && Statement::isEqual(other); }

    HILTI_NODE(Continue)
};

} // namespace hilti::statement
