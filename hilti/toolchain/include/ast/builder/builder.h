// Copyright (c) 2021-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/builder/node-factory.h>

namespace hilti {

class Builder : public builder::NodeFactory {
public:
    Builder(ASTContext* ctx) : NodeFactory(ctx) {}
};

using BuilderPtr = std::shared_ptr<Builder>;

} // namespace hilti
