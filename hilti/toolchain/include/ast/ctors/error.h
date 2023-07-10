// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/error.h>

namespace hilti::ctor {

/** AST node for a error ctor. */
class Error : public Ctor {
public:
    const auto& value() const { return _value; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", _value}};
        return Ctor::properties() + p;
    }

    static auto create(ASTContext* ctx, std::string v, const Meta& meta = {}) {
        return CtorPtr(
            new Error({QualifiedType::create(ctx, type::Error::create(ctx, meta), true)}, std::move(v), meta));
    }

protected:
    Error(Nodes children, std::string v, Meta meta)
        : Ctor(std::move(children), std::move(meta)), _value(std::move(v)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Error>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value;
    }

    HILTI_NODE(Error)

private:
    std::string _value;
};

} // namespace hilti::ctor
