// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/expression.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/auto.h>

namespace hilti::expression {

namespace keyword {
// Type of a reserved keyword
enum class Kind {
    Self,         /**< `self` */
    DollarDollar, /**< `$$` */
    Captures,     /**< `$@` */
    Scope         /**< `$scope`*/
};

namespace detail {
constexpr util::enum_::Value<Kind> kinds[] = {{Kind::Self, "self"},
                                              {Kind::DollarDollar, "$$"},
                                              {Kind::Captures, "$@"},
                                              {Kind::Scope, "$scope"}};
} // namespace detail

namespace kind {
constexpr auto from_string(const std::string_view& s) { return util::enum_::from_string<Kind>(s, detail::kinds); }
} // namespace kind

constexpr auto to_string(Kind m) { return util::enum_::to_string(m, detail::kinds); }

} // namespace keyword

/** AST node for an expression representing a reserved keyword. */
class Keyword : public Expression {
public:
    keyword::Kind kind() const { return _kind; }
    void setType(const QualifiedTypePtr& t) { setChild(0, t); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    node::Properties properties() const final {
        auto p = node::Properties{{{"kind", to_string(_kind)}}};
        return Expression::properties() + p;
    }

    static auto create(ASTContext* ctx, keyword::Kind kind, const QualifiedTypePtr& type, const Meta& meta = {}) {
        return NodeDerivedPtr<Keyword>(new Keyword({type}, kind, meta));
    }

    static auto create(ASTContext* ctx, keyword::Kind kind, const Meta& meta = {}) {
        return create(ctx, kind, QualifiedType::createAuto(ctx, meta), meta);
    }

    /** Helper to create `$$` a declaration of a given type. */
    static auto createDollarDollarDeclaration(ASTContext* ctx, const QualifiedTypePtr& type) {
        type::pruneWalk(type);
        auto kw = create(ctx, keyword::Kind::DollarDollar, type);
        return declaration::Expression::create(ctx, "__dd", kw, hilti::declaration::Linkage::Private);
    }

protected:
    Keyword(Nodes children, keyword::Kind kind, Meta meta)
        : Expression(std::move(children), std::move(meta)), _kind(kind) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Keyword>();
        if ( ! n )
            return false;

        return Expression::isEqual(other) && _kind == n->_kind;
    }

    HILTI_NODE(Keyword)

private:
    keyword::Kind _kind;
};

inline std::ostream& operator<<(std::ostream& stream, const Keyword& keyword) {
    switch ( keyword.kind() ) {
        case keyword::Kind::Self: return stream << "<self>";
        case keyword::Kind::DollarDollar: return stream << "<$$>";
        case keyword::Kind::Captures: return stream << "<captures>";
        case keyword::Kind::Scope: return stream << "<scope>";
    }

    return stream;
}

} // namespace hilti::expression
