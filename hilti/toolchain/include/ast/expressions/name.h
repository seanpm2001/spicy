// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/id.h>
#include <hilti/ast/type.h>

namespace hilti::expression {

/** AST node for an expression referencing an ID. */
class Name : public Expression {
public:
    const auto& id() const { return _id; }
    auto declaration() const { return _declaration; }

    QualifiedTypePtr type() const final;

    void setDeclaration(DeclarationPtr d) { _declaration = std::move(d); }

    node::Properties properties() const final {
        auto p = node::Properties{{"id", _id}};
        return Expression::properties() + p;
    }

    static auto create(ASTContext* ctx, const hilti::ID& id, const Meta& meta = {}) {
        return NodeDerivedPtr<Name>(new Name({QualifiedType::createAuto(ctx, meta)}, id, meta));
    }

protected:
    Name(Nodes children, hilti::ID id, Meta meta)
        : Expression(std::move(children), std::move(meta)), _id(std::move(id)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Name>();
        if ( ! n )
            return false;

        return Expression::isEqual(other) && _id == n->_id;
    }

    HILTI_NODE(Name)

private:
    hilti::ID _id;
    DeclarationPtr _declaration = nullptr;
};

} // namespace hilti::expression
