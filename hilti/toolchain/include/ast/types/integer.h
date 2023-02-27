// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>
#include <vector>

#include <hilti/ast/type.h>

namespace hilti::type {

namespace detail {

/** Common base class for an AST node representing an integer type. */
class IntegerBase : public UnqualifiedType {
public:
    auto width() const { return _width; }
    Nodes typeParameters() const final { return {_type_arg}; }

    node::Properties properties() const final {
        auto p = node::Properties{{"width", _width}};
        return UnqualifiedType::properties() + p;
    }

protected:
    IntegerBase(Nodes children, unsigned int width, NodePtr type_arg, const Meta& m = Meta())
        : UnqualifiedType(std::move(children), m), _width(width), _type_arg(std::move(type_arg)) {}
    IntegerBase(Wildcard _, Meta m = Meta()) : UnqualifiedType(Wildcard(), std::move(m)) {}

    void setTypeArg(NodePtr t) { _type_arg = std::move(t); }

    bool _isAllocable() const final { return true; }
    bool _isSortable() const final { return true; }
    bool _isParameterized() const final { return true; }

private:
    unsigned int _width = 0;
    NodePtr _type_arg; // outside of AST to avoid cycles
};

} // namespace detail

/** AST node for a signed integer type. */
class SignedInteger : public detail::IntegerBase {
public:
    static NodeDerivedPtr<SignedInteger> create(ASTContext* ctx, int width, const Meta& m = Meta());

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<SignedInteger>(new SignedInteger(Wildcard(), std::move(m)));
    }

protected:
    using detail::IntegerBase::IntegerBase;

    bool isEqual(const Node& other) const override {
        return other.isA<SignedInteger>() && UnqualifiedType::isEqual(other);
    }

    HILTI_NODE(SignedInteger)
};

inline bool operator==(const SignedInteger& x, const SignedInteger& y) { return x.width() == y.width(); }
inline bool operator!=(const SignedInteger& x, const SignedInteger& y) { return ! (x == y); }

/** AST node for an unsigned integer type. */
class UnsignedInteger : public detail::IntegerBase {
public:
    static NodeDerivedPtr<UnsignedInteger> create(ASTContext* ctx, int width, const Meta& m = Meta());

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<UnsignedInteger>(new UnsignedInteger(Wildcard(), std::move(m)));
    }

protected:
    using detail::IntegerBase::IntegerBase;

    bool isEqual(const Node& other) const override {
        return other.isA<UnsignedInteger>() && UnqualifiedType::isEqual(other);
    }

    HILTI_NODE(UnsignedInteger);
};

inline bool operator==(const UnsignedInteger& x, const UnsignedInteger& y) { return x.width() == y.width(); }
inline bool operator!=(const UnsignedInteger& x, const UnsignedInteger& y) { return ! (x == y); }

} // namespace hilti::type
