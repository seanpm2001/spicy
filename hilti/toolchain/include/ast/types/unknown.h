// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>

#include <hilti/ast/type.h>

namespace hilti {
namespace type {

/** AST node for an unknown place-holder type. */
class Unknown : public TypeBase, public util::type_erasure::trait::Singleton {
public:
    bool operator==(const Unknown& /* other */) const { return true; }

    bool isEqual(const Type& other) const override { return node::isEqual(this, other); }
    bool _isResolved(ResolvedState* rstate) const override { return true; } // sic!

    node::Properties properties() const override { return node::Properties{}; }

    bool _isAllocable() const override { return true; }

    const std::type_info& typeid_() const override { return typeid(decltype(*this)); }

    /**
     * Wrapper around constructor so that we can make it private. Don't use
     * this, use the singleton `type::unknown` instead.
     */
    static Unknown create(Meta m = Meta()) { return Unknown(std::move(m)); }

    HILTI_TYPE_VISITOR_IMPLEMENT

private:
    Unknown(Meta m = Meta()) : TypeBase(std::move(m)) {}
};

/** Singleton. */
static const Type unknown = Unknown::create(Location("<singleton>"));

} // namespace type

inline const Node& to_node(const type::Unknown& /* t */) {
    static Node unknown = Type(type::unknown);
    return unknown;
}

} // namespace hilti
