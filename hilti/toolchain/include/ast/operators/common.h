// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/expressions/resolved-operator.h>
#include <hilti/ast/forward.h>

#define HILTI_OPERATOR(ns, cls)                                                                                        \
    namespace ns {                                                                                                     \
    class cls : public hilti::expression::ResolvedOperator {                                                           \
    public:                                                                                                            \
        static NodeDerivedPtr<cls> create(ASTContext* ctx, const Operator* op, const QualifiedTypePtr& result,         \
                                          const Expressions& operands, const hilti::Meta& meta) {                      \
            return NodeDerivedPtr<cls>(new cls(op, result, operands, meta));                                           \
        }                                                                                                              \
                                                                                                                       \
        HILTI_NODE(cls)                                                                                                \
                                                                                                                       \
    private:                                                                                                           \
        using hilti::expression::ResolvedOperator::ResolvedOperator;                                                   \
                                                                                                                       \
        bool isEqual(const Node& other) const override { return ResolvedOperator::isEqual(other); }                    \
    };                                                                                                                 \
    } // namespace ns

#if 0

/** Internal helper macro. */
#define __BEGIN_OPERATOR_CUSTOM(ns, op, cls)                                                                           \
    namespace ns {                                                                                                     \
    /** AST node for an operator expression. */                                                                        \
    class cls : public hilti::expression::ResolvedOperator {                                                           \
    public:                                                                                                            \
        static auto create(ASTContext* ctx, OperatorPtr op, Expressions operands, const Meta& meta) {                  \
            return NodeDerivedPtr<cls>(new cls(std::move(operands), std::move(op), meta));                             \
        }                                                                                                              \
                                                                                                                       \
        HILTI_NODE(cls)                                                                                                \
    private:                                                                                                           \
        using hilti::expression::ResolvedOperator::ResolvedOperator;                                                   \
        bool isEqual(const Node& other) const override { return ResolvedOperator::isEqual(other); }                    \
                                                                                                                       \
    public:                                                                                                            \
        /** Class implementing operator interface. */                                                                  \
        struct Operator : public hilti::Operator {                                                                     \
            ~Operator() final {}                                                                                       \
                                                                                                                       \
            ::hilti::operator_::Kind kind() const final { return ::hilti::operator_::Kind::op; }                       \
            std::string docNamespace() const final { return #ns; }                                                     \
                                                                                                                       \
            static ExpressionPtr instantiate(ASTContext* ctx, Expressions operands,                                    \
                                             const Meta& meta = {}) const final {                                      \
                return cls::create(ctx, op, std::move(operands), meta);                                                \
            }


/** Internal helper macro. */
#define __END_OPERATOR_CUSTOM                                                                                          \
    }                                                                                                                  \
    ;                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    }                                                                                                                  \
    ;                                                                                                                  \
    }

/**
 * Starts definition of an operator. This macro is for the simple case where
 * the return type is static and no custom validation is needed.
 *
 * @param ns namespace to define the operator in
 * @param op ``operator_::Kind`` for the operator
 */
#define BEGIN_OPERATOR(ns, op) __BEGIN_OPERATOR_CUSTOM(ns, op, op)

/** Ends definition of a method call operator. */
#define END_OPERATOR                                                                                                   \
    const operator_::Operands operands() const final { return signature().args; }                                      \
                                                                                                                       \
    std::string doc() const final { return signature().doc; }                                                          \
                                                                                                                       \
    QualifiedTypePtr result(const hilti::node::Range<hilti::Expression>& ops) const final {                            \
        return *hilti::operator_::type(signature().result, ops, ops);                                                  \
    }                                                                                                                  \
                                                                                                                       \
    bool isLhs() const final { return signature().lhs; }                                                               \
    hilti::operator_::Priority priority() const final { return signature().priority; }                                 \
                                                                                                                       \
    void validate(const hilti::expression::ResolvedOperator& /* i */) const final {}                                   \
                                                                                                                       \
    __END_OPERATOR_CUSTOM

/**
 * Starts definition of an operator that provides its own implementation of the
 * API's methods.
 *
 * @param ns namespace to define the operator in
 * @param op ``operator_::dnd`` for the operator
 */
#define BEGIN_OPERATOR_CUSTOM(ns, op) __BEGIN_OPERATOR_CUSTOM(ns, op, op)

/**
 * Ends definition of an operator that provides its own implementation of the
 * API's methods..
 */
#define END_OPERATOR_CUSTOM __END_OPERATOR_CUSTOM

/**
 * Starts definition of an operator that provides its own implementation of the
 * API's methods. This version allows to specify a custom class name, which
 * allows for overloading.
 *
 * @param ns namespace to define the operator in
 * @param cls name of the operator's class
 * @param op ``operator_::Kind`` for the operator
 */
#define BEGIN_OPERATOR_CUSTOM_x(ns, cls, op) __BEGIN_OPERATOR_CUSTOM(ns, op, cls)

/**
 * Ends definition of an operator that provides its own implementation of the
 * API's methods. This ends the version that specify's a custom class name.
 */
#define END_OPERATOR_CUSTOM_x __END_OPERATOR_CUSTOM


/**
 * Shortcut version for defining a straight-forward operator with 1 operand.
 */
#define STANDARD_OPERATOR_1(ns, op, result_, ty_op1, doc_)                                                             \
    BEGIN_OPERATOR(ns, op)                                                                                             \
        const auto& signature() const final {                                                                          \
            static hilti::operator_::Signature _signature = {.result = result_,                                        \
                                                             .args =                                                   \
                                                                 {                                                     \
                                                                     {"op", ty_op1},                                   \
                                                                 },                                                    \
                                                             .doc = doc_};                                             \
            return _signature;                                                                                         \
        }                                                                                                              \
    END_OPERATOR

/**
 * Shortcut version for defining a straight-forward operator with 1 operand.
 */
#define STANDARD_OPERATOR_1x(ns, cls, op, result_, ty_op1, doc_)                                                       \
    __BEGIN_OPERATOR_CUSTOM(ns, op, cls)                                                                               \
    const auto& signature() const final {                                                                              \
        static hilti::operator_::Signature _signature = {.result = result_,                                            \
                                                         .args =                                                       \
                                                             {                                                         \
                                                                 {"op", ty_op1},                                       \
                                                             },                                                        \
                                                         .doc = doc_};                                                 \
        return _signature;                                                                                             \
    }                                                                                                                  \
    END_OPERATOR

/**
 * Shortcut version for defining a straight-forward operator with 2 operands.
 */
#define STANDARD_OPERATOR_2(ns, op, result_, ty_op1, ty_op2, doc_)                                                     \
    BEGIN_OPERATOR(ns, op)                                                                                             \
        const auto& signature() const final {                                                                          \
            static hilti::operator_::Signature _signature = {.result = result_,                                        \
                                                             .args = {{"op0", ty_op1}, {"op1", ty_op2}},               \
                                                             .doc = doc_};                                             \
            return _signature;                                                                                         \
        }                                                                                                              \
    END_OPERATOR

/**
 * Shortcut version for defining a straight-forward operator with 2 operands.
 */
#define STANDARD_OPERATOR_2x(ns, cls, op, result_, ty_op1, ty_op2, doc_)                                               \
    __BEGIN_OPERATOR_CUSTOM(ns, op, cls)                                                                               \
    const auto& signature() const final {                                                                              \
        static hilti::operator_::Signature _signature = {.result = result_,                                            \
                                                         .args = {{"op0", ty_op1}, {"op1", ty_op2}},                   \
                                                         .doc = doc_};                                                 \
        return _signature;                                                                                             \
    }                                                                                                                  \
    END_OPERATOR

/**
 * Shortcut version for defining a low priority operator with 2 operands.
 */
#define STANDARD_OPERATOR_2x_low_prio(ns, cls, op, result_, ty_op1, ty_op2, doc_)                                      \
    __BEGIN_OPERATOR_CUSTOM(ns, op, cls)                                                                               \
    const auto& signature() const final {                                                                              \
        static hilti::operator_::Signature _signature = {.priority = operator_::Priority::Low,                         \
                                                         .result = result_,                                            \
                                                         .args = {{"op0", ty_op1}, {"op1", ty_op2}},                   \
                                                         .doc = doc_};                                                 \
        return _signature;                                                                                             \
    }                                                                                                                  \
    END_OPERATOR

/**
 * Shortcut version for defining a straight-forward LHS operator with 2 operands.
 */
#define STANDARD_OPERATOR_2x_lhs(ns, cls, op, result_, ty_op1, ty_op2, doc_)                                           \
    __BEGIN_OPERATOR_CUSTOM(ns, op, cls)                                                                               \
    const auto& signature() const final {                                                                              \
        static hilti::operator_::Signature _signature = {.lhs = true,                                                  \
                                                         .result = result_,                                            \
                                                         .args = {{"op0", ty_op1}, {"op1", ty_op2}},                   \
                                                         .doc = doc_};                                                 \
        return _signature;                                                                                             \
    }                                                                                                                  \
    END_OPERATOR

/**
 * Shortcut version for defining a straight-forward operator with 3 operands.
 */
#define STANDARD_OPERATOR_3(ns, op, result_, ty_op1, ty_op2, ty_op3, doc_)                                             \
    BEGIN_OPERATOR(ns, op)                                                                                             \
        const auto& signature() const final {                                                                          \
            static hilti::operator_::Signature _signature =                                                            \
                {.result = result_, .args = {{"op0", ty_op1}, {"op1", ty_op2}, {"op2", ty_op3}}, .doc = doc_};         \
            return _signature;                                                                                         \
        }                                                                                                              \
    END_OPERATOR

/**
 * Starts definition of a method call operator. This macroi is for the simple
 * case where the return type is static and no custom validation is needed.
 *
 * @param ns namespace to define the operator in
 * @param op Name for the operator (i.e., it's C++-level ID)
 */
#define BEGIN_METHOD(ns, method) __BEGIN_OPERATOR_CUSTOM(ns, MemberCall, method)

/**
 * Starts definition of a method call operator that provides its own result()
 * and validate() implementation.
 *
 * @param ns namespace to define the operator in
 * @param op Name for the operator (i.e., it's C++-level ID)
 */
#define BEGIN_METHOD_CUSTOM_RESULT(ns, method) __BEGIN_OPERATOR_CUSTOM(ns, MemberCall, method)

/** Internal helper macro. */
#define __END_METHOD                                                                                                   \
    const std::vector<hilti::operator_::Operand>& operands() const final {                                             \
        static std::vector<hilti::operator_::Operand> _operands = {{{}, signature().self},                             \
                                                                   {{}, hilti::type::Member(signature().id)},          \
                                                                   {{}, hilti::type::OperandList(signature().args)}};  \
        return _operands;                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    std::string doc() const final { return signature().doc; }

/** Ends definition of a method call operator. */
#define END_METHOD                                                                                                     \
    __END_METHOD                                                                                                       \
                                                                                                                       \
    QualifiedTypePtr result(const hilti::node::Range<hilti::Expression>& ops) const final {                            \
        return *hilti::operator_::type(signature().result, hilti::node::Range(ops), ops);                              \
    }                                                                                                                  \
                                                                                                                       \
    bool isLhs() const final { return false; }                                                                         \
    hilti::operator_::Priority priority() const final { return signature().priority; }                                 \
                                                                                                                       \
    void validate(const hilti::expression::ResolvedOperator& /* i */) const final {}                                   \
                                                                                                                       \
    __END_OPERATOR_CUSTOM

/**
 * Ends definition of a method call operator that provides its own result()
 * and validate() implementation.
 */
#define END_METHOD_CUSTOM_RESULT                                                                                       \
    __END_METHOD                                                                                                       \
    __END_OPERATOR_CUSTOM

/**
 * Starts definition of a constructor-style call operator.
 *
 * @param ns namespace to define the operator in
 * @param cls name of the operator's class
 */
#define BEGIN_CTOR(ns, cls) __BEGIN_OPERATOR_CUSTOM(ns, Call, cls)

#define END_CTOR                                                                                                       \
    const std::vector<hilti::operator_::Operand>& operands() const final {                                             \
        static std::vector<hilti::operator_::Operand> _operands = {{{}, hilti::type::Type_(ctorType())},               \
                                                                   {{}, hilti::type::OperandList(signature().args)}};  \
        return _operands;                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    std::string doc() const final { return signature().doc; }                                                          \
                                                                                                                       \
    QualifiedTypePtr result(const hilti::node::Range<hilti::Expression>& ops) const final {                            \
        if ( ops.size() )                                                                                              \
            return ops[0].type().as<hilti::type::Type_>().typeValue();                                                 \
                                                                                                                       \
        return ctorType();                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    bool isLhs() const final { return false; }                                                                         \
    hilti::operator_::Priority priority() const final { return signature().priority; }                                 \
                                                                                                                       \
    void validate(const hilti::expression::ResolvedOperator& /* i */, hilti::operator_::position_t /* p */)            \
        const final {}                                                                                                 \
                                                                                                                       \
    __END_OPERATOR_CUSTOM

/** Defines a constructor-style call operator introduced by a keyword. */
#define BEGIN_KEYWORD_CTOR(ns, cls, kw, result_, doc_)                                                                 \
    __BEGIN_OPERATOR_CUSTOM(ns, Call, cls)                                                                             \
                                                                                                                       \
    const auto& signature() const final {                                                                              \
        static hilti::operator_::Signature _signature = {.result = result_, .args = parameters(), .doc = doc_};        \
        return _signature;                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    const std::vector<hilti::operator_::Operand>& operands() const final {                                             \
        static std::vector<hilti::operator_::Operand> _operands = {{{}, hilti::type::Member(kw)},                      \
                                                                   {{}, hilti::type::OperandList(signature().args)}};  \
        return _operands;                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    std::string doc() const final { return signature().doc; }                                                          \
                                                                                                                       \
    QualifiedTypePtr result(const hilti::node::Range<hilti::Expression>& ops) const final {                            \
        return *hilti::operator_::type(signature().result, ops, ops);                                                  \
    }                                                                                                                  \
                                                                                                                       \
    bool isLhs() const final { return signature().lhs; }                                                               \
    hilti::operator_::Priority priority() const final { return signature().priority; }                                 \
                                                                                                                       \
    void validate(const hilti::expression::ResolvedOperator& /* i */, hilti::operator_::position_t /* p */)            \
        const final {}


#define END_KEYWORD_CTOR __END_OPERATOR_CUSTOM

/** Shortcut to define a constructor-style call operator introduced by a keyword using a single argument. */
#define STANDARD_KEYWORD_CTOR(ns, cls, kw, result_, ty_op, doc_)                                                       \
    BEGIN_KEYWORD_CTOR(ns, cls, kw, result_, doc_)                                                                     \
                                                                                                                       \
        std::vector<Operand> parameters() const final { return {{"op", ty_op}}; }                                      \
                                                                                                                       \
    END_KEYWORD_CTOR

/**
 * No-op to have the auto-generated code pick up on an operator that's
 * fully defined separately.
 */
#define OPERATOR_DECLARE_ONLY(ns, cls)
#endif
