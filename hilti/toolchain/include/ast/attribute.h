// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/forward.h>
#include <hilti/ast/node.h>

namespace hilti {

/** AST node for attributes. */
class Attribute : public Node {
public:
    ~Attribute() override;

    /** Returns the name of the attribute, including the leading `&`. */
    const auto& tag() const { return _tag; }

    /** Returns true if an argument is associated with the attribute. */
    bool hasValue() const { return child(0) != nullptr; }

    /**
     * Returns the attribute associated with the node.
     *
     * @exception `std::out_of_range` if the attribute does not have an argument
     */
    NodePtr value() const { return child(0); }

    /**
     * Returns the attributes argument as type `T`. `T` must be either an
     * `Expression`, or `std::string`. In the former case, the value must be
     * an AST expression node. In the latter case, the argument must be a
     * string constructor expression, and the returned value will be the
     * string it represents.
     *
     * @tparam T either `Expression` or `std::string`
     * @return the argument, or an error if the argument could not be interpreted as type `T`
     * @exception `std::out_of_range` if the attribute does not have an argument
     */
    Result<ExpressionPtr> valueAsExpression() const;

    Result<std::string> valueAsString() const;

    Result<int64_t> valueAsInteger() const;

    /**
     * Coerce the attribute's expression value to a specified type, modifying
     * the node in place.
     *
     * @return A successful return value if either the coercion succeeded
     * (then the result's value is true), or nothing was to be done (then the
     * result's value is false); a failure if a coercion would have been
     * necessary, but failed, or the attribute does not have a expression value.
     */
    Result<bool> coerceValueTo(const UnqualifiedTypePtr& dst);

    node::Properties properties() const override {
        auto p = node::Properties{{"tag", _tag}};
        return Node::properties() + p;
    }

    /**
     * Factory for an attribute coming with an argument. The argument
     * must be either an AST node representing an expression.
     *
     * @param tag name of the attribute, including the leading `&`
     * @param v node representing the argument to associate with the attribute; must be an expression
     * @param m meta data to associate with the node
     */
    static auto create(ASTContext* ctx, std::string tag, const NodePtr& v, Meta m = Meta()) {
        return NodeDerivedPtr<Attribute>(new Attribute(Nodes{v}, std::move(tag), std::move(m)));
    }

    /**
     * Factory for an attribute with no argument.
     *
     * @param tag name of the attribute, including the leading `&`
     * @param m meta data to associate with the node
     */
    static auto create(ASTContext* ctx, std::string tag, Meta m = Meta()) {
        return create(ctx, std::move(tag), nullptr, std::move(m));
    }

protected:
    Attribute(Nodes children, std::string tag, Meta m = Meta())
        : Node(std::move(children), std::move(m)), _tag(std::move(tag)) {}

    std::string _render() const override;

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Attribute>();
        if ( ! n )
            return false;

        return Node::isEqual(other) && _tag == n->_tag;
    }

    HILTI_NODE(Attribute);

private:
    std::string _tag;
};

/** AST node holding a set of `Attribute` nodes. */
class AttributeSet : public Node {
public:
    ~AttributeSet() override;

    /** Returns the set's attributes. */
    auto attributes() const { return children<Attribute>(0, -1); }

    /**
     * Retrieves an attribute with a given name from the set. If multiple
     * attributes with that tag exist, it's undefined which is returned.
     *
     * @return attribute if found
     */
    AttributePtr find(std::string_view tag) const;

    /**
     * Retrieves all attributes with a given name from the set.
     *
     * @return all attributes with matching name
     */
    hilti::node::Set<Attribute> findAll(std::string_view tag) const;

    /**
     * If an attribute of a given name exists and has an expression value,
     * the value is coerced to a specified type. If not, nothing is done.
     *
     * @return A successful return value if either the coercion succeeded
     * (then the result's value is true), or nothing was to be done (then the
     * result's value is false); a failures if a coercion would have been
     * necessary, but failed.
     */
    Result<bool> coerceValueTo(const std::string& tag, const UnqualifiedTypePtr& dst);

    /**
     * Returns true if there's an attribute with a given name in the set.
     *
     * @param true if found
     */
    bool has(std::string_view tag) const { return find(tag) != nullptr; }

    /** Adds an attribute to the set. */
    void add(const AttributePtr& a) { addChild(a); }

    /** Returns true if the set has at least one element. */
    operator bool() const { return ! children().empty(); }

    static auto create(ASTContext* ctx, Attributes attrs, Meta m = Meta()) {
        return NodeDerivedPtr<AttributeSet>(new AttributeSet(Nodes{std::move(attrs)}, std::move(m)));
    }

#if 0
        // TODO: Do we want/need these static methods?
        //
    /**
     * Returns a new attribute set that adds one element.
     *
     * @param s set to add to.
     * @param a element to add.
     * @return `s` with `a' added
     */
    static AttributeSet add(AttributeSet s, Attribute a) {
        s.addChild(std::move(a));
        return s;
    }

    /**
     * Returns a new attribute set that adds one element.
     *
     * @param s set to add to.
     * @param a element to add.
     * @return `s` with `a' added
     */
    static AttributeSet add(std::optional<AttributeSet> s, Attribute a) {
        if ( ! s )
            s = AttributeSet({}, a.meta());

        s->addChild(std::move(a));
        return *s;
    }

    /**
     * Retrieves an attribute with a given name from a set, dealing correctly
     * with an unset optional set. If multiple attributes with that tag
     * exist, it's undefined which is returned.
     *
     * @param attrs set to inspect
     * @return attribute if found
     */
    static hilti::optional_ref<const Attribute> find(const hilti::optional_ref<const AttributeSet>& attrs,
                                                     std::string_view tag) {
        if ( attrs )
            return attrs->find(tag);
        else
            return {};
    }

    /**
     * Retrieves all attribute with a given name from a set, dealing correctly
     * with an unset optional set.
     *
     * @param attrs set to inspect
     * @return all attributes with matching name
     */
    static hilti::node::Set<Attribute> findAll(const std::optional<const AttributeSet>& attrs, std::string_view tag) {
        if ( attrs )
            return attrs->findAll(tag);
        else
            return {};
    }

    /**
     * Retrieves all attribute with a given name from a set, dealing correctly
     * with an unset optional set.
     *
     * @param attrs set to inspect
     * @return all attributes with matching name
     */
    static hilti::node::Set<Attribute> findAll(const hilti::optional_ref<const AttributeSet>& attrs,
                                               std::string_view tag) {
        if ( attrs )
            return attrs->findAll(tag);
        else
            return {};
    }

    /**
     * Returns true if there's an attribute with a given name in a set,
     * dealing correctly with an unset optional set.
     *
     * @param attrs set to inspect
     * @param true if found
     */
    static bool has(const std::optional<AttributeSet>& attrs, std::string_view tag) {
        if ( attrs )
            return attrs->has(tag);
        else
            return false;
    }
#endif

protected:
    /**
     * Constructs a set from from a vector of attributes.
     *
     * @param a vector to initialize attribute set from
     * @param m meta data to associate with the node
     */
    explicit AttributeSet(Nodes children, Meta m = Meta()) : Node(std::move(children), std::move(m)) {}

    /**
     * Constructs an empty set.
     *
     * @param m meta data to associate with the node
     */
    AttributeSet(Meta m = Meta()) : Node({}, std::move(m)) {}

    std::string _render() const override;
    bool isEqual(const Node& other) const override { return other.isA<AttributeSet>() && Node::isEqual(other); }

    HILTI_NODE(AttributeSet);
};

} // namespace hilti
