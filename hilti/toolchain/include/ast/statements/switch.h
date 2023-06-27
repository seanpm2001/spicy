// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/declarations/local-variable.h>
#include <hilti/ast/declarations/parameter.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/expressions/name.h>
#include <hilti/ast/expressions/unresolved-operator.h>
#include <hilti/ast/statement.h>
#include <hilti/base/logger.h>

namespace hilti::statement {

class Switch;

namespace switch_ {

using Default = struct {};

/**
 * AST node for a switch case type.
 *
 * Note that internally, we store the expressions in a preprocessed matter:
 * `E` turns into `<id> == E`, where ID is selected to match the code
 * generator's output. Doing this allows coercion for the comparison to
 * proceed normally. The preprocessing happens at the time the `Case` gets
 * added to a `Switch` statement, and the new versions are stored separately
 * from the original expressions.
 */
class Case final : public Node {
public:
    ~Case() final;

    auto expressions() const { return children<hilti::Expression>(1, _end_exprs); }
    auto body() const { return child<Statement>(0); }
    bool isDefault() const { return expressions().empty(); }

    auto preprocessedExpressions() const { return children<hilti::Expression>(_end_exprs, -1); }

    static auto create(ASTContext* ctx, const Expressions& exprs, const StatementPtr& body, Meta meta = {}) {
        return NodeDerivedPtr<Case>(new Case(node::flatten(body, exprs), std::move(meta)));
    }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const StatementPtr& body, Meta meta = {}) {
        return create(ctx, Expressions{expr}, body, std::move(meta));
    }

    static auto create(ASTContext* ctx, switch_::Default /*unused*/, const StatementPtr& body, Meta meta = {}) {
        return create(ctx, Expressions{}, body, std::move(meta));
    }

protected:
    friend class statement::Switch;

    Case(Nodes children, Meta meta = {}) : Node(std::move(children), std::move(meta)) {
        _end_exprs = static_cast<int>(Node::children().size());
    }

    void _preprocessExpressions(ASTContext* ctx, const std::string& id) {
        removeChildren(_end_exprs, -1);

        for ( const auto& e : expressions() ) {
            auto n = expression::UnresolvedOperator::create(ctx, operator_::Kind::Equal,
                                                            {expression::Name::create(ctx, ID(id), e->meta()), e},
                                                            e->meta());

            addChild(std::move(n));
        }
    }

    std::string _render() const final;

    bool isEqual(const Node& other) const final { return other.isA<Case>() && Node::isEqual(other); }

    HILTI_NODE(Case);

private:
    int _end_exprs;
};

using CasePtr = std::shared_ptr<Case>;
using Cases = std::vector<CasePtr>;

} // namespace switch_

/** AST node for a `switch` statement. */
class Switch : public Statement {
public:
    auto condition() const { return child<declaration::LocalVariable>(0); }
    auto cases() const { return children<switch_::Case>(1, -1); }

    switch_::CasePtr default_() const {
        for ( const auto& c : cases() ) {
            if ( c->isDefault() )
                return c;
        }

        return nullptr;
    }

    void preprocessCases(ASTContext* ctx) {
        if ( _preprocessed )
            return;

        for ( const auto& c : cases() )
            c->_preprocessExpressions(ctx, condition()->id());

        _preprocessed = true;
    }

    static auto create(ASTContext* ctx, DeclarationPtr cond, const switch_::Cases& cases, Meta meta = {}) {
        return NodeDerivedPtr<Switch>(new Switch(node::flatten(std::move(cond), cases), std::move(meta)));
    }

    static auto create(ASTContext* ctx, const ExpressionPtr& cond, const switch_::Cases& cases, Meta meta = {}) {
        return create(ctx, declaration::LocalVariable::create(ctx, ID("__x")), cases, std::move(meta));
    }

protected:
    Switch(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {
        if ( ! child(0)->isA<declaration::LocalVariable>() )
            logger().internalError("initialization for 'switch' must be a local declaration");
    }

    bool isEqual(const Node& other) const final { return other.isA<Switch>() && Statement::isEqual(other); }

    HILTI_NODE(Switch)

private:
    bool _preprocessed = false;
};

} // namespace hilti::statement
