// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/rt/types/port.h>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/port.h>

namespace hilti::ctor {

/** AST node for a `port` ctor. */
class Port : public Ctor {
public:
    const auto& value() const { return _value; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", to_string(_value)}};
        return Ctor::properties() + p;
    }

    static auto create(ASTContext* ctx, hilti::rt::Port v, const Meta& meta = {}) {
        return NodeDerivedPtr<Port>(
            new Port({QualifiedType::create(ctx, type::Port::create(ctx, meta), true)}, v, meta));
    }

protected:
    Port(Nodes children, hilti::rt::Port v, Meta meta) : Ctor(std::move(children), std::move(meta)), _value(v) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Port>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value;
    }

    HILTI_NODE(Port)

private:
    hilti::rt::Port _value;
};

} // namespace hilti::ctor
