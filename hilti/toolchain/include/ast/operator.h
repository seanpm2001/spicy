// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <hilti/ast/expression.h>
#include <hilti/ast/node.h>
#include <hilti/ast/types/doc-only.h>
#include <hilti/ast/types/type.h>
#include <hilti/base/logger.h>
#include <hilti/base/util.h>

namespace hilti {

namespace operator_ {

namespace detail {
inline auto docType(std::string description) {
    return QualifiedType::create(nullptr, type::DocOnly::create(nullptr, std::move(description)), true);
}

inline auto docTypeUnqualified(std::string description) {
    return type::DocOnly::create(nullptr, std::move(description));
}
} // namespace detail

using OperandType =
    std::variant<QualifiedTypePtr, std::function<QualifiedTypePtr(const hilti::node::Range<Expression>&,
                                                                  const hilti::node::Range<Expression>&)>>;
inline QualifiedTypePtr type(const OperandType& t, const hilti::node::Range<Expression>& orig_ops,
                             const hilti::node::Range<Expression>& resolved_ops) {
    if ( const auto& f = std::get_if<std::function<QualifiedTypePtr(const hilti::node::Range<Expression>&,
                                                                    const hilti::node::Range<Expression>&)>>(&t) )
        return (*f)(orig_ops, resolved_ops);

    return std::get<QualifiedTypePtr>(t);
}

inline QualifiedTypePtr type(const OperandType& t, const hilti::node::Range<Expression>& orig_ops,
                             const Expressions& resolved_ops) {
    auto resolved_ops_as_nodes = node::flatten(resolved_ops);
    return type(t, orig_ops, hilti::node::Range<Expression>(resolved_ops_as_nodes));
}

inline QualifiedTypePtr type(const OperandType& t, const Expressions& orig_ops, const Expressions& resolved_ops) {
    auto orig_ops_as_nodes = node::flatten(orig_ops);
    return type(t, hilti::node::Range<Expression>(orig_ops_as_nodes), resolved_ops);
}

inline auto operandType(unsigned int op, const char* doc = "<no-doc>") {
    return [=](const hilti::node::Range<Expression>& /* orig_ops */,
               const hilti::node::Range<Expression>& resolved_ops) -> QualifiedTypePtr {
        if ( resolved_ops.empty() )
            return detail::docType(doc);

        if ( op >= resolved_ops.size() )
            logger().internalError(util::fmt("operandType(): index %d out of range, only %" PRIu64 " ops available", op,
                                             resolved_ops.size()));

        return resolved_ops[op]->type();
    };
}

inline auto elementType(unsigned int op, const char* doc = "<type of element>") {
    return [=](const hilti::node::Range<Expression>& /* orig_ops */,
               const hilti::node::Range<Expression>& resolved_ops) -> QualifiedTypePtr {
        if ( resolved_ops.empty() )
            return detail::docType(doc);

        if ( op >= resolved_ops.size() )
            logger().internalError(util::fmt("elementType(): index %d out of range, only %" PRIu64 " ops available", op,
                                             resolved_ops.size()));

        if ( type::isIterable(resolved_ops[op]->type()) ) {
            auto t = *resolved_ops[op]->type()->type()->elementType();
            return resolved_ops[op]->type();
        }

        return {};
    };
}

inline auto constantElementType(unsigned int op, const char* doc = "<type of element>") {
    return [=](const hilti::node::Range<Expression>& /* orig_ops */,
               const hilti::node::Range<Expression>& resolved_ops) -> QualifiedTypePtr {
        if ( resolved_ops.empty() )
            return detail::docType(doc);

        if ( op >= resolved_ops.size() )
            logger().internalError(util::fmt("elementType(): index %d out of range, only %" PRIu64 " ops available", op,
                                             resolved_ops.size()));

        if ( type::isIterable(resolved_ops[op]->type()) )
            return resolved_ops[op]->type()->type()->elementType();

        return {};
    };
}

inline auto iteratorType(unsigned int op, bool const_, const char* doc = "<iterator>") {
    return [=](const hilti::node::Range<Expression>& /* orig_ops */,
               const hilti::node::Range<Expression>& resolved_ops) -> UnqualifiedTypePtr {
        if ( resolved_ops.empty() )
            return detail::docTypeUnqualified(doc);

        if ( op >= resolved_ops.size() )
            logger().internalError(util::fmt("iteratorType(): index %d out of range, only %" PRIu64 " ops available",
                                             op, resolved_ops.size()));

        if ( type::isIterable(resolved_ops[op]->type()) )
            return resolved_ops[op]->type()->type()->iteratorType();

        return {};
    };
}

inline auto dereferencedType(unsigned int op, const char* doc = "<dereferenced type>") {
    return [=](const hilti::node::Range<Expression>& /* orig_ops */,
               const hilti::node::Range<Expression>& resolved_ops) -> QualifiedTypePtr {
        if ( resolved_ops.empty() )
            return detail::docType(doc);

        if ( op >= resolved_ops.size() )
            logger().internalError(util::fmt("dereferencedType(): index %d out of range, only %" PRIu64
                                             " ops available",
                                             op, resolved_ops.size()));

        return resolved_ops[op]->type()->type()->dereferencedType();
    };
}

inline auto sameTypeAs(unsigned int op, const char* doc = "<no-doc>") {
    return [=](const hilti::node::Range<Expression>& /* orig_ops */,
               const hilti::node::Range<Expression>& resolved_ops) -> QualifiedTypePtr {
        if ( resolved_ops.empty() )
            return detail::docType(doc);

        if ( op >= resolved_ops.size() )
            logger().internalError(util::fmt("sameTypeAs(): index %d out of range, only %" PRIu64 " ops available", op,
                                             resolved_ops.size()));

        return resolved_ops[op]->type();
    };
}

inline auto typedType(unsigned int op, const char* doc = "<type>") {
    return [=](const hilti::node::Range<Expression>& /* orig_ops */,
               const hilti::node::Range<Expression>& resolved_ops) -> QualifiedTypePtr {
        if ( resolved_ops.empty() )
            return detail::docType(doc);

        if ( op >= resolved_ops.size() )
            logger().internalError(util::fmt("typedType(): index %d out of range, only %" PRIu64 " ops available", op,
                                             resolved_ops.size()));

        return resolved_ops[op]->type()->as<type::Type_>()->typeValue();
    };
}

/** Describes an operand that an operator accepts. */
struct Operand {
    Operand(Operand&&) = default;
    Operand(const Operand&) = default;

    Operand& operator=(Operand&&) = default;
    Operand& operator=(const Operand&) = default;

    Operand(std::optional<ID> _id = {}, OperandType _type = {}, bool _optional = false, ExpressionPtr _default = {},
            std::optional<std::string> _doc = {})
        : id(std::move(_id)),
          type(std::move(_type)),
          optional(_optional),
          default_(std::move(_default)),
          doc(std::move(_doc)) {}

    std::optional<ID> id;             /**< ID for the operand; used only for documentation purposes. */
    OperandType type;                 /**< operand's type */
    bool optional = false;            /**< true if operand can be skipped; `default_` will be used instead */
    ExpressionPtr default_ = nullptr; /**< default valuer if operator is skipped */
    std::optional<std::string> doc;   /**< alternative rendering for the auto-generated documentation */

    bool operator==(const Operand& other) const {
        if ( this == &other )
            return true;

        if ( ! (std::holds_alternative<QualifiedTypePtr>(type) &&
                std::holds_alternative<QualifiedTypePtr>(other.type)) )
            return false;

        return std::get<QualifiedTypePtr>(type) == std::get<QualifiedTypePtr>(other.type) && id == other.id &&
               optional == other.optional && default_ == other.default_;
    }
};

inline std::ostream& operator<<(std::ostream& out, const Operand& op) {
    if ( auto t = std::get_if<QualifiedTypePtr>(&op.type) )
        out << *t;
    else
        out << "<inferred type>";

    if ( op.id )
        out << ' ' << *op.id;

    if ( op.default_ )
        out << " = " << op.default_->print();
    else if ( op.optional )
        out << " (optional)";

    return out;
}

using ResultType = OperandType;

/** Operator priority during resolving relative to others of the same kind. */
enum Priority { Low, Normal };

/**
 * Describes the signature of an operator method.
 *
 * @todo For operands, we only use the type information so far. Instead of
 * using `type::Tuple` to describe the 3rd parameter to a MethodCall
 * operator, we should create a new `type::ArgumentList` that takes a list
 * of `Operand` instances.
 */
struct Signature {
    QualifiedTypePtr self; /**< type the method operates on */
    bool const_ = true;
    bool lhs = false;                  /**< true if operator's result can be assigned to */
    Priority priority = Priority::Low; /**< operator priority */
    ResultType result;                 /**< result of the method; skipped if using `{BEGIN/END}_METHOD_CUSTOM_RESULT` */
    ID id;                             /**< name of the method */
    std::vector<Operand> args;         /**< operands the method receives */
    std::string doc;                   /**< documentation string for the autogenerated reference manual */
};

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

using Operands = std::vector<Operand>;

/**
 * Returns a descriptive string representation of an operator kind. This is
 * meant just for display purposes, and does not correspond directly to the
 * HILTI code representation (because they may differ based on context).
 */
constexpr auto to_string(Kind m) { return util::enum_::to_string(m, detail::kinds); }

} // namespace operator_

/**
 * Base class for HILTI operators.
 *
 * Operators aren't AST nodes themselves. Instead, they generally *define* an
 * operator that's available in HILTI. An operator can then instantiated with
 * concrete operands to create a corresponding AST expression node.
 */
class Operator {
public:
    /** Returns the operator's kind. */
    virtual hilti::operator_::Kind kind() const = 0;

    /**
     * Returns a documentation string associated with the operator. This
     * goes into automatically generated HILTI reference documentation.
     */
    virtual std::string doc() const = 0;

    /**
     * Returns the internal namespace this operator is defined in. This is
     * used for grouping operators in the auto-generated documentation.
     */
    virtual std::string docNamespace() const = 0;

#if 0
    /**
     * Validates correctness of an instantiated version of this operator.
     * This will be called as part of HILTI's AST validation process and
     * should ensure that the operator's operands are valid.
     *
     * @param o instantiated operator expression
     * @param p position of `o` in the AST being validated; can be used to derive further context about the usage
     * @return false if the instantiation doesn't reflect a correct usage of the operator
     */
    virtual void validate(const expression::ResolvedOperator& o) const = 0;
#endif

    /**
     * Returns the HILTI type that the operator yields when evaluated with a
     * given set of operands.
     *
     * @param ops operands to use for deriving the result type; note that
     * when auto-generating documentation, this vector will always be empty.
     * Operators that require expressions to calculate their return type
     * dynamically must catch getting an empty vector and return a
     * type::DocOnly() instance in that case that provides a textual
     * description for documentation.
     *
     * @return type when evaluated
     *
     */
    virtual QualifiedTypePtr result(const hilti::node::Range<Expression>& ops) const = 0;

    /** True if operator's result can be assigned to. */
    virtual bool isLhs() const = 0;

    /** Returns the operators resolver priority relative to others of the same kind. */
    virtual hilti::operator_::Priority priority() const = 0;

    /**
     * Describes the operands that the operator accepts, such as their types
     * and default values.
     *
     * @return
     */
    virtual const std::vector<hilti::operator_::Operand>& operands() const = 0;

    /**
     * Creates an expression representing this operator instantied with a
     * given set of operands. The returned expression will typically be a
     * `expression::ResolvedOperator`.
     *
     * @param ops operands to instantiate the operator with
     * @param m meta information to associated with the instantiated expression
     * @return new expression to integrate into AST
     */
    virtual ExpressionPtr instantiate(const Expressions& operands, const Meta& m) const = 0;
};

using OperatorPtr = std::shared_ptr<Operator>;
using Operators = std::vector<OperatorPtr>;

} // namespace hilti
