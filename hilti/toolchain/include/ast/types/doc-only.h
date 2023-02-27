// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/type.h>

namespace hilti::type {

/**
 * AST node for a type that's only used for documentation purposes. This type
 * allows to carry a textual description of a type over into auto-generated
 * documentation. If it's used anywhere else, it'll cause trouble.
 */
class DocOnly : public UnqualifiedType {
public:
    auto description() const { return _description; }

    static auto create(ASTContext* ctx, std::string description, Meta meta = {}) {
        // Note: We allow (i.e., must support) `ctx` being null.
        return NodeDerivedPtr<DocOnly>(new DocOnly(std::move(description), std::move(meta)));
    }

protected:
    DocOnly(std::string description, Meta meta)
        : UnqualifiedType(std::move(meta)), _description(std::move(description)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<DocOnly>();
        if ( ! n )
            return false;

        return UnqualifiedType::isEqual(other) && _description == n->_description;
    }

    HILTI_NODE(DocOnly)

private:
    std::string _description;
};

} // namespace hilti::type
