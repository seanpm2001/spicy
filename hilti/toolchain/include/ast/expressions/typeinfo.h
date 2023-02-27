// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/** AST node for a `typeinfo` expression. */
class TypeInfo : public Expression {
public:
    auto expression() const { return child<Expression>(0); }

    QualifiedTypePtr type() const final { return expression()->type(); }
    bool isLhs() const final { return false; }
    bool isTemporary() const final { return true; }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const Meta& meta = {}) {
        return NodeDerivedPtr<TypeInfo>(new TypeInfo({expr}, meta));
    }

protected:
    TypeInfo(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<TypeInfo>() && Expression::isEqual(other); }

    HILTI_NODE(TypeInfo)
};

} // namespace hilti::expression
