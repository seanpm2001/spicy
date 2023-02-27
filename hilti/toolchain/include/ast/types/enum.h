// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/constant.h>
#include <hilti/ast/id.h>
#include <hilti/ast/node.h>
#include <hilti/ast/type.h>

namespace hilti::type {

class Enum;

namespace enum_ {

/** AST node for an enum label. */
class Label final : public Node {
public:
    ~Label() final;

    const ID& id() const { return _id; }
    auto value() const { return _value; }
    auto enumType() const { return _enum_type; }

    node::Properties properties() const final {
        auto p = node::Properties{{"id", _id}, {"value", _value}};
        return Node::properties() + p;
    }

    static auto create(ASTContext* ctx, const ID& id, int value, const Meta& meta = {}) {
        return NodeDerivedPtr<Label>(new Label(id, value, {}, meta));
    }

    static auto create(ASTContext* ctx, const ID& id, const Meta& meta = {}) {
        return NodeDerivedPtr<Label>(new Label(id, -1, {}, meta));
    }

protected:
    friend class type::Enum;

    Label(ID id, int value, std::weak_ptr<UnqualifiedType> enum_type, Meta meta = {})
        : Node(std::move(meta)), _id(std::move(id)), _value(value), _enum_type(std::move(enum_type)) {}

    std::string _render() const final;

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Label>();
        if ( ! n )
            return false;

        return Node::isEqual(other) && _id == n->_id && _value == n->_value;
    }

    HILTI_NODE(Label)

    static auto create(ASTContext* ctx, const ID& id, int value, const std::weak_ptr<UnqualifiedType>& enum_type,
                       const Meta& meta = {}) {
        return NodeDerivedPtr<Label>(new Label(id, value, enum_type, meta));
    }

private:
    ID _id;
    int _value = -1;

    std::weak_ptr<UnqualifiedType> _enum_type;
};

using LabelPtr = std::shared_ptr<Label>;
using Labels = std::vector<LabelPtr>;

} // namespace enum_

/** AST node for a `enum` type. */
class Enum : public UnqualifiedType {
public:
    enum_::Labels labels() const;
    auto labelDeclarations() { return children<Declaration>(0, -1); }

    /**
     * Filters a set of labels so that it includes each enumerator value at
     * most once.
     */
    enum_::Labels uniqueLabels() const;

    enum_::LabelPtr label(const ID& id) const {
        for ( const auto& l : labels() ) {
            if ( l->id() == id )
                return l;
        }

        return {};
    }

    Nodes typeParameters() const final {
        Nodes params;
        for ( auto&& c : uniqueLabels() )
            params.emplace_back(c.get());

        return params;
    }

    static auto create(ASTContext* ctx, enum_::Labels labels, Meta meta = {}) {
        return NodeDerivedPtr<Enum>(new Enum(_normalizeLabels(ctx, std::move(labels)), std::move(meta)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Enum>(new Enum(Wildcard(), std::move(m)));
    }

    /** Helper method for the resolver to link labels to their type. */
    static void initLabelTypes(ASTContext* ctx, const NodePtr& n);

protected:
    Enum(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Enum(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isSortable() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final { return _initialized; }

    bool isEqual(const Node& other) const override { return other.isA<Enum>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Enum)

private:
    bool _initialized = false;

    static Declarations _normalizeLabels(ASTContext* ctx, enum_::Labels labels);
};

} // namespace hilti::type
