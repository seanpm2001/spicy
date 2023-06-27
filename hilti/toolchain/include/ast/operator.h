// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <hilti/ast/forward.h>
#include <hilti/ast/node.h>
#include <hilti/ast/types/operand-list.h>
#include <hilti/base/logger.h>

namespace hilti {

namespace expression {
class ResolvedOperator;
}

namespace operator_ {
class Registry;

/** Enumeration of all types of operators that HILTI supports. */
enum class Kind {
    Add,
    Begin,
    BitAnd,
    BitOr,
    BitXor,
    Call,
    Cast,
    CustomAssign,
    DecrPostfix,
    DecrPrefix,
    Delete,
    Deref,
    Difference,
    DifferenceAssign,
    Division,
    DivisionAssign,
    Equal,
    End,
    Greater,
    GreaterEqual,
    HasMember,
    In,
    IncrPostfix,
    IncrPrefix,
    Index,
    IndexAssign,
    Lower,
    LowerEqual,
    Member,
    MemberCall,
    Modulo,
    Multiple,
    MultipleAssign,
    Negate,
    New,
    Pack,
    Power,
    ShiftLeft,
    ShiftRight,
    SignNeg,
    SignPos,
    Size,
    Sum,
    SumAssign,
    TryMember,
    Unequal,
    Unknown,
    Unpack,
    Unset
};

/** Returns true for operator types that HILTI considers commutative. */
inline auto isCommutative(Kind k) {
    switch ( k ) {
        case Kind::BitAnd:
        case Kind::BitOr:
        case Kind::BitXor:
        case Kind::Equal:
        case Kind::Unequal:
        case Kind::Multiple:
        case Kind::Sum: return true;

        case Kind::Add:
        case Kind::Begin:
        case Kind::Call:
        case Kind::Cast:
        case Kind::CustomAssign:
        case Kind::DecrPostfix:
        case Kind::DecrPrefix:
        case Kind::Delete:
        case Kind::Deref:
        case Kind::Difference:
        case Kind::DifferenceAssign:
        case Kind::Division:
        case Kind::DivisionAssign:
        case Kind::End:
        case Kind::Greater:
        case Kind::GreaterEqual:
        case Kind::HasMember:
        case Kind::In:
        case Kind::IncrPostfix:
        case Kind::IncrPrefix:
        case Kind::Index:
        case Kind::IndexAssign:
        case Kind::Lower:
        case Kind::LowerEqual:
        case Kind::Member:
        case Kind::MemberCall:
        case Kind::Modulo:
        case Kind::MultipleAssign:
        case Kind::Negate:
        case Kind::New:
        case Kind::Pack:
        case Kind::Power:
        case Kind::ShiftLeft:
        case Kind::ShiftRight:
        case Kind::SignNeg:
        case Kind::SignPos:
        case Kind::Size:
        case Kind::SumAssign:
        case Kind::TryMember:
        case Kind::Unknown:
        case Kind::Unpack:
        case Kind::Unset: return false;
    };

    util::cannot_be_reached();
}

namespace detail {
constexpr util::enum_::Value<Kind> kinds[] = {{Kind::Add, "add"},           {Kind::Begin, "begin"},
                                              {Kind::BitAnd, "&"},          {Kind::BitOr, "|"},
                                              {Kind::BitXor, "^"},          {Kind::Call, "call"},
                                              {Kind::Cast, "cast"},         {Kind::CustomAssign, "="},
                                              {Kind::DecrPostfix, "--"},    {Kind::DecrPrefix, "--"},
                                              {Kind::Delete, "delete"},     {Kind::Deref, "*"},
                                              {Kind::Division, "/"},        {Kind::DivisionAssign, "/="},
                                              {Kind::Equal, "=="},          {Kind::End, "end"},
                                              {Kind::Greater, ">"},         {Kind::GreaterEqual, ">="},
                                              {Kind::HasMember, "?."},      {Kind::In, "in"},
                                              {Kind::IncrPostfix, "++"},    {Kind::IncrPrefix, "++"},
                                              {Kind::Index, "index"},       {Kind::IndexAssign, "index_assign"},
                                              {Kind::Lower, "<"},           {Kind::LowerEqual, "<="},
                                              {Kind::Member, "."},          {Kind::MemberCall, "method call"},
                                              {Kind::Negate, "~"},          {Kind::New, "new"},
                                              {Kind::Difference, "-"},      {Kind::DifferenceAssign, "-="},
                                              {Kind::Modulo, "%"},          {Kind::Multiple, "*"},
                                              {Kind::MultipleAssign, "*="}, {Kind::Sum, "+"},
                                              {Kind::Pack, "unpack"},       {Kind::Unset, "unset"},
                                              {Kind::SumAssign, "+="},      {Kind::Power, "**"},
                                              {Kind::ShiftLeft, "<<"},      {Kind::ShiftRight, ">>"},
                                              {Kind::SignNeg, "-"},         {Kind::SignPos, "+"},
                                              {Kind::Size, "size"},         {Kind::TryMember, ".?"},
                                              {Kind::Unequal, "!="},        {Kind::Unknown, "<unknown>"},
                                              {Kind::Unpack, "unpack"},     {Kind::Unset, "unset"}};

} // namespace detail

/**
 * Returns a descriptive string representation of an operator kind. This is
 * meant just for display purposes, and does not correspond directly to the
 * HILTI code representation (because they may differ based on context).
 */
constexpr auto to_string(Kind m) { return util::enum_::to_string(m, detail::kinds); }

/** Operator priority during resolving relative to others of the same kind. */
enum Priority { Low, Normal };

using Operand = type::operand_list::Operand;
using Operands = type::operand_list::Operands;

struct Signature {
    operator_::Kind kind;
    QualifiedTypePtr result; /**< result of the method; if null, result() will be called dynamically` */
    OperandListPtr operands;
    operator_::Priority priority;

    std::string ns;  /**< namespace where to document this operator */
    std::string doc; /**< documentation string for the autogenerated reference manual */
};

} // namespace operator_

/**
 * Class representing available HILTI operators.
 *
 * Operators aren't AST nodes themselves. Instead, they generally *define* an
 * operator that's available in HILTI. An operator can then instantiated with
 * concrete operands to create a corresponding AST expression node.
 */
class Operator {
public:
    template<typename T>
    using Result = ::hilti::Result<T>;

    Operator() {}
    virtual ~Operator() {}
    Operator(const Operator& other) = delete;
    Operator(Operator&& other) = delete;

    Operator& operator=(const Operator& other) = delete;
    Operator& operator=(Operator&& other) = delete;

    const auto& signature() const {
        assert(_signature);
        return *_signature;
    }

    auto kind() const { return signature().kind; }
    auto operands() const { return signature().operands->operands(); }

    virtual QualifiedTypePtr result(Builder* builder, const expression::UnresolvedOperator* op) const {
        assert(_signature);
        if ( _signature->result )
            return _signature->result;
        else
            logger().internalError("operator::Operator::result() not overridden for dynamic operator result");
    }

    virtual Result<ResolvedOperatorPtr> instantiate(Builder* builder,
                                                    const expression::UnresolvedOperator* op) const = 0;

protected:
    friend class operator_::Registry;
    void init(Builder* builder, const NodePtr& scope_root = nullptr);

    virtual operator_::Signature signature(Builder* builder) const = 0;

private:
    std::optional<operator_::Signature> _signature;
};

} // namespace hilti
