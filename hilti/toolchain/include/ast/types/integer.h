// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>
#include <vector>

#include <hilti/ast/type.h>

namespace hilti::type {

namespace detail {

// CHECK: IntegerBase = TypeBase
/** Base class for an AST node representing an integer type. */
class IntegerBase : public TypeBase {
public:
    IntegerBase(Wildcard /*unused*/, Meta m = Meta()) : TypeBase(std::move(m)), _wildcard(true) {}
    IntegerBase(int width, Meta m = Meta()) : TypeBase(std::move(m)), _width(width) {}
    IntegerBase(Meta m = Meta()) : TypeBase(std::move(m)) {}

    auto width() const { return _width; }

    bool isWildcard() const override { return _wildcard; }
    bool _isResolved(ResolvedState* rstate) const override { return true; }
    node::Properties properties() const override { return node::Properties{{"width", _width}}; }

    bool _isAllocable() const override { return true; }
    bool _isSortable() const override { return true; }
    bool _isParameterized() const override { return true; }

    const std::type_info& typeid_() const override { return typeid(decltype(*this)); }

    HILTI_TYPE_VISITOR_IMPLEMENT

private:
    bool _wildcard = false;
    int _width = 0;
};

} // namespace detail

/** AST node for a signed integer type. */
class SignedInteger : public detail::IntegerBase {
public:
    using detail::IntegerBase::IntegerBase;

    bool operator==(const SignedInteger& other) const { return width() == other.width(); }

    std::vector<Node> typeParameters() const override;

    bool isEqual(const Type& other) const override { return node::isEqual(this, other); }

    const std::type_info& typeid_() const override { return typeid(decltype(*this)); }

    HILTI_TYPE_VISITOR_IMPLEMENT
};

/** AST node for an unsigned integer type. */
class UnsignedInteger : public detail::IntegerBase {
public:
    using detail::IntegerBase::IntegerBase;

    bool operator==(const UnsignedInteger& other) const { return width() == other.width(); }

    std::vector<Node> typeParameters() const override;

    bool isEqual(const Type& other) const override { return node::isEqual(this, other); }

    const std::type_info& typeid_() const override { return typeid(decltype(*this)); }

    HILTI_TYPE_VISITOR_IMPLEMENT
};

} // namespace hilti::type
