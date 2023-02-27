// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for a type representing a member of another type. */
class Member : public UnqualifiedType {
public:
    const auto& id() const { return _id; }
    auto typeValue() const { return child<Member>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"id", _id}};
        return UnqualifiedType::properties() + p;
    }

    static auto create(ASTContext* ctx, const ID& id, Meta meta = {}) {
        return NodeDerivedPtr<Member>(new Member(id, std::move(meta)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Member>(new Member(Wildcard(), std::move(m)));
    }

protected:
    Member(ID id, Meta meta) : UnqualifiedType(std::move(meta)), _id(std::move(id)) {}
    Member(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)), _id("<wildcard>") {}

    bool _isParameterized() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final { return type::detail::isResolved(typeValue(), rstate); }

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Member>();
        if ( ! n )
            return false;

        return UnqualifiedType::isEqual(other) && _id == n->_id;
    }

    HILTI_NODE(Member)

private:
    ID _id;
};

} // namespace hilti::type
