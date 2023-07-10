// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/local-variable.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/list.h>

namespace hilti::expression {

/** AST node for a vector comprehension expression. */
class ListComprehension : public Expression {
public:
    auto input() const { return child<Expression>(0); }
    auto output() const { return child<Expression>(1); }
    auto local() const { return child<declaration::LocalVariable>(2); }
    auto condition() const { return child<Expression>(3); }

    /**
     * Returns the output expression's scope. Note that the scope is shared
     * among any copies of an instance.
     */
    auto scope() const { return output()->scope(); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(4); }

    static auto create(ASTContext* ctx, const ExpressionPtr& input, const ExpressionPtr& output, const ID& id,
                       const ExpressionPtr& cond, const Meta& meta = {}) {
        auto local = declaration::LocalVariable::create(ctx, id, QualifiedType::createAuto(ctx, meta), meta);
        auto list = type::List::create(ctx, QualifiedType::createAuto(ctx, meta), meta);
        return NodeDerivedPtr<ListComprehension>(new ListComprehension({input, output, local, cond, list}, meta));
    }

protected:
    ListComprehension(Nodes children, Meta meta) : Expression(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override {
        return other.isA<ListComprehension>() && Expression::isEqual(other);
    }

    HILTI_NODE(ListComprehension)
};

} // namespace hilti::expression
