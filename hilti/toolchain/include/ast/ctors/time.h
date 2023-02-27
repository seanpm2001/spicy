// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/rt/types/time.h>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/time.h>

namespace hilti::ctor {

/** AST node for a `time` ctor. */
class Time : public Ctor {
public:
    const auto& value() const { return _value; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", to_string(_value)}};
        return Ctor::properties() + p;
    }

    static auto create(ASTContext* ctx, hilti::rt::Time v, const Meta& meta = {}) {
        return NodeDerivedPtr<Time>(
            new Time({QualifiedType::create(ctx, type::Time::create(ctx, meta), true)}, v, meta));
    }

protected:
    Time(Nodes children, hilti::rt::Time v, Meta meta)
        : Ctor(std::move(children), std::move(meta)), _value(std::move(v)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Time>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value;
    }

    HILTI_NODE(Time)

private:
    hilti::rt::Time _value;
};

} // namespace hilti::ctor
