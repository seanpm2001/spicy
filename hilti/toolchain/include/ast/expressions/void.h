// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/void.h>

namespace hilti::expression {

/** AST node for a void expression. */
class Void : public Expression {
public:
    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }
    bool isLhs() const final { return false; }
    bool isTemporary() const final { return true; }

    static auto create(ASTContext* ctx, const Meta& meta = {}) {
        return NodeDerivedPtr<Void>(new Void({type::Void::create(ctx, meta)}, meta));
    }

protected:
    Void(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Void>() && Expression::isEqual(other); }

    HILTI_NODE(Void)
};

} // namespace hilti::expression
