// Copyright (c) 2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/builder/builder.h>
#include <hilti/ast/operator.h>

using namespace hilti;
using namespace hilti::util;
using namespace hilti::operator_;

static std::string printOperator(operator_::Kind kind, const std::vector<std::string>& ops) {
    switch ( kind ) {
        case operator_::Kind::Add: return fmt("add %s[%s]", ops[0], ops[1]);
        case operator_::Kind::Begin: return fmt("begin(%s)", ops[0]);
        case operator_::Kind::BitAnd: return fmt("%s & %s", ops[0], ops[1]);
        case operator_::Kind::BitOr: return fmt("%s | %s", ops[0], ops[1]);
        case operator_::Kind::BitXor: return fmt("%s ^ %s", ops[0], ops[1]);
        case operator_::Kind::Call: return fmt("%s(%s)", ops[0], ops[1]);
        case operator_::Kind::Cast: return fmt("cast<%s>(%s)", ops[1], ops[0]);
        case operator_::Kind::CustomAssign: return fmt("%s = %s", ops[0], ops[1]);
        case operator_::Kind::DecrPostfix: return fmt("%s--", ops[0]);
        case operator_::Kind::DecrPrefix: return fmt("--%s", ops[0]);
        case operator_::Kind::Delete: return fmt("delete %s[%s]", ops[0], ops[1]);
        case operator_::Kind::Deref: return fmt("(*%s)", ops[0]);
        case operator_::Kind::Difference: return fmt("%s - %s", ops[0], ops[1]);
        case operator_::Kind::DifferenceAssign: return fmt("%s -= %s", ops[0], ops[1]);
        case operator_::Kind::Division: return fmt("%s / %s", ops[0], ops[1]);
        case operator_::Kind::DivisionAssign: return fmt("%s /= %s", ops[0], ops[1]);
        case operator_::Kind::Equal: return fmt("%s == %s", ops[0], ops[1]);
        case operator_::Kind::End: return fmt("end(%s)", ops[0]);
        case operator_::Kind::Greater: return fmt("%s > %s", ops[0], ops[1]);
        case operator_::Kind::GreaterEqual: return fmt("%s >= %s", ops[0], ops[1]);
        case operator_::Kind::HasMember: return fmt("%s?.%s", ops[0], ops[1]);
        case operator_::Kind::In: return fmt("%s in %s", ops[0], ops[1]);
        case operator_::Kind::IncrPostfix: return fmt("%s++", ops[0]);
        case operator_::Kind::IncrPrefix: return fmt("++%s", ops[0]);
        case operator_::Kind::Index: return fmt("%s[%s]", ops[0], ops[1]);
        case operator_::Kind::IndexAssign: return fmt("%s[%s] = %s", ops[0], ops[1], ops[2]);
        case operator_::Kind::Lower: return fmt("%s < %s", ops[0], ops[1]);
        case operator_::Kind::LowerEqual: return fmt("%s <= %s", ops[0], ops[1]);
        case operator_::Kind::Member: return fmt("%s.%s", ops[0], ops[1]);
        case operator_::Kind::MemberCall: return fmt("%s.%s(%s)", ops[0], ops[1], ops[2]);
        case operator_::Kind::Modulo: return fmt("%s %% %s", ops[0], ops[1]);
        case operator_::Kind::Multiple: return fmt("%s * %s", ops[0], ops[1]);
        case operator_::Kind::MultipleAssign: return fmt("%s *= %s", ops[0], ops[1]);
        case operator_::Kind::Negate: return fmt("~%s", ops[0]);
        case operator_::Kind::New: return fmt("new %s%s", ops[0], ops[1]);
        case operator_::Kind::Pack: return fmt("pack%s", ops[0]);
        case operator_::Kind::Power: return fmt("%s ** %s", ops[0], ops[1]);
        case operator_::Kind::ShiftLeft: return fmt("%s << %s", ops[0], ops[1]);
        case operator_::Kind::ShiftRight: return fmt("%s >> %s", ops[0], ops[1]);
        case operator_::Kind::SignNeg: return fmt("-%s", ops[0]);
        case operator_::Kind::SignPos: return fmt("+%s", ops[0]);
        case operator_::Kind::Size: return fmt("|%s|", ops[0]);
        case operator_::Kind::Sum: return fmt("%s + %s", ops[0], ops[1]);
        case operator_::Kind::SumAssign: return fmt("%s += %s", ops[0], ops[1]);
        case operator_::Kind::TryMember: return fmt("%s.?%s", ops[0], ops[1]);
        case operator_::Kind::Unequal: return fmt("%s != %s", ops[0], ops[1]);
        case operator_::Kind::Unpack: return fmt("unpack<%s>(%s)", ops[0], ops[1]);
        case operator_::Kind::Unknown: logger().internalError("\"unknown\" operator");
        case operator_::Kind::Unset: return fmt("unset %s", ops[0]);
    }

    util::cannot_be_reached();
}

std::string operator_::detail::print(Kind kind, const Expressions& operands) {
    switch ( kind ) {
        case operator_::Kind::MemberCall: {
            assert(operands.size() == 3);
            const auto& args = operands[2]->type()->type()->as<type::Tuple>()->elements();
            auto args_ = util::join(util::transform(args, [](const auto& x) { return x->type()->print(); }), ", ");
            return printOperator(kind, {operands[0]->print(), operands[1]->print(), args_});
        }

        case operator_::Kind::Call: {
            assert(operands.size() == 2);
            const auto& args = operands[1]->type()->type()->as<type::Tuple>()->elements();
            auto args_ = util::join(util::transform(args, [](const auto& x) { return x->type()->print(); }), ", ");
            return printOperator(kind, {operands[0]->print(), args_});
            break;
        }

        default:
            auto ops = util::transform(operands, [](const auto& x) { return x->print(); });
            return printOperator(kind, ops);
    }
}

void Operator::init(Builder* builder, const NodePtr& scope_root) {
    _signature = signature(builder);

    auto* context = builder->context();

    _signature->result->unify(context, scope_root);
    for ( auto& op : _signature->operands->operands() )
        op.type->unify(context, scope_root);
}

std::string Operator::print() const {
    return printOperator(kind(), util::transform(operands(), [](const auto& x) { return fmt("<%s>", x.print()); }));
}

std::string MethodCall::print() const {
    assert(operands().size() == 3);
    const auto& ops = op2().type->type()->as<type::OperandList>()->operands();
    auto args = util::join(util::transform(ops, [](const auto& x) { return x.print(); }), ", ");
    return printOperator(kind(), {fmt("<%s>", op0().type), op1().type->print(), args});
}

void FunctionCall::setDeclaration(declaration::Function* fdecl) { _fdecl = fdecl; }

operator_::Signature FunctionCall::signature(Builder* builder) const {
    assert(_fdecl);

    auto op0 = operator_::Operand{{}, builder->qualifiedType(builder->typeAny(), true)}; // IDs won't be resolved
    auto op1 = operator_::Operand{{},
                                  type::OperandList::fromParameters(builder->context(),
                                                                    _fdecl->function()->ftype()->parameters())};

    return {.kind = Kind::Call,
            .operands = builder->typeOperandList({op0, op1}),
            .result = _fdecl->function()->ftype()->result()};
}

Result<ResolvedOperatorPtr> FunctionCall::instantiate(Builder* builder, node::Range<hilti::Expression> operands,
                                                      const Meta& meta) const {
    assert(_fdecl);

    auto callee = builder->expressionName(_fdecl->id(), meta);
    auto args = operands[1];

    callee->setDeclaration(_fdecl->as<Declaration>());

    return {operator_::function::Call::create(builder->context(), this, signature(builder).result,
                                              {std::move(callee), std::move(args)}, meta)};
}

std::string FunctionCall::print() const {
    assert(operands().size() == 2);

    ID id;

    if ( _fdecl ) {
        id = _fdecl->fullyQualifiedID();
        if ( ! id )
            id = _fdecl->id();
    }
    else
        id = ID(op1().print());

    const auto& ops = op1().type->type()->as<type::OperandList>()->operands();
    auto args = util::join(util::transform(ops, [&](const auto& x) { return x.print(); }), ", ");
    return printOperator(kind(), {id, args});
}
