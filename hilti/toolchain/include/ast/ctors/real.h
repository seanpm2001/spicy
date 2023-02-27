// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/real.h>

namespace hilti::ctor {

/** AST node for a `real` ctor. */
class Real : public Ctor {
public:
    const auto& value() const { return _value; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", _value}};
        return Ctor::properties() + p;
    }

    static auto create(ASTContext* ctx, double v, const Meta& meta = {}) {
        return NodeDerivedPtr<Real>(
            new Real({QualifiedType::create(ctx, type::Real::create(ctx, meta), true)}, v, meta));
    }

protected:
    Real(Nodes children, double v, Meta meta) : Ctor(std::move(children), std::move(meta)), _value(v) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Real>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value;
    }

    HILTI_NODE(Real)

private:
    double _value;
};

} // namespace hilti::ctor
