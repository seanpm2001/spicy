// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/declarations/function.h>
#include <hilti/ast/expressions/name.h>
#include <hilti/ast/operators/function.h>
#include <hilti/ast/types/operand-list.h>
#include <hilti/ast/types/struct.h>

#include "ast/builder/builder.h"
#include "ast/node.h"

using namespace hilti;
using namespace hilti::operator_;

void declaration::function::Operator::setDeclaration(declaration::Function* fdecl) { _fdecl = fdecl; }

operator_::Signature declaration::function::Operator::Operator::signature(Builder* builder) const {
    assert(_fdecl);

    auto op0 = operator_::Operand{{}, builder->qualifiedType(builder->typeAny(), true)}; // IDs won't be resolved
    auto op1 = operator_::Operand{{},
                                  type::OperandList::fromParameters(builder->context(),
                                                                    _fdecl->function()->ftype()->parameters())};

    return {.kind = Kind::Call,
            .operands = builder->typeOperandList({op0, op1}),
            .result = _fdecl->function()->ftype()->result()};
}

Result<ResolvedOperatorPtr> declaration::function::Operator::instantiate(
    Builder* builder, const expression::UnresolvedOperator* op) const {
    assert(_fdecl);

    auto callee = builder->expressionName(_fdecl->id(), op->meta());
    auto args = op->operands()[1];

    callee->setDeclaration(_fdecl->as<Declaration>());

    return {operator_::function::Call::create(builder->context(), this, signature(builder).result,
                                              {std::move(callee), std::move(args)}, op->meta())};
}
