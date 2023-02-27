// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <string>
#include <utility>

#include <hilti/ast/forward.h>
#include <hilti/ast/node.h>

namespace hilti {

/** Base class for classes implementing constructor nodes. */
class Ctor : public Node {
public:
    ~Ctor() override;

    /** Returns the HILTI type of the constructor's value. */
    virtual QualifiedTypePtr type() const = 0;

    /** Returns true if the ctor can be the target of an assignment. */
    virtual bool isLhs() const { return false; }

    /**
     * Returns true if, when evaluated as RHS, the ctor will yield a
     * temporary value.
     */
    virtual bool isTemporary() const { return true; }

protected:
    Ctor(Nodes children, Meta meta) : Node::Node(std::move(children), std::move(meta)) {}

    std::string _render() const override;
    bool isEqual(const Node& other) const override { return other.isA<Ctor>() && Node::isEqual(other); }

    HILTI_NODE_BASE(Ctor);
};

} // namespace hilti
