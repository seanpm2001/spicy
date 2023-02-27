// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <string>
#include <utility>

#include <hilti/ast/forward.h>
#include <hilti/ast/node.h>

namespace hilti {

/** Base class for statement nodes. */
class Statement : public Node {
public:
    ~Statement() override;

protected:
    Statement(Nodes children, Meta meta) : Node::Node(std::move(children), std::move(meta)) {}

    std::string _render() const override;
    bool isEqual(const Node& other) const override { return other.isA<Statement>() && Node::isEqual(other); }

    HILTI_NODE_BASE(Statement);
};

} // namespace hilti
