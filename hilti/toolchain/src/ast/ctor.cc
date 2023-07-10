// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/ctor.h>
#include <hilti/ast/visitor.h>

using namespace hilti;

Ctor::~Ctor() = default;

std::string Ctor::_render() const { return ""; }
