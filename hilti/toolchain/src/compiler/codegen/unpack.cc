// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <optional>
#include <string>
#include <utility>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/base/logger.h>
#include <hilti/compiler/detail/codegen/codegen.h>
#include <hilti/compiler/detail/cxx/all.h>
#include "ast/types/integer.h"

using namespace hilti;
using util::fmt;

using namespace hilti::detail;

namespace {

struct Visitor : hilti::visitor::PreOrder {
    enum class Kind { Pack, Unpack };

    Visitor(CodeGen* cg, Kind kind, cxx::Expression data, const std::vector<cxx::Expression>& args)
        : cg(cg), kind(kind), data(std::move(data)), args(args) {}

    CodeGen* cg;
    Kind kind;
    cxx::Expression data;
    const std::vector<cxx::Expression>& args;

    std::optional<std::string> result;

    auto kindToString() const {
        switch ( kind ) {
            case Kind::Pack: return "pack";
            case Kind::Unpack: return "unpack";
        }

        util::cannot_be_reached();
    }

    void operator()(type::Address* n) final {
        switch ( kind ) {
            case Kind::Pack: result = fmt("::hilti::rt::address::pack(%s, %s)", data, args[0]); break;
            case Kind::Unpack: result = fmt("::hilti::rt::address::unpack(%s, %s, %s)", data, args[0], args[1]); break;
        }

        util::cannot_be_reached();
    }

    void operator()(type::UnsignedInteger* n) final {
        result = fmt("::hilti::rt::integer::%s<uint%d_t>(%s, %s)", kindToString(), n->width(), data, args[0]);
    }

    void operator()(type::SignedInteger* n) final {
        result = fmt("::hilti::rt::integer::%s<int%d_t>(%s, %s)", kindToString(), n->width(), data, args[0]);
    }

    void operator()(type::Real* n) final {
        result = fmt("::hilti::rt::real::%s(%s, %s, %s)", kindToString(), data, args[0], args[1]);
    }
};

} // anonymous namespace

cxx::Expression CodeGen::pack(const ExpressionPtr& data, const Expressions& args) {
    auto cxx_args = util::transform(args, [&](const auto& e) { return compile(e, false); });
    auto v = Visitor(this, Visitor::Kind::Pack, compile(data), cxx_args);
    if ( auto result = hilti::visitor::dispatch(v, data->type(), [](const auto& v) { return v.result; }) )
        return cxx::Expression(*result);

    logger().internalError("pack failed to compile", data->type());
}

cxx::Expression CodeGen::pack(const QualifiedTypePtr& t, const cxx::Expression& data,
                              const std::vector<cxx::Expression>& args) {
    auto v = Visitor(this, Visitor::Kind::Pack, data, args);
    if ( auto result = hilti::visitor::dispatch(v, t, [](const auto& v) { return v.result; }) )
        return cxx::Expression(*result);

    logger().internalError("pack failed to compile", t);
}

cxx::Expression CodeGen::unpack(const QualifiedTypePtr& t, const ExpressionPtr& data, const Expressions& args,
                                bool throw_on_error) {
    auto cxx_args = util::transform(args, [&](const auto& e) { return compile(e, false); });
    auto v = Visitor(this, Visitor::Kind::Unpack, compile(data), cxx_args);
    if ( auto result = hilti::visitor::dispatch(v, t, [](const auto& v) { return v.result; }) ) {
        if ( throw_on_error )
            return cxx::Expression(util::fmt("%s.valueOrThrow()", *result));
        else
            return cxx::Expression(*result);
    }

    logger().internalError("unpack failed to compile", t);
}

cxx::Expression CodeGen::unpack(const QualifiedTypePtr& t, const cxx::Expression& data,
                                const std::vector<cxx::Expression>& args, bool throw_on_error) {
    auto v = Visitor(this, Visitor::Kind::Unpack, data, args);
    if ( auto result = hilti::visitor::dispatch(v, t, [](const auto& v) { return v.result; }) ) {
        if ( throw_on_error )
            return cxx::Expression(util::fmt("%s.valueOrThrow<::hilti::rt::InvalidValue>()", *result));
        else
            return cxx::Expression(*result);
    }

    logger().internalError("unpack failed to compile", t);
}
