// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/integer.h>

namespace hilti::ctor {

namespace detail {
/** Base class for AST nodes for both signed and unsigned integer constructors. */
template<typename Value>
class IntegerBase : public Ctor {
public:
    const auto& value() const { return _value; }
    auto width() const { return _width; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", _value}, {"width", _width}};
        return Ctor::properties() + p;
    }

protected:
    IntegerBase(Nodes children, Value v, int width, Meta meta)
        : Ctor(std::move(children), std::move(meta)), _value(v), _width(width) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<IntegerBase>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value && _width == n->_width;
    }

private:
    Value _value;
    int _width;
};

} // namespace detail

/** AST node for a signed integer constructor. */
class SignedInteger : public detail::IntegerBase<int64_t> {
public:
    static auto create(ASTContext* ctx, int64_t value, int width, const Meta& meta = {}) {
        return CtorPtr(
            new SignedInteger({QualifiedType::create(ctx, type::SignedInteger::create(ctx, width, meta), true)}, value,
                              width, meta));
    }

protected:
    SignedInteger(Nodes children, int64_t value, int width, Meta meta)
        : IntegerBase(std::move(children), value, width, std::move(meta)) {}

    bool isEqual(const Node& other) const override {
        return other.isA<SignedInteger>() && detail::IntegerBase<int64_t>::isEqual(other);
    }

    HILTI_NODE(SignedInteger)
};

/** AST node for a unsigned integer constructor. */
class UnsignedInteger : public detail::IntegerBase<uint64_t> {
public:
    static auto create(ASTContext* ctx, uint64_t value, int width, const Meta& meta = {}) {
        return CtorPtr(
            new UnsignedInteger({QualifiedType::create(ctx, type::UnsignedInteger::create(ctx, width, meta), true)},
                                value, width, meta));
    }

    static auto create(ASTContext* ctx, uint64_t value, int width, const UnqualifiedTypePtr& t, const Meta& meta = {}) {
        return CtorPtr(new UnsignedInteger({QualifiedType::create(ctx, t, true)}, value, width, meta));
    }

protected:
    UnsignedInteger(Nodes children, uint64_t value, int width, Meta meta)
        : IntegerBase(std::move(children), value, width, std::move(meta)) {}

    bool isEqual(const Node& other) const override {
        return other.isA<UnsignedInteger>() && detail::IntegerBase<uint64_t>::isEqual(other);
    }

    HILTI_NODE(UnsignedInteger)
};

} // namespace hilti::ctor
