// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/rt/types/network.h>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/network.h>

namespace hilti::ctor {

/** AST node for a `network` ctor. */
class Network : public Ctor {
public:
    const auto& value() const { return _value; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", to_string(_value)}};
        return Ctor::properties() + p;
    }

    static auto create(ASTContext* ctx, hilti::rt::Network v, const Meta& meta = {}) {
        return NodeDerivedPtr<Network>(
            new Network({QualifiedType::create(ctx, type::Network::create(ctx, meta), true)}, v, meta));
    }

protected:
    Network(Nodes children, hilti::rt::Network v, Meta meta) : Ctor(std::move(children), std::move(meta)), _value(v) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Network>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value;
    }

    HILTI_NODE(Network)

private:
    hilti::rt::Network _value;
};

} // namespace hilti::ctor
