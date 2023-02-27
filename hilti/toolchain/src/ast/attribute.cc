// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/attribute.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/visitor.h>

using namespace hilti;

Attribute::~Attribute() = default;

Result<ExpressionPtr> Attribute::valueAsExpression() const {
    if ( ! hasValue() )
        return result::Error(hilti::util::fmt("attribute '%s' requires an expression", _tag));

    if ( ! value()->isA<Expression>() )
        return result::Error(hilti::util::fmt("value for attribute '%s' must be an expression", _tag));

    return {value()->as<Expression>()};
}

Result<std::string> Attribute::valueAsString() const {
    if ( ! hasValue() )
        return result::Error(hilti::util::fmt("attribute '%s' requires a string", _tag));

    /* TODO
     * if ( auto e = value()->tryAs<expression::Ctor>() )
     *     if ( auto s = e->ctor()->tryAs<ctor::String>() )
     *         return s->value();
     */

    return result::Error(hilti::util::fmt("value for attribute '%s' must be a string", _tag));
}

Result<int64_t> Attribute::valueAsInteger() const {
    if ( ! hasValue() )
        return result::Error(hilti::util::fmt("attribute '%s' requires an integer", _tag));

    /* TODO
     * if ( auto e = value().tryAs<expression::Ctor>() ) {
     *     if ( auto s = e->ctor().tryAs<ctor::SignedInteger>() )
     *         return s->value();
     *
     *     if ( auto s = e->ctor().tryAs<ctor::UnsignedInteger>() )
     *         return static_cast<int64_t>(s->value());
     * }
     */

    return result::Error(hilti::util::fmt("value for attribute '%s' must be an integer", _tag));
}

// TODO: This needs to mvoe somewhere else. Into builder?
/*
 * Result<bool> Attribute::coerceValueTo(const TypePtr& dst) {
 *     auto x = valueAsExpression();
 *     if ( ! x )
 *         return x.error();
 *
 *     if ( ! type::isResolved(dst) )
 *         return false;
 *
 *     auto ne = coerceExpression(*x, dst);
 *     if ( ! ne.coerced )
 *         return result::Error(
 *             util::fmt("cannot coerce attribute's expression from type '%s' to '%s' (%s)", x->get().type(), dst, tag()));
 *
 *     if ( ne.nexpr ) {
 *         children()[0] = *ne.nexpr;
 *         return true;
 *     }
 *
 *     return false;
 * }
 */

std::string Attribute::_render() const { return ""; }

AttributeSet::~AttributeSet() = default;

std::string AttributeSet::_render() const { return ""; }

AttributePtr AttributeSet::find(std::string_view tag) const {
    for ( const auto& a : attributes() )
        if ( a->tag() == tag )
            return a;

    return {};
}

hilti::node::Set<Attribute> AttributeSet::findAll(std::string_view tag) const {
    hilti::node::Set<Attribute> result;

    for ( const auto& a : attributes() )
        if ( a->tag() == tag )
            result.insert(a);

    return result;
}

// TODO: This needs to mvoe somewhere else. Into builder?
/*
 * Result<bool> AttributeSet::coerceValueTo(const std::string& tag, const TypePtr& dst) {
 *     if ( ! type::isResolved(dst) )
 *         return false;
 *
 *     for ( auto& n : children() ) {
 *         auto a = n->as<Attribute>();
 *         if ( a->tag() != tag )
 *             continue;
 *
 *         if ( auto e = a->valueAsExpression() ) {
 *             auto ne = coerceExpression(*e, dst);
 *             if ( ! ne.coerced )
 *                 return result::Error("cannot coerce attribute value");
 *
 *             if ( ne.nexpr ) {
 *                 n = Attribute(tag, std::move(*ne.nexpr));
 *                 return true;
 *             }
 *
 *             return false;
 *         }
 *     }
 *
 *     return false;
 * }
 */
