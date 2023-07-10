// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/statements/try.h>

using namespace hilti;

statement::try_::Catch::~Catch() = default;

std::string statement::try_::Catch::_render() const { return ""; }
