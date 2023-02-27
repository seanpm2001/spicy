// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/stream.h>

namespace hilti::ctor {

/** AST node for a `stream` ctor. */
class Stream : public Ctor {
public:
    const auto& value() const { return _value; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", _value}};
        return Ctor::properties() + p;
    }

    static auto create(ASTContext* ctx, std::string value, const Meta& meta = {}) {
        return CtorPtr(
            new Stream({QualifiedType::create(ctx, type::Stream::create(ctx, meta), true)}, std::move(value), meta));
    }

protected:
    Stream(Nodes children, std::string value, Meta meta)
        : Ctor(std::move(children), std::move(meta)), _value(std::move(value)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Stream>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value;
    }

    HILTI_NODE(Stream)

private:
    std::string _value;
};

} // namespace hilti::ctor
