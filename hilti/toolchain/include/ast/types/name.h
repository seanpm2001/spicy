// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <string>
#include <utility>

#include <hilti/ast/type.h>

#include "ast/declarations/type.h"

namespace hilti::type {

/** AST node for type referenced by Name. */
class Name : public UnqualifiedType {
public:
    auto id() const { return _id; }
    const auto& declaration() const { return _declaration; }

    void setDeclaration(NodeDerivedPtr<declaration::Type> d) { _declaration = std::move(d); }

    node::Properties properties() const final {
        auto p = node::Properties{{"id", _id},
                                  {"resolved", (_declaration ? _declaration->canonicalID().str() : std::string("-"))}};
        return UnqualifiedType::properties() + p;
    }

    static auto create(ASTContext* ctx, ID id, Meta meta = {}) {
        return NodeDerivedPtr<Name>(new Name(std::move(id), std::move(meta)));
    }

protected:
    Name(ID id, Meta meta) : UnqualifiedType(std::move(meta)), _id(std::move(id)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Name>();
        if ( ! n )
            return false;

        return UnqualifiedType::isEqual(other) && _id == n->_id;
    }

    HILTI_NODE(Name)

private:
    ID _id;
    NodeDerivedPtr<declaration::Type> _declaration;
};

} // namespace hilti::type
