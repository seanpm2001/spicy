// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <utility>

#include <hilti/ast/ctors/coerced.h>
#include <hilti/ast/ctors/null.h>
#include <hilti/ast/ctors/tuple.h>
#include <hilti/ast/expressions/coerced.h>
#include <hilti/ast/expressions/ctor.h>
#include <hilti/ast/expressions/member.h>
#include <hilti/ast/expressions/resolved-operator.h>
#include <hilti/ast/expressions/unresolved-operator.h>
#include <hilti/ast/types/any.h>
#include <hilti/ast/types/auto.h>
#include <hilti/ast/types/operand-list.h>
#include <hilti/ast/types/optional.h>
#include <hilti/ast/types/reference.h>
#include <hilti/ast/types/result.h>
#include <hilti/base/logger.h>
#include <hilti/compiler/coercion.h>
#include <hilti/compiler/plugin.h>
#include <hilti/global.h>

#include "ast/builder/builder.h"
#include "base/timing.h"

using namespace hilti;
using namespace util;

namespace hilti::logging::debug {
inline const DebugStream Operator("operator");
} // namespace hilti::logging::debug


namespace {

struct VisitorCtor : visitor::PreOrder {
    explicit VisitorCtor(QualifiedTypePtr  dst, bitmask<CoercionStyle> style) : dst(std::move(dst)), style(style) {}

    QualifiedTypePtr dst;
    bitmask<CoercionStyle> style;

    CtorPtr result = nullptr;

#if 0
    void operator()(const ctor::Enum& c) {
        if ( dst.isA<type::Bool>() && (style & CoercionStyle::ContextualConversion) )
            return ctor::Bool(c.value().id() != ID("Undef"), c.meta());

        return {};
    }

    void operator()(const ctor::Map& c) {
        if ( auto t = dst.tryAs<type::Map>() ) {
            std::vector<ctor::map::Element> nelemns;
            for ( const auto& e : c.value() ) {
                auto k = hilti::coerceExpression(e.key(), t->keyType(), style);
                auto v = hilti::coerceExpression(e.value(), *t->elementType(), style);

                if ( k && v )
                    nelemns.emplace_back(*k.coerced, *v.coerced);
                else
                    return {};
            }

            return ctor::Map(t->keyType(), *t->elementType(), nelemns, c.meta());
        }

        return {};
    }

    void operator()(const ctor::Null& c) {
        if ( auto t = dst.tryAs<type::Optional>() )
            return ctor::Optional(*t->dereferencedType());

        if ( auto t = dst.tryAs<type::StrongReference>() )
            return ctor::StrongReference(*t->dereferencedType());

        if ( auto t = dst.tryAs<type::WeakReference>() )
            return ctor::WeakReference(*t->dereferencedType());

        return {};
    }

    void operator()(const ctor::List& c) {
        if ( auto t = dst.tryAs<type::List>() ) {
            std::vector<Expression> nexprs;
            for ( const auto& e : c.value() ) {
                if ( auto x = hilti::coerceExpression(e, *t->elementType(), CoercionStyle::TryAllForAssignment) )
                    nexprs.push_back(*x.coerced);
                else
                    return {};
            }
            return ctor::List(*t->elementType(), std::move(nexprs), c.meta());
        }

        if ( auto t = dst.tryAs<type::Vector>() ) {
            auto dt = t->isWildcard() ? c.elementType() : *t->elementType();

            std::vector<Expression> nexprs;
            for ( const auto& e : c.value() ) {
                if ( auto x = hilti::coerceExpression(e, dt, CoercionStyle::TryAllForAssignment) )
                    nexprs.push_back(*x.coerced);
                else
                    return {};
            }
            return ctor::Vector(dt, std::move(nexprs), c.meta());
        }

        if ( auto t = dst.tryAs<type::Set>() ) {
            auto dt = t->isWildcard() ? c.elementType() : *t->elementType();

            std::vector<Expression> nexprs;
            for ( const auto& e : c.value() ) {
                if ( auto x = hilti::coerceExpression(e, dt, CoercionStyle::TryAllForAssignment) )
                    nexprs.push_back(*x.coerced);
                else
                    return {};
            }
            return ctor::Set(dt, std::move(nexprs), c.meta());
        }

        return {};
    }

    void operator()(const ctor::Real& c) {
        // Note: double->Integral constant conversions check 'non-narrowing' via
        // double->Int->double roundtrip - the generated code looks good.

        if ( auto t = dst.tryAs<type::SignedInteger>() ) {
            double d = c.value();

            if ( static_cast<double>(static_cast<int64_t>(d)) == d ) {
                switch ( t->isWildcard() ? 64 : t->width() ) {
                    case 8:
                        if ( static_cast<double>(int8_t(d)) == d )
                            return ctor::SignedInteger(int64_t(d), 8, c.meta());
                        break;

                    case 16:
                        if ( static_cast<double>(static_cast<int16_t>(d)) == d )
                            return ctor::SignedInteger(static_cast<int64_t>(d), 16, c.meta());
                        break;

                    case 32:
                        if ( static_cast<double>(static_cast<int32_t>(d)) == d )
                            return ctor::SignedInteger(static_cast<int64_t>(d), 32, c.meta());
                        break;

                    case 64: return ctor::SignedInteger(static_cast<int64_t>(d), 64, c.meta()); break;
                }
            }
        }

        if ( auto t = dst.tryAs<type::UnsignedInteger>() ) {
            double d = c.value();

            if ( static_cast<double>(static_cast<uint64_t>(d)) == d ) {
                switch ( t->isWildcard() ? 64 : t->width() ) {
                    case 8:
                        if ( static_cast<double>(static_cast<uint8_t>(d)) == d )
                            return ctor::UnsignedInteger(static_cast<uint64_t>(d), 8, c.meta());
                        break;

                    case 16:
                        if ( static_cast<double>(static_cast<uint16_t>(d)) == d )
                            return ctor::UnsignedInteger(uint64_t(d), 16, c.meta());
                        break;

                    case 32:
                        if ( static_cast<double>(static_cast<uint32_t>(d)) == d )
                            return ctor::UnsignedInteger(static_cast<uint64_t>(d), 32, c.meta());
                        break;

                    case 64: return ctor::UnsignedInteger(static_cast<uint64_t>(d), 64, c.meta()); break;
                }
            }
        }

        return {};
    }

    void operator()(const ctor::Set& c) {
        if ( auto t = dst.tryAs<type::Set>() ) {
            std::vector<Expression> nexprs;
            for ( const auto& e : c.value() ) {
                if ( auto x = hilti::coerceExpression(e, *t->elementType(), style) )
                    nexprs.push_back(*x.coerced);
                else
                    return {};
            }
            return ctor::Set(*t->elementType(), std::move(nexprs), c.meta());
        }

        return {};
    }

    void operator()(const ctor::SignedInteger& c) {
        if ( auto t = dst.tryAs<type::SignedInteger>() ) {
            if ( t->width() == 64 )
                return c;

            int64_t i = c.value();

            if ( t->isWildcard() )
                return ctor::SignedInteger(i, c.width(), c.meta());

            if ( auto [imin, imax] = util::signed_integer_range(t->width()); i >= imin && i <= imax )
                return ctor::SignedInteger(i, t->width(), c.meta());
        }

        if ( auto t = dst.tryAs<type::UnsignedInteger>(); t && c.value() >= 0 ) {
            auto u = static_cast<uint64_t>(c.value());

            if ( t->isWildcard() )
                return ctor::UnsignedInteger(u, c.width(), c.meta());

            if ( auto [zero, umax] = util::unsigned_integer_range(t->width()); u <= umax )
                return ctor::UnsignedInteger(u, t->width(), c.meta());
        }

        if ( auto t = dst.tryAs<type::Real>() ) {
            if ( static_cast<int64_t>(static_cast<double>(c.value())) == c.value() )
                return ctor::Real(static_cast<double>(c.value()));
        }

        if ( dst.isA<type::Bool>() && (style & CoercionStyle::ContextualConversion) )
            return ctor::Bool(c.value() != 0, c.meta());

        return {};
    }

    void operator()(const ctor::Vector& c) {
        if ( auto t = dst.tryAs<type::Vector>() ) {
            std::vector<Expression> nexprs;
            for ( const auto& e : c.value() ) {
                if ( auto x = hilti::coerceExpression(e, *t->elementType(), style) )
                    nexprs.push_back(*x.coerced);
                else
                    return {};
            }
            return ctor::Vector(*t->elementType(), std::move(nexprs), c.meta());
        }

        return {};
    }

    void operator()(const ctor::UnsignedInteger& c) {
        if ( auto t = dst.tryAs<type::UnsignedInteger>() ) {
            if ( t->width() == 64 )
                return c;

            uint64_t u = c.value();

            if ( t->isWildcard() )
                return ctor::UnsignedInteger(u, c.width(), c.meta());

            if ( auto [umin, umax] = util::unsigned_integer_range(t->width()); u >= umin && u <= umax )
                return ctor::UnsignedInteger(u, t->width(), c.meta());
        }

        if ( auto t = dst.tryAs<type::SignedInteger>(); t && static_cast<int64_t>(c.value()) >= 0 ) {
            auto i = static_cast<int64_t>(c.value());

            if ( t->isWildcard() )
                return ctor::SignedInteger(i, c.width(), c.meta());

            if ( auto [imin, imax] = util::signed_integer_range(t->width()); i >= imin && i <= imax )
                return ctor::SignedInteger(i, t->width(), c.meta());
        }

        if ( dst.isA<type::Bool>() && (style & CoercionStyle::ContextualConversion) )
            return ctor::Bool(c.value() != 0, c.meta());

        if ( auto t = dst.tryAs<type::Real>() ) {
            if ( static_cast<uint64_t>(static_cast<double>(c.value())) == c.value() )
                return ctor::Real(static_cast<double>(c.value()));
        }

        return {};
    }

    void operator()(const ctor::Tuple& c) {
        if ( auto t = dst.tryAs<type::Tuple>() ) {
            auto vc = c.value();
            auto ve = t.value().elements();

            if ( vc.size() != ve.size() )
                return {};

            std::vector<Expression> coerced;
            coerced.reserve(vc.size());

            for ( auto i = std::make_pair(vc.begin(), ve.begin()); i.first != vc.end(); ++i.first, ++i.second ) {
                if ( auto x =
                         hilti::coerceExpression(*i.first, (*i.second).type(), CoercionStyle::TryAllForAssignment) ) {
                    coerced.push_back(*x.coerced);
                }
                else
                    return {};
            }

            return ctor::Tuple(coerced, c.meta());
        }

        return {};
    }

    void operator()(const ctor::Struct& c) {
        auto dst_ = dst;

        if ( (dst.isA<type::ValueReference>() || dst.isA<type::StrongReference>()) && ! type::isReferenceType(dst) )
            // Allow coercion from value to reference type with new instance.
            dst_ = *dst.dereferencedType();

        if ( auto dtype = dst_.tryAs<type::Struct>() ) {
            if ( ! dst_.typeID() )
                // Wait for this to be resolved.
                return {};

            auto stype = c.type().as<type::Struct>();

            std::set<ID> src_fields;
            for ( const auto& f : stype.fields() )
                src_fields.insert(f.id());

            std::set<ID> dst_fields;
            for ( const auto& f : dtype->fields() )
                dst_fields.insert(f.id());

            // Check for fields in ctor that type does not have.
            if ( ! util::set_difference(src_fields, dst_fields).empty() )
                return {};

            // Check for fields that the type has, but are left out in the
            // ctor. These must all be either optional, internal, or have a
            // default.
            auto x = util::set_difference(dst_fields, src_fields);

            std::set<ID> can_be_missing;

            for ( const auto& k : x ) {
                auto f = dtype->field(k);
                if ( f->isOptional() || f->isInternal() || f->default_() || f->type().isA<type::Function>() )
                    can_be_missing.insert(k);
            }

            x = util::set_difference(x, can_be_missing);

            if ( ! x.empty() )
                // Uninitialized fields.
                return {};

            // Coerce each field.
            std::vector<ctor::struct_::Field> nf;

            for ( const auto& sf : stype.fields() ) {
                const auto& df = dtype->field(sf.id());
                const auto& se = c.field(sf.id());
                assert(df && se);
                if ( const auto& ne = hilti::coerceExpression(se->expression(), df->type(), style) )
                    nf.emplace_back(sf.id(), *ne.coerced);
                else
                    // Cannot coerce.
                    return {};
            }

            return ctor::Struct(std::move(nf), dst_, c.meta());
        }

        return {};
    }
#endif
};

struct VisitorType : visitor::PreOrder {
    explicit VisitorType(QualifiedTypePtr dst, bitmask<CoercionStyle> style) : dst(std::move(dst)), style(style) {}

    QualifiedTypePtr dst;
    bitmask<CoercionStyle> style;

    QualifiedTypePtr result = nullptr;

#if 0
    void operator()(const type::Enum& c, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Bool>(); t && (style & CoercionStyle::ContextualConversion) )
            _result = dst;
    }

    void operator()(const type::Interval& c, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Bool>(); t && (style & CoercionStyle::ContextualConversion) )
            _result = dst;
    }

    void operator()(const type::Null& c, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Optional>() )
            _result = dst;
        else if ( auto t = dst.tryAs<type::StrongReference>() )
            _result = dst;
        else if ( auto t = dst.tryAs<type::WeakReference>() )
            _result = dst;
    }

    void operator()(const type::Bytes& c, type::Visitor::position_t&) override {
        if ( dst.tryAs<type::Stream>() && (style & (CoercionStyle::Assignment | CoercionStyle::FunctionCall)) )
            _result = dst;
    }

    void operator()(const type::Error& e, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Result>() )
            _result = dst;
    }

    void operator()(const type::List& e, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Set>(); t && t->elementType() == e.elementType() )
            _result = dst;

        else if ( auto t = dst.tryAs<type::Vector>(); t && t->elementType() == e.elementType() )
            _result = dst;
    }

    void operator()(const type::Optional& r, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Optional>() ) {
            const auto& s = *r.dereferencedType();
            const auto& d = *t->dereferencedType();

            if ( type::sameExceptForConstness(s, d) && (style & CoercionStyle::Assignment) ) {
                // Assignments copy, so it's safe to turn  into the
                // destination without considering constness.
                _result = dst;
                return;
            }
        }

        if ( auto t = dst.tryAs<type::Bool>(); (style & CoercionStyle::ContextualConversion) && t )
            _result = dst;
    }

    void operator()(const type::StrongReference& r, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Bool>(); (style & CoercionStyle::ContextualConversion) && t ) {
            _result = dst;
            return;
        }

        if ( type::isReferenceType(dst) ) {
            if ( type::sameExceptForConstness(*r.dereferencedType(), *dst.dereferencedType()) ) {
                _result = dst;
                return;
            }
        }

        if ( ! (style & CoercionStyle::Assignment) ) {
            if ( r.dereferencedType() == dst ) {
                _result = dst;
            }
        }
        }

    void operator()(const type::Time& c, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Bool>(); t && (style & CoercionStyle::ContextualConversion) )
            _result = dst;
    }

    void operator()(const type::Result& r, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Bool>(); (style & CoercionStyle::ContextualConversion) && t )
            _result = dst;

        else if ( auto t = dst.tryAs<type::Optional>(); t && t->dereferencedType() == r.dereferencedType() )
            _result = dst;
    }

    void operator()(const type::SignedInteger& src, type::Visitor::position_t&) override {
        if ( dst.isA<type::Bool>() && (style & CoercionStyle::ContextualConversion) )
            _result = dst;

        else if ( auto t = dst.tryAs<type::SignedInteger>() ) {
            if ( src.width() <= t->width() )
                _result = dst;
        }
        }

    void operator()(const type::Stream& c, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::stream::View>() )
            _result = dst;
    }

    void operator()(const type::stream::View& c, type::Visitor::position_t&) override {
        if ( dst.tryAs<type::Bytes>() && (style & (CoercionStyle::Assignment | CoercionStyle::FunctionCall)) )
            _result = dst;
    }

    void operator()(const type::Type_& src, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Type_>() ) {
            // We don't allow arbitrary coercions here, just (more or less) direct matches.
            if ( auto x = hilti::coerceType(src.typeValue(), t->typeValue(), CoercionStyle::TryDirectForMatching) )
                _result = type::Type_(*x);
        }
        }

    void operator()(const type::Union& c, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Bool>(); t && (style & CoercionStyle::ContextualConversion) )
            _result = dst;
    }

    void operator()(const type::UnsignedInteger& src, type::Visitor::position_t&) override {
        if ( dst.isA<type::Bool>() && (style & CoercionStyle::ContextualConversion) ) {
            _result = dst;
            return;
        }

        if ( auto t = dst.tryAs<type::UnsignedInteger>() ) {
            if ( src.width() <= t->width() ) {
                _result = dst;
                return;
            }
        }

        if ( auto t = dst.tryAs<type::SignedInteger>() ) {
            // As long as the target type has more bits, we can coerce.
            if ( src.width() < t->width() ) {
                _result = dst;
                return;
            }
        }
        }

    void operator()(const type::Tuple& src, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Tuple>() ) {
            auto vc = src.elements();
            auto ve = t->elements();

            if ( vc.size() != ve.size() )
                return;

            for ( auto i = std::make_pair(vc.begin(), ve.begin()); i.first != vc.end(); ++i.first, ++i.second ) {
                if ( auto x = hilti::coerceType((*i.first).type(), (*i.second).type()); ! x )
                    return;
            }

            _result = dst;
        }
        }

    void operator()(const type::ValueReference& r, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Bool>(); (style & CoercionStyle::ContextualConversion) && t ) {
            _result = hilti::coerceType(*r.dereferencedType(), dst, style);
            return;
    }

        if ( type::isReferenceType(dst) ) {
            if ( type::sameExceptForConstness(*r.dereferencedType(), *dst.dereferencedType()) ) {
                _result = dst;
                return;
            }
        }

        if ( r.dereferencedType() == dst ) {
            _result = dst;
            return;
        }
    }

    void operator()(const type::WeakReference& r, type::Visitor::position_t&) override {
        if ( auto t = dst.tryAs<type::Bool>(); (style & CoercionStyle::ContextualConversion) && t ) {
            _result = dst;
            return;
        }

        if ( type::isReferenceType(dst) ) {
            if ( type::sameExceptForConstness(*r.dereferencedType(), *dst.dereferencedType()) ) {
                _result = dst;
                return;
            }
        }

        if ( ! (style & CoercionStyle::Assignment) ) {
            if ( r.dereferencedType() == dst ) {
                _result = dst;
                return;
            }
        }
    }
#endif
};

} // anonymous namespace

// Public version going through all plugins.
Result<CtorPtr> hilti::coerceCtor(Builder* builder, CtorPtr c, const QualifiedTypePtr& dst,
                                  bitmask<CoercionStyle> style) {
    if ( c->type() == dst )
        return std::move(c);

    for ( auto p : plugin::registry().plugins() ) {
        if ( ! (p.coerce_ctor) )
            continue;

        if ( auto nc = (*p.coerce_ctor)(builder, c, dst, style) )
            return nc;
    }

    return result::Error("could not coerce type for constructor");
}

static Result<QualifiedTypePtr> _coerceParameterizedType(Builder* builder, const QualifiedTypePtr& src,
                                                         const QualifiedTypePtr& dst, bitmask<CoercionStyle> style) {
    if ( src && dst && *src == *dst )
        return dst;

    if ( src->type()->typename_() != dst->type()->typename_() )
        return {};

    if ( dst->type()->isWildcard() )
        return src;

    auto params1 = src->type()->typeParameters();
    auto params2 = dst->type()->typeParameters();

    if ( params1.size() != params2.size() )
        return {};

    bool have_wildcard = false;

    for ( auto&& [p1, p2] : util::zip2(params1, params2) ) {
        // If we cannot get both parameters as types, we don't have a generic
        // node comparison for the individual parameters, so just stop here and
        // decline. (Note that the case of src == dst has been handled already,
        // that usually does it.)

        auto t1 = p1->tryAs<QualifiedType>();
        if ( ! t1 )
            return {};

        auto t2 = p2->tryAs<QualifiedType>();
        if ( ! t2 )
            return {};

        if ( ! coerceType(builder, t1, t2, style) )
            return {};

        if ( t2->type()->isWildcard() )
            have_wildcard = true;
    }

    // If one of the parameter types is a wildcard, we return the original type
    // instead of the coerced destination type. That's a heuristic that isn't
    // perfect, but will generally do the job. What we'd actually need is a
    // generic way to retype the type parameters, so that we could coerce them
    // individually. But we don't have that capability because all the types
    // compute them dynamically.
    return have_wildcard ? src : dst;
}

static Result<QualifiedTypePtr> _coerceType(Builder* builder, const QualifiedTypePtr& src, const QualifiedTypePtr& dst,
                                            bitmask<CoercionStyle> style) {
    // TODO(robin): Not sure if this should/must replicate all the type coercion
    // login in coerceExpression(). If so, we should factor that out.
    // Update: I believe the answer is yes ... Added a few more cases, but this will
    // likely need more work.

    if ( src->type()->typeID() && dst->type()->typeID() ) {
        if ( *src->type()->typeID() == *dst->type()->typeID() )
            return dst;
        else
            return result::Error("type IDs do not match");
    }

    if ( src == dst )
        return src;

    if ( style & (CoercionStyle::Assignment | CoercionStyle::FunctionCall) ) {
        if ( auto opt = dst->tryAs<type::Optional>() ) {
            if ( dst->type()->isWildcard() )
                return dst;

            // All types converts into a corresponding optional.
            if ( auto x = coerceType(builder, src, opt->dereferencedType(), style) )
                return builder->qualifiedType(builder->typeOptional(*x, src->meta()), false);
        }

        if ( auto opt = dst->type()->tryAs<type::Result>() ) {
            if ( dst->type()->isWildcard() )
                return dst;

            // All types converts into a corresponding result.
            if ( auto x = coerceType(builder, src, opt->dereferencedType(), style) )
                return builder->qualifiedType(builder->typeResult(*x, src->meta()), false);
        }

        if ( auto x = dst->type()->tryAs<type::ValueReference>(); x && ! type::isReferenceType(src) ) {
            // All types converts into a corresponding value_ref.
            if ( auto y = coerceType(builder, src, x->dereferencedType(), style) )
                return builder->qualifiedType(builder->typeValueReference(dst, src->meta()), false);
        }
    }

    if ( type::isParameterized(src) && type::isParameterized(dst) ) {
        if ( auto x = _coerceParameterizedType(builder, src, dst, style) )
            return *x;
    }

    for ( auto p : plugin::registry().plugins() ) {
        if ( ! (p.coerce_type) )
            continue;

        if ( auto nt = (*p.coerce_type)(builder, src, dst, style) )
            return nt;
    }

    return result::Error("cannot coerce types");
}

// Public version going through all plugins.
Result<QualifiedTypePtr> hilti::coerceType(Builder* builder, const QualifiedTypePtr& src, const QualifiedTypePtr& dst,
                                           bitmask<CoercionStyle> style) {
    return _coerceType(builder, src, dst, style);
}

std::string hilti::to_string(bitmask<CoercionStyle> style) {
    std::vector<std::string> labels;

    if ( style & CoercionStyle::TryExactMatch )
        labels.emplace_back("try-exact-match");

    if ( style & CoercionStyle::TryConstPromotion )
        labels.emplace_back("try-const-promotion");

    if ( style & CoercionStyle::TryCoercion )
        labels.emplace_back("try-coercion");

    if ( style & CoercionStyle::Assignment )
        labels.emplace_back("assignment");

    if ( style & CoercionStyle::FunctionCall )
        labels.emplace_back("function-call");

    if ( style & CoercionStyle::OperandMatching )
        labels.emplace_back("operand-matching");

    if ( style & CoercionStyle::DisallowTypeChanges )
        labels.emplace_back("disallow-type-changes");

    if ( style & CoercionStyle::ContextualConversion )
        labels.emplace_back("contextual-conversion");

    return util::join(labels, ",");
};

Result<std::pair<bool, Expressions>> hilti::coerceOperands(Builder* builder, const node::Range<Expression>& exprs,
                                                           const operator_::Operands& operands,
                                                                       bitmask<CoercionStyle> style) {
    int num_type_changes = 0;
    bool changed = false;
    Expressions transformed;

    if ( exprs.size() > operands.size() )
        return result::Error("more expressions than operands");

    for ( const auto&& [i, op] : util::enumerate(operands) ) {
        if ( i >= exprs.size() ) {
            // Running out of operands, must have a default or be optional.
            if ( op.default_ ) {
                transformed.emplace_back(op.default_);
                changed = true;
            }
            else if ( op.optional ) {
                // transformed.push_back(hilti::expression::Ctor(hilti::ctor::Null()));
            }
            else
                return result::Error("stray operand");

            continue;
        }

        auto oat = op.type;
        if ( ! oat ) {
            HILTI_DEBUG(logging::debug::Operator,
                        util::fmt("  [param %d] could not look up operand type -> failure", i));
            return result::Error("could not look up operand type");
        }

        auto result = coerceExpression(builder, exprs[i], oat, style);
        if ( ! result ) {
            HILTI_DEBUG(logging::debug::Operator,
                        util::fmt("  [param %d] matching %s against %s -> failure", i, exprs[i]->type(), *oat));
            return result::Error("could not match coercion operands");
        }

        HILTI_DEBUG(logging::debug::Operator,
                    util::fmt("  [param %d] matching %s against %s -> success: %s (coerced expression is %s) (%s)", i,
                              exprs[i]->type(), *oat, (*result.coerced)->type(),
                              ((*result.coerced)->type()->isConstant() ? "const" : "non-const"),
                              (result.consider_type_changed ? "type changed" : "type not changed")));

        // We check if the primary type of the alternative has changed. Only
        // one operand must change its primary type for an alternative to
        // match.
        if ( result.consider_type_changed && (++num_type_changes > 1 || style & CoercionStyle::DisallowTypeChanges) &&
             ! (style & CoercionStyle::FunctionCall) )
            return result::Error("no valid coercion found");

        transformed.emplace_back(*result.coerced);

        if ( result.nexpr )
            changed = true;
    }

    Expressions x;
    x.reserve(transformed.size());
    for ( const auto& n : transformed )
        x.push_back(n->as<Expression>());

    return std::make_pair(changed, std::move(x));
}

static CoercedExpression _coerceExpression(Builder* builder, const ExpressionPtr& e, const QualifiedTypePtr& src,
                                           const QualifiedTypePtr& dst, bitmask<CoercionStyle> style, bool lhs) {
    if ( ! (style & CoercionStyle::_Recursing) )
        style |= CoercionStyle::_Recursing;

    const auto& no_change = e;
    CoercedExpression _result;
    int _line = 0;

#define RETURN(x)                                                                                                      \
    {                                                                                                                  \
        _result = (x);                                                                                                 \
        _line = __LINE__;                                                                                              \
        goto exit;                                                                                                     \
    }

    const bool dst_is_const = dst->isConstant();
    const bool dst_is_mut = type::isMutable(dst);
    const bool e_is_const = e->isConstant();

    if ( dst->isA<type::Auto>() )
        // Always accept, we're going to update the auto type later.
        RETURN(no_change);

    if ( src->type()->cxxID() && dst->type()->cxxID() ) {
        if ( *src->type()->cxxID() == *dst->type()->cxxID() ) {
            RETURN(no_change);
        }
    }

    if ( src->type()->typeID() && dst->type()->typeID() ) {
        if ( *src->type()->typeID() == *dst->type()->typeID() ) {
            RETURN(no_change);
        }
        else {
            RETURN(result::Error());
        }
    }

    if ( style & CoercionStyle::TryExactMatch ) {
        if ( src == dst ) {
            if ( e_is_const == dst_is_const )
                RETURN(no_change);

            if ( style & CoercionStyle::OperandMatching && ! dst_is_mut )
                RETURN(no_change);
        }

        if ( e_is_const == dst_is_const && type::isParameterized(src) && type::isParameterized(dst) &&
             _coerceParameterizedType(builder, src, dst, CoercionStyle::TryExactMatch) )
            RETURN(no_change); // can say no_change because we're in the ExactMatch case
    }

    if ( style & CoercionStyle::TryConstPromotion ) {
        if ( style & (CoercionStyle::OperandMatching | CoercionStyle::FunctionCall) ) {
            // Don't allow a constant value to match a non-constant operand.
            if ( e_is_const && (! dst_is_const) && dst_is_mut )
                RETURN(result::Error());

            if ( dst->type()->isWildcard() && src->type()->typename_() == dst->type()->typename_() )
                RETURN(no_change);

            if ( src == dst )
                RETURN(no_change);

            if ( src->type() == dst->type() ) {
                RETURN(no_change);
            }
        }

        if ( style & CoercionStyle::Assignment ) {
            if ( src == dst )
                RETURN(no_change);

            if ( src->type() == dst->type() )
                RETURN(no_change);

            if ( dst->type()->isWildcard() && src->type()->typename_() == dst->type()->typename_() )
                RETURN(no_change);
        }
    }

    else {
        if ( style & (CoercionStyle::Assignment | CoercionStyle::FunctionCall) ) {
            // Don't allow assigning to a constant.
            if ( dst_is_const )
                RETURN(result::Error());
        }

        if ( style & CoercionStyle::OperandMatching ) {
            // Don't allow a constant value to match a non-constant operand.
            if ( e_is_const && !dst_is_const && dst_is_mut )
                RETURN(result::Error());
        }
    }

    if ( dst->type()->isA<type::Any>() )
        // type::Any accepts anything without actual coercion.
        RETURN(no_change);

    if ( auto x = e->tryAs<expression::Member>() ) {
        // Make sure the expression remains a member expression, as we will
        // be expecting to cast it to that.
        if ( auto t = hilti::coerceType(builder, x->type(), dst, style) ) {
            RETURN(CoercedExpression(src, builder->expressionMember(*t, x->id(), x->meta())));
        }
        else
            RETURN(result::Error());
    }

    if ( auto o = dst->template tryAs<type::OperandList>() ) {
        // Match tuple against operands according to function call rules.
        HILTI_DEBUG(logging::debug::Operator, util::fmt("matching against call parameters"));
        logging::DebugPushIndent _(logging::debug::Operator);

        auto c = e->template tryAs<expression::Ctor>();
        if ( ! c )
            RETURN(CoercedExpression());

        // TODO(robin): Why do we need this block? We do a separate operand
        // matching afterwards, too.

        if ( auto t = c->ctor()->template tryAs<hilti::ctor::Tuple>() ) {
            CoercionStyle function_style =
                (style & CoercionStyle::TryCoercion ? CoercionStyle::TryAllForFunctionCall :
                                                      CoercionStyle::TryDirectMatchForFunctionCall);
            if ( auto result = coerceOperands(builder, t->value(), o->operands(), function_style) ) {
                if ( result->first ) {
                    RETURN(CoercedExpression(e->type(), builder->expressionCtor(builder->ctorTuple(result->second))));
                }
                else {
                    RETURN(no_change);
                }
            }
        }

        RETURN(CoercedExpression());
    }

    if ( style & (CoercionStyle::Assignment | CoercionStyle::FunctionCall) ) {
        if ( auto opt = dst->tryAs<type::Optional>() ) {
            if ( opt->isWildcard() )
                RETURN(no_change);

            // All types converts into a corresponding optional.
            if ( auto x = coerceExpression(builder, e, opt->dereferencedType(), style) )
                RETURN(CoercedExpression(src, builder->expressionCoerced(*x.coerced, dst, e->meta())));
        }

        if ( auto result = dst->tryAs<type::Result>() ) {
            if ( result->isWildcard() )
                RETURN(no_change);

            // All types convert into a corresponding result.
            if ( auto x = coerceExpression(builder, e, result->dereferencedType(), style) )
                RETURN(CoercedExpression(src, builder->expressionCoerced(*x.coerced, dst, e->meta())));
        }

        if ( auto x = dst->tryAs<type::ValueReference>(); x && ! type::isReferenceType(src) ) {
            // All types converts into a corresponding value_ref.
            if ( auto y = coerceExpression(builder, e, x->dereferencedType(), style) )
                RETURN(CoercedExpression(src, builder->expressionCoerced(*y.coerced, dst, e->meta())));
        }
    }

    if ( style & CoercionStyle::TryCoercion ) {
        if ( auto c = e->tryAs<expression::Ctor>() ) {
            if ( auto nc = hilti::coerceCtor(builder, c->ctor(), dst, style) )
                RETURN(CoercedExpression(src, builder->expressionCtor(builder->ctorCoerced(c->ctor(), *nc, c->meta()),
                                                                      e->meta())));
        }

        if ( auto t = hilti::coerceType(builder, src, dst, style) )
            // We wrap the expression into a coercion even if the new type is
            // the same as *dst*. That way the overloader has a way to
            // recognize that the types aren't identical.
            RETURN(CoercedExpression(src, builder->expressionCoerced(e, *t, e->meta())));
    }

    _result = result::Error();

exit:
    if ( logger().isEnabled(logging::debug::Operator) )
        HILTI_DEBUG(logging::debug::Operator,
                    util::fmt("coercing %s %s (%s) to %s%s (%s) -> %s [%s] (%s) (#%d)",
                              (e_is_const ? "const" : "non-const"), *src,
                              util::replace(src->typename_(), "hilti::type::", ""), (dst_is_const ? "" : "non-const "),
                              *dst, util::replace(dst->typename_(), "hilti::type::", ""),
                              (_result ?
                                   util::fmt("%s %s (%s)", ((*_result.coerced)->isConstant() ? "const" : "non-const"),
                                             (*_result.coerced)->type(),
                                             util::replace((*_result.coerced)->type()->typename_(),
                                                           "hilti::type::", "")) :
                                   "fail"),
                              to_string(style), e->meta().location(), _line));

#undef RETURN

    return _result;
}

// Public version going through all plugins.
CoercedExpression hilti::coerceExpression(Builder* builder, const ExpressionPtr& e, const QualifiedTypePtr& src,
                                          const QualifiedTypePtr& dst, bitmask<CoercionStyle> style, bool lhs) {
    return _coerceExpression(builder, e, src, dst, style, lhs);
}

// Public version going through all plugins.
CoercedExpression hilti::coerceExpression(Builder* builder, const ExpressionPtr& e, const QualifiedTypePtr& dst,
                                          bitmask<CoercionStyle> style, bool lhs) {
    return coerceExpression(builder, e, e->type(), dst, style, lhs);
}

// Plugin-specific version just kicking off the local visitor.
CtorPtr hilti::detail::coerceCtor(Builder* builder, const CtorPtr& c, const QualifiedTypePtr& dst,
                                  bitmask<CoercionStyle> style) {
    util::timing::Collector _("hilti/compiler/ast/coerce");

    if ( ! (type::isResolved(c->type()) && type::isResolved(dst)) )
        return {};

    auto v = VisitorCtor(dst, style);
    return hilti::visitor::visit(std::move(v), c, [](const auto& v) { return v.result; });
}

// Plugin-specific version just kicking off the local visitor.
QualifiedTypePtr hilti::detail::coerceType(Builder* builder, const QualifiedTypePtr& t, const QualifiedTypePtr& dst,
                                           bitmask<CoercionStyle> style) {
    util::timing::Collector _("hilti/compiler/ast/coerce");

    if ( ! (type::isResolved(t) && type::isResolved(dst)) )
        return {};

    auto v = VisitorType(dst, style);
    return hilti::visitor::visit(std::move(v), t, [](const auto& v) { return v.result; });
}
