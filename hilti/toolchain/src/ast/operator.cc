// Copyright (c) 2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/builder/builder.h>
#include <hilti/ast/operator.h>

using namespace hilti;
using namespace hilti::operator_;

void Operator::init(Builder* builder, const NodePtr& scope_root) {
    _signature = signature(builder);

    auto* context = builder->context();

    _signature->result->unify(context, scope_root);
    for ( auto& op : _signature->operands->operands() )
        op.type->unify(context, scope_root);
}
