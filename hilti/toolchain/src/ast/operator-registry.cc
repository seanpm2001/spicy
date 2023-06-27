// Copyright (c) 2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/builder/builder.h>
#include <hilti/ast/detail/operator-registry.h>

using namespace hilti;
using namespace hilti::operator_;

namespace hilti::logging::debug {
inline const DebugStream OperatorRegistry("operator-registry");
} // namespace hilti::logging::debug

void Registry::register_(std::unique_ptr<Operator> op) { _operators.push_back(std::move(op)); }

void Registry::init(ASTContext* ctx) {
    Builder builder(ctx);

    for ( auto& op : _operators ) {
        op->init(&builder, ctx->root());
        _operators_by_kind[op->kind()].push_back(op.get());

        if ( op->kind() == Kind::MemberCall ) {
            auto id = op->signature().operands->op1().type->type()->as<type::Member>()->id();
            _operators_by_method[id].push_back(op.get());
        }

        int status;
        const auto& op_ = *op;
        std::string n = abi::__cxa_demangle(typeid(op_).name(), nullptr, nullptr, &status);
        n = util::replace(n, "::(anonymous namespace)", "");
        HILTI_DEBUG(hilti::logging::debug::OperatorRegistry,
                    hilti::util::fmt("initialized operator %s::%s for '%s'", op->signature().ns, n,
                                     to_string(op->kind())));
    }
}
