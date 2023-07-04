// Copyright (c) 2021-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/builder/builder.h>
#include <hilti/ast/expressions/unresolved-operator.h>
#include <hilti/ast/operator.h>
#include <hilti/ast/operators/string.h>

#include "ast/detail/operator-registry.h"

using namespace hilti::expression;
using namespace hilti::operator_;
using namespace hilti::type;
using namespace hilti;

namespace {

class Equal : public Operator {
public:
    operator_::Signature signature(Builder* builder) const final {
        return {.kind = Kind::Equal,
                .operands = builder->typeOperandList({{.type = builder->qualifiedType(builder->typeString(), true)},
                                                      {.type = builder->qualifiedType(builder->typeString(), true)}}),
                .result = builder->qualifiedType(builder->typeBool(), true),
                .ns = "string",
                .doc = "Compares two strings lexicographically."};
    }

    Result<ResolvedOperatorPtr> instantiate(Builder* builder, node::Range<Expression> operands,
                                            const Meta& meta) const final {
        return {operator_::string::Equal::create(builder->context(), this, result(builder, operands, meta), operands,
                                                 meta)};
    }
};

REGISTER_OPERATOR(Equal);

class Encode : public MethodCall {
public:
    operator_::Signature signature(Builder* builder) const final {
        return {.kind = Kind::MemberCall,
                .operands = builder->typeOperandList(
                    {{.type = builder->qualifiedType(builder->typeString(), true)},
                     {.type = builder->qualifiedType(builder->typeMember("encode"), true)},
                     {.type =
                          builder->qualifiedType(builder->typeOperandList(
                                                     {{.id = "charset",
                                                       .type =
                                                           builder->qualifiedType(builder->typeMember("hilti::Charset"),
                                                                                  true),
                                                       .default_ = builder->expressionByName("hilti::Charset::UTF8")}}),
                                                 true)}}),
                .result = builder->qualifiedType(builder->typeBytes(), false),
                .ns = "string",
                .doc = "Compares two strings lexicographically."};
    }

    Result<ResolvedOperatorPtr> instantiate(Builder* builder, node::Range<Expression> operands,
                                            const Meta& meta) const final {
        return {operator_::string::Encode::create(builder->context(), this, result(builder, operands, meta), operands,
                                                  meta)};
    }
};

REGISTER_OPERATOR(Encode);
} // namespace
