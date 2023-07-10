// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/type.h>

namespace hilti::expression {

/** AST node for a type expression. */
class Type_ : public Expression {
public:
    auto typeValue() const { return type()->type()->as<type::Type_>()->typeValue(); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& type, const Meta& meta = {}) {
        return NodeDerivedPtr<Type_>(new Type_({type::Type_::create(ctx, type, meta)}, meta));
    }

protected:
    Type_(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Type_>() && Expression::isEqual(other); }

    HILTI_NODE(Type_)
};

} // namespace hilti::expression
