// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/ctors/struct.h>

using namespace hilti;

ctor::struct_::Field::~Field() = default;

std::string ctor::struct_::Field::_render() const { return util::fmt("[id: %s]", id()); }
