// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/forward.h>
#include <hilti/ast/type.h>
#include <hilti/base/util.h>

namespace hilti::type {

namespace tuple {

/** Base class for tuple element nodes. */
class Element final : public Node {
public:
    ~Element() final;

    const auto& id() const { return _id; }
    auto type() const { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{"id", _id}};
        return Node::properties() + p;
    }

    static auto create(ASTContext* ctx, ID id, const QualifiedTypePtr& type, Meta meta = {}) {
        return NodeDerivedPtr<Element>(new Element({type}, std::move(id), std::move(meta)));
    }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& type, Meta meta = {}) {
        return NodeDerivedPtr<Element>(new Element({type}, ID(), std::move(meta)));
    }

protected:
    Element(Nodes children, ID id, Meta meta = {}) : Node(std::move(children), std::move(meta)), _id(id) {}

    std::string _render() const final;

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Element>();
        if ( ! n )
            return false;

        return Node::isEqual(other) && _id == n->_id;
    }

    HILTI_NODE(Element);

private:
    std::optional<ID> _id;
};

using ElementPtr = std::shared_ptr<Element>;
using Elements = std::vector<std::shared_ptr<Element>>;

} // namespace tuple

/** AST node for a tuple type. */
class Tuple : public UnqualifiedType {
public:
    auto elements() const { return children<tuple::Element>(0, -1); }
    std::optional<std::pair<int, type::tuple::ElementPtr>> elementByID(const ID& id) const;

    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, type::tuple::Elements elements, Meta meta = {}) {
        return NodeDerivedPtr<Tuple>(new Tuple(std::move(elements), std::move(meta)));
    }

    static auto create(ASTContext* ctx, const QualifiedTypes& types, const Meta& meta = {}) {
        auto elements = util::transform(types, [&](const auto& t) { return tuple::Element::create(ctx, t, meta); });
        return NodeDerivedPtr<Tuple>(new Tuple(std::move(elements), meta));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Tuple>(new Tuple(Wildcard(), std::move(m)));
    }

protected:
    Tuple(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Tuple(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isSortable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final {
        const auto& cs = children();

        return std::all_of(cs.begin(), cs.end(), [&](const auto& c) {
            auto t = c->template tryAs<QualifiedType>();
            return ! t || type::detail::isResolved(*t, rstate);
        });
    }

    bool isEqual(const Node& other) const override { return other.isA<Tuple>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Tuple)
};

} // namespace hilti::type
