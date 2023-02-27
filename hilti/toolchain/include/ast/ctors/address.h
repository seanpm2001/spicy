// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/rt/types/address.h>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/address.h>

#include "ast/type.h"

namespace hilti::ctor {

/** AST node for a `address` ctor. */
class Address : public Ctor {
public:
    const auto& value() const { return _value; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", to_string(_value)}};
        return Ctor::properties() + p;
    }

    static auto create(ASTContext* ctx, hilti::rt::Address v, const Meta& meta = {}) {
        return NodeDerivedPtr<Address>(
            new Address({QualifiedType::create(ctx, type::Address::create(ctx, meta), true)}, v, meta));
    }

protected:
    Address(Nodes children, hilti::rt::Address v, Meta meta) : Ctor(std::move(children), std::move(meta)), _value(v) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Address>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value;
    }

    HILTI_NODE(Address)

private:
    hilti::rt::Address _value;
};

} // namespace hilti::ctor
