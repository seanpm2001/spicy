// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/** AST node for a constructor expression. */
class Ctor : public Expression {
public:
    auto ctor() const { return child<hilti::Ctor>(0); }

    QualifiedTypePtr type() const final { return ctor()->type(); }
    bool isLhs() const final { return ctor()->isLhs(); }
    bool isTemporary() const final { return ctor()->isTemporary(); }

    static auto create(ASTContext* ctx, const CtorPtr& ctor, const Meta& meta = {}) {
        return NodeDerivedPtr<Ctor>(new Ctor({ctor}, meta));
    }

protected:
    Ctor(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Ctor>() && Expression::isEqual(other); }

    HILTI_NODE(Ctor)
};

} // namespace hilti::expression
