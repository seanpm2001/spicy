// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <hilti/ast/attribute.h>
#include <hilti/ast/forward.h>
#include <hilti/ast/node.h>
#include <hilti/ast/statement.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/function.h>

namespace hilti {

namespace function {

/** A function's calling convention. */
enum class CallingConvention {
    Extern,          /**< function can be called from external C++ code */
    ExternNoSuspend, /**< function can be called from external C++ code, and is guaranteed to not suspend. */
    Standard         /**< default, nothing special */
};

namespace detail {
constexpr util::enum_::Value<CallingConvention> conventions[] = {
    {CallingConvention::Extern, "extern"},
    {CallingConvention::ExternNoSuspend, "extern-no-suspend"},
    {CallingConvention::Standard, "<standard>"},
};
} // namespace detail

constexpr auto to_string(CallingConvention cc) { return util::enum_::to_string(cc, detail::conventions); }

namespace calling_convention {
constexpr inline auto from_string(const std::string_view& s) {
    return util::enum_::from_string<CallingConvention>(s, detail::conventions);
}
} // namespace calling_convention

} // namespace function
/** Base class for function nodes. */
class Function : public Node {
public:
    ~Function() override;

    const auto& id() const { return _id; }
    auto type() const { return child<QualifiedType>(0); }
    auto ftype() const { return child<QualifiedType>(0)->type()->as<type::Function>(); }
    auto body() const { return child<Statement>(1); }
    auto attributes() const { return child<AttributeSet>(2); }
    auto callingConvention() const { return _cc; }
    bool isStatic() const { return attributes()->find("&static")->hasValue(); }

    void setBody(const StatementPtr& b) { setChild(1, b); }
    void setID(const ID& id) { _id = id; }
    void setResultType(const QualifiedTypePtr& t) { ftype()->setResultType(t); }

    node::Properties properties() const override {
        auto p = node::Properties{{"cc", to_string(_cc)}};
        return Node::properties() + p;
    }

    static auto create(ASTContext* ctx, const ID& id, const type::FunctionPtr& ftype, const StatementPtr& body,
                       function::CallingConvention cc = function::CallingConvention::Standard,
                       const AttributeSetPtr& attrs = nullptr, const Meta& meta = {}) {
        return NodeDerivedPtr<Function>(
            new Function({QualifiedType::create(ctx, ftype, true, meta), body, attrs}, id, cc, meta));
    }

protected:
    Function(Nodes children, ID id, function::CallingConvention cc, Meta meta = {})
        : Node(std::move(children), std::move(meta)), _id(std::move(id)), _cc(cc) {}

    std::string _render() const override;

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Function>();
        if ( ! n )
            return false;

        return Node::isEqual(other) && _id == n->_id && _cc == n->_cc;
    }

    HILTI_NODE(Function);

private:
    ID _id;
    function::CallingConvention _cc;
};

} // namespace hilti
