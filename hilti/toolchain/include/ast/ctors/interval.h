// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/rt/types/interval.h>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/interval.h>

namespace hilti::ctor {

/** AST node for a `interval` ctor. */
class Interval : public Ctor {
public:
    const auto& value() const { return _value; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", to_string(_value)}};
        return Ctor::properties() + p;
    }

    static auto create(ASTContext* ctx, hilti::rt::Interval v, const Meta& meta = {}) {
        return NodeDerivedPtr<Interval>(
            new Interval({QualifiedType::create(ctx, type::Interval::create(ctx, meta), true)}, v, meta));
    }

protected:
    Interval(Nodes children, hilti::rt::Interval v, Meta meta)
        : Ctor(std::move(children), std::move(meta)), _value(std::move(v)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Interval>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value;
    }

    HILTI_NODE(Interval)

private:
    hilti::rt::Interval _value;
};

} // namespace hilti::ctor
