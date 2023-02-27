// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/id.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/member.h>

namespace hilti::expression {

/** AST node for a member expression. */
class Member : public Expression {
public:
    const auto& id() const { return _id; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }
    bool isLhs() const final { return true; }
    bool isTemporary() const final { return false; }

    node::Properties properties() const final {
        auto p = node::Properties{{"id", _id}};
        return Expression::properties() + p;
    }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& member_type, const hilti::ID& id,
                       const Meta& meta = {}) {
        return NodeDerivedPtr<Member>(new Member({member_type}, id, meta));
    }

    static auto create(ASTContext* ctx, const hilti::ID& id, const Meta& meta = {}) {
        return create(ctx, QualifiedType::create(ctx, type::Member::create(ctx, id, meta), true, meta), id, meta);
    }

protected:
    Member(Nodes children, hilti::ID id, Meta meta)
        : Expression(std::move(children), std::move(meta)), _id(std::move(id)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Member>();
        if ( ! n )
            return false;

        return Expression::isEqual(other) && _id == n->_id;
    }

    HILTI_NODE(Member)

private:
    hilti::ID _id;
};

} // namespace hilti::expression
