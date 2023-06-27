// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <optional>

#include <hilti/ast/declarations/global-variable.h>
#include <hilti/ast/expressions/all.h>
#include <hilti/ast/types/optional.h>
#include <hilti/ast/types/reference.h>
#include <hilti/ast/types/result.h>
#include <hilti/base/logger.h>
#include <hilti/compiler/detail/codegen/codegen.h>
#include <hilti/compiler/detail/cxx/all.h>

#include "ast/types/integer.h"
#include "ast/types/interval.h"
#include "ast/types/time.h"
#include "ast/types/tuple.h"
#include "ast/types/vector.h"
#include "ast/types/stream.h"
#include "ast/types/bytes.h"
#include "ast/types/set.h"

using namespace hilti;
using util::fmt;

using namespace hilti::detail;

namespace {

struct Visitor : public hilti::visitor::PreOrder {
    Visitor(CodeGen* cg, const cxx::Expression& expr, const QualifiedTypePtr& src, const QualifiedTypePtr& dst) : cg(cg), expr(expr), src(src), dst(dst) {}

    CodeGen* cg;
    const cxx::Expression& expr;
    const QualifiedTypePtr& src;
    const QualifiedTypePtr& dst;

    std::optional<cxx::Expression> result;

    void operator()(type::Bytes* src) final {
        if ( auto t = dst->tryAs<type::Stream>() )
            result = fmt("::hilti::rt::Stream(%s)", expr);

        else
            logger().internalError(fmt("codegen: unexpected type coercion from bytes to %s", dst->typename_()));
    }

    void operator()(type::Enum* t) final {
        if ( auto t = dst->tryAs<type::Bool>() ) {
            auto id = cg->compile(src, codegen::TypeUsage::Storage);
            result = fmt("(%s != %s(%s::Undef))", expr, id, id);
        }

        else
            logger().internalError(fmt("codegen: unexpected type coercion from enum to %s", dst->typename_()));
    }

    void operator()(type::Error* t) final {
        if ( auto t = dst->tryAs<type::Result>() ) {
            result = fmt("%s(%s)", cg->compile(dst, codegen::TypeUsage::Storage), expr);
        }

        else
            logger().internalError(fmt("codegen: unexpected type coercion from error to %s", dst->typename_()));
    }

    void operator()(type::Interval* t) final {
        if ( auto t = dst->tryAs<type::Bool>() ) {
            auto id = cg->compile(src, codegen::TypeUsage::Storage);
            result = fmt("(%s != hilti::rt::Interval())", expr);
        }

        else
            logger().internalError(fmt("codegen: unexpected type coercion from interval to %s", dst->typename_()));
    }

    void operator()(type::List* t) final {
        if ( auto t = dst->tryAs<type::Set>() )
            result = fmt("::hilti::rt::Set(%s)", expr);

        else if ( auto t = dst->tryAs<type::Vector>() ) {
            auto x = cg->compile(t->elementType(), codegen::TypeUsage::Storage);

            std::string allocator;
            if ( auto def = cg->typeDefaultValue(t->elementType()) )
                allocator = fmt(", hilti::rt::vector::Allocator<%s, %s>", x, *def);

            result = fmt("::hilti::rt::Vector<%s%s>(%s)", x, allocator, expr);
        }

        else
            logger().internalError(fmt("codegen: unexpected type coercion from lisst to %s", dst->typename_()));
    }

    void operator()(type::Optional* t) final {
        if ( auto t = dst->tryAs<type::Optional>() ) {
            // Create tmp to avoid evaluation "expr" twice.
            auto tmp = cg->addTmp("opt", cg->compile(src, codegen::TypeUsage::Storage));
            result = {fmt("(%s = (%s), %s.has_value() ? std::make_optional(*%s) : std::nullopt)", tmp, expr, tmp, tmp),
                      cxx::Side::LHS};
        }

        else if ( auto t = dst->tryAs<type::Bool>() )
            result = fmt("%s.has_value()", expr);

        else
            logger().internalError(fmt("codegen: unexpected type coercion from optional to %s", dst->typename_()));
    }

    void operator()(type::StrongReference* t) final {
        if ( auto t = dst->tryAs<type::Bool>() )
            result = fmt("::hilti::rt::Bool(static_cast<bool>(%s))", expr);

        else if ( auto t = dst->tryAs<type::ValueReference>() )
            result = fmt("%s.derefAsValue()", expr);

        else if ( auto t = dst->tryAs<type::WeakReference>() )
            result = fmt("::hilti::rt::WeakReference<%s>(%s)",
                         cg->compile(t->dereferencedType(), codegen::TypeUsage::Ctor), expr);

        else if ( t->dereferencedType() == dst )
            result = {fmt("(*%s)", expr), cxx::Side::LHS};

        else
            logger().internalError(fmt("codegen: unexpected type coercion from %s to %s", *t, dst->typename_()));
    }

    void operator()(type::Time* t) final {
        if ( auto t = dst->tryAs<type::Bool>() ) {
            auto id = cg->compile(src, codegen::TypeUsage::Storage);
            result = fmt("(%s != hilti::rt::Time())", expr);
        }

        else
            logger().internalError(fmt("codegen: unexpected type coercion from time to %s", dst->typename_()));
    }

    void operator()(type::Result* t) final {
        if ( auto t = dst->tryAs<type::Bool>() )
            result = fmt("::hilti::rt::Bool(static_cast<bool>(%s))", expr);

        else if ( auto t = dst->tryAs<type::Optional>() )
            result = fmt("static_cast<%s>(%s)", cg->compile(dst, codegen::TypeUsage::Storage), expr);

        else
            logger().internalError(fmt("codegen: unexpected type coercion from result to %s", dst->typename_()));
    }

    void operator()(type::SignedInteger* t) final {
        if ( dst->isA<type::Bool>() )
            result = fmt("::hilti::rt::Bool(static_cast<bool>(%s))", expr);

        else if ( auto t = dst->tryAs<type::SignedInteger>() )
            result = fmt("::hilti::rt::integer::safe<int%d_t>(%s)", t->width(), expr);

        else if ( auto t = dst->tryAs<type::UnsignedInteger>() )
            result = fmt("::hilti::rt::integer::safe<uint%d_t>(%s)", t->width(), expr);

        else
            logger().internalError(
                fmt("codegen: unexpected type coercion from signed integer to %s", dst->typename_()));
    }

    void operator()(type::Stream* t) final {
        if ( auto t = dst->tryAs<type::stream::View>() )
            result = fmt("%s.view()", expr);

        else
            logger().internalError(fmt("codegen: unexpected type coercion from stream to %s", dst->typename_()));
    }

    void operator()(type::Union* t) final {
        if ( auto t = dst->tryAs<type::Bool>() ) {
            auto id = cg->compile(src, codegen::TypeUsage::Storage);
            result = fmt("(%s.index() > 0)", expr);
        }

        else
            logger().internalError(fmt("codegen: unexpected type coercion from union to %s", dst->typename_()));
    }

    void operator()(type::stream::View* t) final {
        if ( auto t = dst->tryAs<type::Bytes>() )
            result = fmt("%s.data()", expr);

        else
            logger().internalError(fmt("codegen: unexpected type coercion from view<stream> to %s", dst->typename_()));
    }

    void operator()(type::Tuple* t) final {
        if ( auto t = dst->tryAs<type::Tuple>() ) {
            std::vector<cxx::Expression> exprs;

            assert(t->elements().size() == t->elements().size());
            for ( auto i = 0U; i < t->elements().size(); i++ )
                exprs.push_back(
                    cg->coerce(fmt("std::get<%d>(%s)", i, expr), t->elements()[i]->type(), t->elements()[i]->type()));

            result = fmt("std::make_tuple(%s)", util::join(exprs, ", "));
        }

        else
            logger().internalError(fmt("codegen: unexpected type coercion from tuple to %s", dst->typename_()));
    }

    void operator()(type::UnsignedInteger* t) final {
        if ( dst->isA<type::Bool>() )
            result = fmt("::hilti::rt::Bool(static_cast<bool>(%s))", expr);

        else if ( auto t = dst->tryAs<type::SignedInteger>() )
            result = fmt("::hilti::rt::integer::safe<int%d_t>(%s)", t->width(), expr);

        else if ( auto t = dst->tryAs<type::UnsignedInteger>() )
            result = fmt("::hilti::rt::integer::safe<uint%d_t>(%s)", t->width(), expr);

        else
            logger().internalError(
                fmt("codegen: unexpected type coercion from unsigned integer to %s", dst->typename_()));
    }

    void operator()(type::WeakReference* t) final {
        if ( auto t = dst->tryAs<type::Bool>() )
            result = fmt("::hilti::rt::Bool(static_cast<bool>(%s))", expr);

        else if ( auto t = dst->tryAs<type::StrongReference>() )
            result = fmt("::hilti::rt::StrongReference<%s>(%s)",
                         cg->compile(t->dereferencedType(), codegen::TypeUsage::Ctor), expr);

        else if ( auto t = dst->tryAs<type::ValueReference>() )
            result = fmt("%s.derefAsValue()", expr);

        else if ( t->dereferencedType() == dst )
            result = {fmt("(*%s)", expr), cxx::Side::LHS};

        else
            logger().internalError(
                fmt("codegen: unexpected type coercion from weak reference to %s", dst->typename_()));
    }

    void operator()(type::ValueReference* t) final {
        if ( auto t = dst->tryAs<type::Bool>() )
            result = cg->coerce(fmt("*%s", expr), t->dereferencedType(), dst);

        else if ( auto t = dst->tryAs<type::ValueReference>();
                  t && *t->dereferencedType()->type() == *t->dereferencedType()->type() )
            result = fmt("%s", expr);

        else if ( auto t = dst->tryAs<type::StrongReference>() )
            result = fmt("::hilti::rt::StrongReference<%s>(%s)",
                         cg->compile(t->dereferencedType(), codegen::TypeUsage::Ctor), expr);

        else if ( auto t = dst->tryAs<type::WeakReference>() )
            result = fmt("::hilti::rt::WeakReference<%s>(%s)",
                         cg->compile(t->dereferencedType(), codegen::TypeUsage::Ctor), expr);

        else if ( t->dereferencedType() == dst )
            result = {fmt("(*%s)", expr), cxx::Side::LHS};

        else
            logger().internalError(
                fmt("codegen: unexpected type coercion from value reference to %s", dst->typename_()));
    }
};

} // anonymous namespace

cxx::Expression CodeGen::coerce(const cxx::Expression& e, const QualifiedTypePtr& src, const QualifiedTypePtr& dst) {
    if ( src->type() == dst->type() )
        // If only difference is constness, nothing to do.
        return e;

    if ( auto t = dst->tryAs<type::Optional>(); t && ! src->isA<type::Optional>() )
        return fmt("%s(%s)", compile(dst, codegen::TypeUsage::Storage), e);

    if ( auto t = dst->tryAs<type::Result>() )
        return fmt("%s(%s)", compile(dst, codegen::TypeUsage::Storage), e);

    if ( dst->tryAs<type::ValueReference>() && ! type::isReferenceType(src) )
        return e;

    auto v = Visitor(this, e, src, dst);
    if ( auto nt = hilti::visitor::dispatch(v, src->type(), [](const auto& v) { return v.result; }) )
        return *nt;

    logger().internalError(fmt("codegen: type %s unhandled for coercion", src->typename_()));
}
