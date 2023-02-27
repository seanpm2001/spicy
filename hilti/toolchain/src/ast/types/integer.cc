// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <algorithm>
#include <exception>

#include <hilti/ast/ctors/integer.h>
#include <hilti/ast/types/integer.h>

using namespace hilti;
using namespace hilti::type;

NodeDerivedPtr<SignedInteger> type::SignedInteger::create(ASTContext* ctx, int width, const Meta& m) {
    auto t = NodeDerivedPtr<SignedInteger>(new SignedInteger({}, width, nullptr, m));
    auto ctor = ctor::UnsignedInteger::create(ctx, width, 64, t);
    t->setTypeArg(ctor);
    return t;
}

NodeDerivedPtr<UnsignedInteger> type::UnsignedInteger::create(ASTContext* ctx, int width, const Meta& m) {
    auto t = NodeDerivedPtr<UnsignedInteger>(new UnsignedInteger({}, width, nullptr, m));
    auto ctor = ctor::UnsignedInteger::create(ctx, width, 64, t);
    t->setTypeArg(ctor);
    return t;
}
