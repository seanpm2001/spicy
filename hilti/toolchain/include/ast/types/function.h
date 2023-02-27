// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/parameter.h>
#include <hilti/ast/forward.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/auto.h>

namespace hilti::type {

namespace function {

using Parameter = declaration::Parameter;
using ParameterPtr = declaration::ParameterPtr;
using Parameters = declaration::Parameters;

namespace parameter {
using Kind = declaration::parameter::Kind;
} // namespace parameter

/**
 * A function's flavor differentiates between a set of "function-like"
 * language element.
 */
enum class Flavor {
    Hook,    /**< a hook */
    Method,  /**< a struct method */
    Standard /**< a normal function */
};

namespace detail {
constexpr util::enum_::Value<Flavor> flavors[] = {
    {Flavor::Hook, "hook"},
    {Flavor::Method, "method"},
    {Flavor::Standard, "standard"},
};
} // namespace detail

constexpr auto to_string(Flavor f) { return util::enum_::to_string(f, detail::flavors); }

namespace flavor {
constexpr auto from_string(const std::string_view& s) { return util::enum_::from_string<Flavor>(s, detail::flavors); }
} // namespace flavor

} // namespace function

/** AST node for a `function` type. */
class Function : public UnqualifiedType {
public:
    auto result() const { return child<QualifiedType>(0); }
    auto flavor() const { return _flavor; }

    void setResultType(const QualifiedTypePtr& t) { setChild(0, t); }

    node::Set<type::function::Parameter> parameters() const final {
        node::Set<type::function::Parameter> result;
        for ( auto&& p : children<function::Parameter>(1, -1) )
            result.insert(p);
        return result;
    }

    Nodes typeParameters() const final { return children(); }

    node::Properties properties() const final {
        auto p = node::Properties{{"flavor", to_string(_flavor)}};
        return UnqualifiedType::properties() + p;
    }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& result, const declaration::Parameters& params,
                       function::Flavor flavor = function::Flavor::Standard, const Meta& meta = {}) {
        return NodeDerivedPtr<Function>(new Function(flatten(result, params), flavor, meta));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Function>(new Function(Wildcard(), std::move(m)));
    }

protected:
    Function(Nodes children, function::Flavor flavor, Meta meta)
        : UnqualifiedType(std::move(children), std::move(meta)), _flavor(flavor) {}

    Function(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isResolved(ResolvedState* rstate) const final {
        if ( result()->isAuto() )
            // We treat this as resolved because (1) it doesn't need to hold up
            // other resolving, and (2) can lead to resolver dead-locks if we
            // let it.
            return true;

        if ( ! type::detail::isResolved(result(), rstate) )
            return false;

        for ( auto p = children().begin() + 1; p != children().end(); p++ ) {
            if ( ! (*p)->as<declaration::Parameter>()->isResolved(rstate) )
                return false;
        }

        return true;
    }

    bool _isParameterized() const final { return true; }

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Function>();
        if ( ! n )
            return false;

        return UnqualifiedType::isEqual(other) && _flavor == n->_flavor;
    }

    HILTI_NODE(Function)

private:
    function::Flavor _flavor;
};

using FunctionPtr = std::shared_ptr<Function>;

} // namespace hilti::type


// TODO: Do we need this?
/*
 * #<{(|* AST node for a result type. |)}>#
 * class Result : public NodeBase {
 * public:
 *     Result(TypePtr type, Meta m = Meta()) : NodeBase(nodes(std::move(type)), std::move(m)) {}
 *
 *     Result() : NodeBase(nodes(node::none), Meta()) {}
 *
 *     const auto& type() const { return child<TypePtr>(0); }
 *
 *     void setType(const TypePtr& x) { children()[0] = x; }
 *
 *     bool operator==(const Result& other) const { return type() == other.type(); }
 *
 *     #<{(|* Implements the `Node` interface. |)}>#
 *     auto properties() const { return node::Properties{}; }
 * };
 */
