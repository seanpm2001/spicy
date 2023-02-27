// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/statement.h>

namespace hilti::statement {

class SetLocation : public Statement {
public:
    auto expression() const { return child<::hilti::Expression>(0); }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, Meta meta = {}) {
        return NodeDerivedPtr<SetLocation>(new SetLocation({expr}, std::move(meta)));
    }

protected:
    SetLocation(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<SetLocation>() && Statement::isEqual(other); }

    HILTI_NODE(SetLocation)
};

} // namespace hilti::statement
