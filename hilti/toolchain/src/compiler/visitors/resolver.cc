// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <functional>
#include <optional>
#include <sstream>
#include <utility>

#include <hilti/ast/ast-context.h>
#include <hilti/ast/ctors/reference.h>
#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/imported-module.h>
#include <hilti/ast/declarations/local-variable.h>
#include <hilti/ast/declarations/parameter.h>
#include <hilti/ast/detail/operator-registry.h>
#include <hilti/ast/expressions/deferred.h>
#include <hilti/ast/expressions/keyword.h>
#include <hilti/ast/expressions/list-comprehension.h>
#include <hilti/ast/expressions/name.h>
#include <hilti/ast/expressions/type.h>
#include <hilti/ast/expressions/typeinfo.h>
#include <hilti/ast/node.h>
#include <hilti/ast/operator.h>
// #include <hilti/ast/operators/struct.h>
#include <hilti/ast/scope-lookup.h>
#include <hilti/ast/scope.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/function.h>
#include <hilti/ast/types/reference.h>
#include <hilti/ast/types/unknown.h>
#include <hilti/ast/types/void.h>
#include <hilti/base/util.h>
#include <hilti/compiler/context.h>
#include <hilti/compiler/detail/visitors.h>
#include <hilti/compiler/unit.h>
#include <hilti/global.h>

#include "ast/builder/builder.h"
#include "base/timing.h"
#include "compiler/coercion.h"

using namespace hilti;

namespace hilti::logging::debug {
inline const hilti::logging::DebugStream Resolver("resolver");
inline const hilti::logging::DebugStream Operator("operator");
} // namespace hilti::logging::debug

namespace {

struct Resolver : visitor::PostOrder {
    explicit Resolver(Builder* builder, const ASTRootPtr& root) : root(root), builder(builder) {}

    const ASTRootPtr& root;
    Builder* builder;

    bool modified = false;

#if 0
    std::map<ID, QualifiedType> auto_params; // mapping of `auto` parameters inferred, indexed by canonical ID
    std::set<const Node*> seen;

    void preDispatch(const Node& n, int level) override {
        std::string prefix = "# ";

        if ( seen.find(&n) != seen.end() )
            prefix = "! ";
        else
            seen.insert(&n);

        auto indent = std::string(level * 2, ' ');
        std::cerr << prefix << indent << "> " << n.render() << std::endl;
        n.scope()->render(std::cerr, "    | ");
    };
#endif

    // Log debug message recording resolving a epxxression.
    void logChange(const Node* old, const DeclarationPtr& ndecl) {
        HILTI_DEBUG(logging::debug::Resolver,
                    util::fmt("%s \"%s\" -> declaration %s (%s)", ID(ID(old->typename_()).local()).local(), *old,
                              ndecl->canonicalID(), old->location()));
    }

    void logChange(const Node* old, const ExpressionPtr& nexpr) {
        HILTI_DEBUG(logging::debug::Resolver, util::fmt("%s %s -> expression %s (%s)", ID(old->typename_()).local(),
                                                        *old, *nexpr, old->location()));
    }

    // Log debug message recording resolving a statement.
    void logChange(const Node* old, const StatementPtr& nstmt) {
        HILTI_DEBUG(logging::debug::Resolver, util::fmt("%s %s -> statement %s (%s)", ID(old->typename_()).local(),
                                                        *old, *nstmt, old->location()));
    }

    // Log debug message recording resolving a type.
    void logChange(const Node* old, const QualifiedTypePtr& ntype) {
        HILTI_DEBUG(logging::debug::Resolver,
                    util::fmt("%s %s -> type %s (%s)", ID(old->typename_()).local(), *old, *ntype, old->location()));
    }

    void logChange(const Node* old, const std::string& msg) {
        HILTI_DEBUG(logging::debug::Resolver,
                    util::fmt("%s %s -> %s (%s)", ID(old->typename_()).local(), *old, msg, old->location()));
    }

    void operator()(expression::Name* u) final {
        if ( u->declaration() )
            return;

        auto resolved = scope::lookupID<Declaration>(u->id(), u, "declaration");
        if ( ! resolved ) {
            if ( u->id() == ID("__dd") )
                // Provide better error message
                u->addError("$$ is not available in this context", node::ErrorPriority::High);
            else
                u->addError(resolved.error(), node::ErrorPriority::High);

            return;
        }

        logChange(u, resolved->first);
        u->setDeclaration(resolved->first);
        modified = true;
    }

    // Helpers for operator resolving
    static std::pair<bool, std::optional<std::vector<const Operator*>>> getFunctionCallCandidates(
        expression::UnresolvedOperator* u) {
        assert(u->operands().size() > 0);

        std::vector<const Operator*> candidates;

        auto callee = u->op0()->tryAs<expression::Name>();
        if ( ! callee )
            return std::make_pair(true, std::move(candidates));

        for ( const Node* n = u; n; n = n->parent() ) {
            if ( ! n->scope() )
                continue;

            for ( const auto& r : n->scope()->lookupAll(callee->id()) ) {
                auto d = r.node->tryAs<declaration::Function>();
                if ( ! d ) {
                    u->addError(util::fmt("ID '%s' resolves to something other than just functions", callee->id()));
                    return std::make_pair(false, std::nullopt);
                }

                if ( r.external && d->linkage() != declaration::Linkage::Public ) {
                    u->addError(util::fmt("function has not been declared public: %s", r.qualified));
                    return std::make_pair(false, std::nullopt);
                }

                candidates.emplace_back(d->operator_());
            }
        }

        return std::make_pair(true, std::move(candidates));
    }

    // If an expression is a reference, dereference it; otherwise return the
    // expression itself.
    ExpressionPtr derefOperand(const ExpressionPtr& op) {
        if ( ! type::isReferenceType(op->type()) )
            return op;

#if 0
        //TODO
        if ( op->type()->isA<type::ValueReference>() )
            return operator_::value_reference::Deref::create({op}, op->meta());
        else if ( op->type()->isA<type::StrongReference>() )
            return operator_::strong_reference::Deref::create({op}, op->meta());
        else if ( op->type()->isA<type::WeakReference>() )
            return operator_::weak_reference::Deref::create({op}, op->meta());
        else
#endif
        logger().internalError("unknown reference type");
    }

    std::vector<ResolvedOperatorPtr> matchOperators(expression::UnresolvedOperator* u,
                                                    const std::vector<const Operator*> candidates,
                                                    bool disallow_type_changes = false) {
        const std::array<bitmask<CoercionStyle>, 4> styles = {
            CoercionStyle::OperandMatching | CoercionStyle::TryExactMatch,
            CoercionStyle::OperandMatching | CoercionStyle::TryExactMatch | CoercionStyle::TryCoercion,
            CoercionStyle::OperandMatching | CoercionStyle::TryExactMatch | CoercionStyle::TryConstPromotion,
            CoercionStyle::OperandMatching | CoercionStyle::TryExactMatch | CoercionStyle::TryConstPromotion |
                CoercionStyle::TryCoercion,
        };

        auto deref_operands = [&](const node::Range<Expression>& ops) {
            return node::transform(ops, [this](const auto& op) { return derefOperand(op); });
        };

        auto try_candidate = [&](const Operator* candidate, const node::Range<Expression>& operands, auto style,
                                 const Meta& meta, const auto& dbg_msg) -> ResolvedOperatorPtr {

#if 0
        // TODO
            auto noperands = coerceOperands(operands, candidate.operands(), style);
            if ( ! noperands ) {
                if ( ! (style & CoercionStyle::DisallowTypeChanges) ) {
                    // If any of the operands is a reference type, try the derefed operands, too.
                    for ( const auto& op : operands ) {
                        if ( type::isReferenceType(op.type()) )
                            noperands = coerceOperands(node::Range<Expression>(deref_operands(operands)),
                                                       candidate.operands(), style);
                    }
                }
            }

            if ( ! noperands )
                return {};
#else
            // TODO: This is just a dummy version.
            if ( candidate->operands().size() != operands.size() )
                return {};

            for ( auto i = 0U; i < operands.size(); ++i ) {
                if ( ! type::isResolved(operands[i]->type()) )
                    return {};

                if ( ! type::sameType(operands[i]->type(), candidate->operands()[i].type) )
                    return {};
            }

            auto noperands = operands;
#endif

            auto r = candidate->instantiate(builder, noperands, meta);
            if ( ! r ) {
                u->addError(r.error());
                return {};
            }

            assert(*r);

#if 0
        // TODO
            // Fold any constants right here in case downstream resolving depends
            // on finding a constant (like for coercion).
            if ( auto ctor = detail::foldConstant(r); ctor && *ctor )
                r = expression::Ctor(**ctor, r.meta());
#endif

            // Some operators may not be able to determine their type before the
            // resolver had a chance to provide the information needed. They will
            // return "auto" in that case (specifically, that's the case for Spicy
            // unit member access). Note we can't check if type::isResolved() here
            // because operators may legitimately return other unresolved types
            // (e.g., IDs that still need to be looked up).
            if ( (*r)->type()->isAuto() )
                return {};

            HILTI_DEBUG(logging::debug::Operator, util::fmt("-> %s, resolves to %s", dbg_msg, **r))
            return *r;
        };

        auto try_all_candidates = [&](std::vector<ResolvedOperatorPtr>* resolved,
                                      std::set<operator_::Kind>* kinds_resolved, operator_::Priority priority) {
            for ( auto style : styles ) {
                if ( disallow_type_changes )
                    style |= CoercionStyle::DisallowTypeChanges;

                HILTI_DEBUG(logging::debug::Operator, util::fmt("style: %s", to_string(style)));
                logging::DebugPushIndent _(logging::debug::Operator);

                for ( const auto& c : candidates ) {
                    if ( priority != c->signature().priority )
                        // Not looking at operators of this priority right now.
                        continue;

                    if ( priority == operator_::Priority::Low && kinds_resolved->count(c->kind()) )
                        // Already have a higher priority match for this operator kind.
                        continue;

                    HILTI_DEBUG(logging::debug::Operator, util::fmt("candidate: %s", c->print()));
                    logging::DebugPushIndent _(logging::debug::Operator);

                    if ( auto r = try_candidate(c, u->operands(), style, u->meta(), "candidate matches") ) {
                        kinds_resolved->insert(c->kind());
                        resolved->push_back(std::move(r));
                    }
                    else {
                        auto operands = u->operands();
                        // Try to swap the operators for commutative operators.
                        if ( operator_::isCommutative(c->kind()) && operands.size() == 2 ) {
                            if ( auto r = try_candidate(c, node::Range<Expression>({operands[1], operands[0]}), style,
                                                        u->meta(), "candidate matches with operands swapped") ) {
                                kinds_resolved->insert(c->kind());
                                resolved->emplace_back(std::move(r));
                            }
                        }
                    }
                }

                if ( resolved->size() )
                    return;
            }
        };

        HILTI_DEBUG(logging::debug::Operator, util::fmt("trying to resolve: %s (%s)", u->print(), u->location()));
        logging::DebugPushIndent _(logging::debug::Operator);

        std::set<operator_::Kind> kinds_resolved;
        std::vector<ResolvedOperatorPtr> resolved;

        try_all_candidates(&resolved, &kinds_resolved, operator_::Priority::Normal);
        if ( resolved.size() )
            return resolved;

        try_all_candidates(&resolved, &kinds_resolved, operator_::Priority::Low);
        return resolved;
    }

    void operator()(expression::UnresolvedOperator* u) final {
        std::vector<const Operator*> candidates;

        if ( u->kind() == operator_::Kind::Call ) {
            if ( ! expression::isResolved(*u->op1()) )
                return;

            auto [valid, functions] = getFunctionCallCandidates(u);
            if ( ! valid )
                return;

            candidates = *functions;
        }
        else if ( u->kind() == operator_::Kind::MemberCall )
            candidates = operator_::registry().byMethodID(u->op1()->as<expression::Member>()->id());

        if ( candidates.empty() )
            candidates = operator_::registry().byKind(u->kind());


        auto matches = matchOperators(u, candidates, u->kind() == operator_::Kind::Cast);
        if ( matches.empty() )
            return;

        if ( matches.size() > 1 ) {
            std::vector<std::string> context = {"candidates:"};
            for ( const auto& op : matches )
                context.emplace_back(util::fmt("- %s [%s]", op->print()));

            u->addError(util::fmt("operator usage is ambiguous: %s", u->print()), std::move(context));
            return;
        }

        logChange(u, matches[0]);
        u->parent()->replaceChild(u, std::move(matches[0]));
        modified = true;
    }

    void operator()(type::Name* u) final {
        if ( u->declaration() )
            return;

        if ( auto resolved = scope::lookupID<declaration::Type>(u->id(), u, "type") ) {
            logChange(u, resolved->first);
            u->setDeclaration(resolved->first);
            modified = true;
        }
        else
            u->addError(resolved.error(), node::ErrorPriority::High);

#if 0
        // Note: We accept types here even when they aren't fully resolved yet,
        // so that we can handle dependency cycles.

        const auto& d = resolved->first->as<declaration::Type>();
        auto t = d.type();
        t = addTypeID(std::move(t), resolved->second, d.attributes());

        if ( d.isOnHeap() ) {
            auto replace = false;

            if ( p.parent().tryAs<Declaration>() )
                replace = true;

            if ( p.parent().isA<declaration::LocalVariable>() && ! p.parent(2).isA<statement::Declaration>() )
                replace = false;

            if ( replace )
                t = type::ValueReference(std::move(t), Location("<on-heap-replacement>"));
        }

        logChange(p.node, t);
        p.node = hilti::type::pruneWalk(std::move(t)); // alias to avoid visitor cycles
        modified = true;
#endif
    }

    void operator()(ctor::Tuple* u) final {
        if ( type::isResolved(u->type()) || ! expression::isResolved(u->value()) )
            return;

        auto elems = node::transform(u->value(), [](const auto& e) { return e->type(); });
        auto t = builder->qualifiedType(builder->typeTuple(elems, u->meta()), true);

        logChange(u, t);
        u->setType(t);
        modified = true;
    }

#if 0
    // Attempt to infer a common type from a list of expression.
    hilti::TypePtrPtr typeForExpressions(position_t* p, node::Range<Expression> exprs) {
        hilti::TypePtrPtr t;

        for ( const auto& e : exprs ) {
            if ( ! type::isResolved(e.type()) )
                return {};

            if ( ! t )
                t = e.type();
            else {
                if ( e.type() != *t )
                    return type::unknown; // inconsistent, will need some other way to resolve
            }
        }

        return t;
    }

    // Associate a type ID, and potentially `&cxxname` with a type.
    Type addTypeID(Type t, ID fully_qualified_id, const hilti::optional_ref<const AttributeSet>& attrs) {
        t.setTypeID(std::move(fully_qualified_id));

        if ( attrs ) {
            /*
             * if ( auto a = AttributeSet::find(*attrs, "&alias") )
             *     t.setTypeID(ID(*a->valueAsString()));
             */

            if ( auto a = AttributeSet::find(*attrs, "&cxxname") )
                t.setCxxID(ID(*a->valueAsString()));
        }

        return t;
    }


    void operator()(ctor::List* u) final {
        if ( type::isResolved(u.type()) )
            return;

        if ( auto ntype = typeForExpressions(&p, u.value()) ) {
            logChange(p.node, *ntype);
            p.node.as<ctor::List>().setElementType(*ntype);
            modified = true;
        }
    }

    void operator()(ctor::Map* u) final {
        if ( type::isResolved(u.keyType()) && type::isResolved(u.valueType()) )
            return;

        hilti::TypePtrPtr key;
        hilti::TypePtrPtr value;

        for ( const auto& e : u.value() ) {
            if ( ! (type::isResolved(e.key().type()) && type::isResolved(e.value().type())) )
                return;

            if ( ! key )
                key = e.key().type();
            else if ( e.key().type() != *key ) {
                p.node.addError("inconsistent key types in map");
                return;
            }

            if ( ! value )
                value = e.value().type();
            else if ( e.value().type() != *value ) {
                p.node.addError("inconsistent value types in map");
                return;
            }
        }

        if ( ! (key && value) ) {
            // empty map
            key = type::unknown;
            value = type::unknown;
        }

        logChange(p.node, type::Tuple({*key, *value}));
        p.node.as<ctor::Map>().setElementType(*key, *value);
        modified = true;
    }

    void operator()(ctor::Optional* u) final {
        if ( type::isResolved(u.type()) || ! u.value() || ! type::isResolved(u.value()->type()) )
            return;

        logChange(p.node, u.value()->type());
        p.node.as<ctor::Optional>().setDereferencedType(u.value()->type());
        modified = true;
    }

    void operator()(ctor::Result* u) final {
        if ( type::isResolved(u.type()) || ! u.value() || ! type::isResolved(u.value()->type()) )
            return;

        logChange(p.node, u.value()->type());
        p.node.as<ctor::Result>().setDereferencedType(u.value()->type());
        modified = true;
    }

    void operator()(ctor::Set* u) final {
        if ( type::isResolved(u.type()) )
            return;

        if ( auto ntype = typeForExpressions(&p, u.value()) ) {
            logChange(p.node, *ntype);
            p.node.as<ctor::Set>().setElementType(*ntype);
            modified = true;
        }
    }

    void operator()(ctor::Struct* u) final {
        if ( type::isResolved(u.type()) )
            return;

        std::vector<Declaration> field_types;

        for ( const auto& f : u.fields() ) {
            if ( ! expression::isResolved(f.expression()) )
                return;

            field_types.emplace_back(declaration::Field(f.id(), f.expression().type(), std::nullopt, f.id().meta()));
        }

        Type ntype = type::Struct(type::Struct::AnonymousStruct{}, std::move(field_types), u.meta());
        if ( auto id = u.type().typeID() )
            ntype.setTypeID(*id);

        logChange(p.node, ntype);
        p.node.as<ctor::Struct>().setType(ntype);
        modified = true;
    }

    void operator()(ctor::ValueReference* u) final {
        if ( type::isResolved(u.type()) || ! type::isResolved(u.expression().type()) )
            return;

        logChange(p.node, u.expression().type());
        p.node.as<ctor::ValueReference>().setDereferencedType(u.expression().type());
        modified = true;
    }

    void operator()(ctor::Vector* u) final {
        if ( type::isResolved(u.type()) )
            return;

        if ( auto ntype = typeForExpressions(&p, u.value()) ) {
            logChange(p.node, *ntype);
            p.node.as<ctor::Vector>().setElementType(*ntype);
            modified = true;
        }
    }

    void operator()(declaration::Function* u) final {
        if ( u.linkage() != declaration::Linkage::Struct && u.id().namespace_() ) {
            // See if the namespace refers to a struct. If so, change linkage
            // because that's what the normalizer will look for when linking
            // methods to their parent type.
            if ( auto r = scope::lookupID<declaration::Type>(u.id().namespace_(), p, "type") ) {
                if ( auto d = r->first->tryAs<declaration::Type>(); d && d->type().isA<type::Struct>() ) {
                    HILTI_DEBUG(logging::debug::Resolver, util::fmt("[%s] setting linkage to 'struct' (%s)",
                                                                    p.node.typename_(), p.node.location()));
                    p.node.as<declaration::Function>().setLinkage(declaration::Linkage::Struct);
                    modified = true;
                }
            }
        }
    }

#endif

    void operator()(declaration::ImportedModule* m) final {
        auto current_module = m->parent<declaration::Module>();
        assert(current_module);

        if ( m->uid() )
            return;

        auto uid = builder->context()->importModule(m->id(), m->scope(), m->parseExtension(),
                                                    current_module->uid().process_extension, m->searchDirectories());

        if ( ! uid ) {
            logger().error(util::fmt("cannot import module '%s': %s", m->id(), uid.error()), m->meta().location());
            return;
        }

        m->setUID(*uid);
        current_module->addDependency(*uid);
        modified = true;
    }

#if 0

    void operator()(declaration::Type* u) final {
        if ( u.type().typeID() )
            return;

        assert(u.canonicalID());
        auto t = addTypeID(u.type(), u.canonicalID(), u.attributes());
        HILTI_DEBUG(logging::debug::Resolver,
                    util::fmt("[%s] setting type ID to %s (%s)", p.node.typename_(), *t.typeID(), p.node.location()));
        p.node.as<declaration::Type>().setType(t);
        modified = true;
    }

    void operator()(expression::Deferred* e) final {
        if ( type::isResolved(e.type()) )
            return;

        if ( expression::isResolved(e.expression()) ) {
            logChange(p.node, e.expression().type());
            p.node.as<expression::Deferred>().setType(e.expression().type());
            modified = true;
        }
    }

    void operator()(expression::Keyword* e) final {
        if ( e.kind() != expression::keyword::Kind::Scope )
            return;

        if ( type::isResolved(e.type()) )
            return;

        logChange(p.node, hilti::type::String());
        p.node.as<expression::Keyword>().setType(hilti::type::String());
        modified = true;
    }

    void operator()(expression::ListComprehension* e) final {
        if ( ! type::isResolved(e.type()) && type::isResolved(e.output().type()) ) {
            logChange(p.node, e.output().type());
            p.node.as<expression::ListComprehension>().setElementType(e.output().type());
            modified = true;
        }

        if ( ! type::isResolved(e.local().type()) && type::isResolved(e.input().type()) ) {
            auto container = e.input().type();
            if ( ! type::isIterable(container) ) {
                p.node.addError("right-hand side of list comprehension is not iterable");
                return;
            }

            const auto& et = *container.elementType();
            logChange(p.node, et);
            p.node.as<expression::ListComprehension>().setLocalType(et);
            modified = true;
        }
    }


    // Helpers for operator resolving
    bool resolveOperator(const expression::UnresolvedOperator& u, position_t p);
    bool resolveFunctionCall(const expression::UnresolvedOperator& u, position_t p);
    bool resolveMethodCall(const expression::UnresolvedOperator& u, position_t p);
    bool resolveCast(const expression::UnresolvedOperator& u, position_t p);
    void recordAutoParameters(const TypePtr& type, const Expression& args);
    std::vector<Node> matchOverloads(const std::vector<Operator>& candidates, const node::Range<Expression>& operands,
                                     const Meta& meta, bool disallow_type_changes = false);

    void operator()(expression::UnresolvedOperator* u) final {
        if ( u.kind() == operator_::Kind::Call && resolveFunctionCall(u, p) )
            return;

        if ( u.kind() == operator_::Kind::MemberCall && resolveMethodCall(u, p) )
            return;

        if ( u.kind() == operator_::Kind::Cast && resolveCast(u, p) )
            return;

        resolveOperator(u, p);
    }

    void operator()(statement::For* u) final {
        if ( type::isResolved(u.local().type()) )
            return;

        if ( ! type::isResolved(u.sequence().type()) )
            return;

        const auto& t = u.sequence().type();
        if ( ! type::isIterable(t) ) {
            p.node.addError("expression is not iterable");
            return;
        }

        const auto& et = *t.iteratorType(true)->dereferencedType();
        logChange(p.node, et);
        p.node.as<statement::For>().setLocalType(et);
        modified = true;
    }

    void operator()(Function* f) final {
        if ( ! f.ftype().result().type().isA<type::Auto>() )
            return;

        // Look for a `return` to infer the return type.
        auto v = visitor::PreOrder<>();
        for ( const auto i : v.walk(&p.node) ) {
            if ( auto x = i.node.tryAs<statement::Return>();
                 x && x->expression() && type::isResolved(x->expression()->type()) ) {
                const auto& rt = x->expression()->type();
                logChange(p.node, rt);
                const_cast<type::function::Result&>(p.node.as<Function>().ftype().result()).setType(rt);
                modified = true;
                break;
            }
        }
    }

    void operator()(type::Enum* m) final {
        if ( type::isResolved(p.node.as<Type>()) )
            return;

        if ( ! p.node.as<Type>().typeID() )
            // Need to make sure we know the type ID before proceeding.
            return;

        // The labels need to store a reference the type's node.
        type::Enum::initLabelTypes(&p.node);
        modified = true;
    }

#endif
};

// Visitor to resolve any auto parameters that we inferred during the main resolver pass.
struct VisitorApplyAutoParameters : visitor::PreOrder {
    VisitorApplyAutoParameters(const ::Resolver& v) : visitor(v) {}

    const ::Resolver& visitor;
    bool modified = false;

#if 0
    void operator()(declaration::Parameter* u) final {
        if ( ! u.type().isA<type::Auto>() )
            return;

        assert(u.canonicalID());
        auto i = visitor.auto_params.find(u.canonicalID());
        if ( i == visitor.auto_params.end() )
            return;

        HILTI_DEBUG(logging::debug::Resolver,
                    util::fmt("[%s] %s -> type %s (%s)", p.node.typename_(), p.node, i->second, p.node.location()));

        p.node.as<declaration::Parameter>().setType(i->second);
        modified = true;
    }
#endif
};

#if 0
bool Visitor::resolveOperator(const expression::UnresolvedOperator& u, position_t p) {
    if ( ! u.areOperandsResolved() )
        return false;

    HILTI_DEBUG(logging::debug::Operator,
                util::fmt("== trying to resolve operator: %s (%s)", to_node(u), u.meta().location().render(true)));
    logging::DebugPushIndent _(logging::debug::Operator);

    std::vector<Node> resolved;

    const auto& candidates = operator_::registry().allOfKind(u.kind());

    if ( u.kind() == operator_::Kind::MemberCall && u.operands().size() >= 2 ) {
        // Pre-filter list of all member-call operators down to those
        // with matching methods. This is just a performance
        // optimization.
        const auto& member_id = u.operands()[1].template as<expression::Member>().id();
        auto filtered = util::filter(candidates, [&](const auto& c) {
            return std::get<Type>(c.operands()[1].type).template as<type::Member>() == member_id;
        });

        resolved = matchOverloads(filtered, u.operands(), u.meta());
    }
    else
        resolved = matchOverloads(candidates, u.operands(), u.meta(), u.kind() == operator_::Kind::Cast);

    if ( resolved.empty() )
        return false;

    if ( resolved.size() > 1 ) {
        std::vector<std::string> context = {"candidates:"};
        for ( auto i : resolved )
            context.emplace_back(util::fmt("- %s [%s]",
                                           detail::renderOperatorPrototype(i.as<expression::ResolvedOperator>()),
                                           i.typename_()));

        p.node.addError(util::fmt("operator usage is ambiguous: %s", detail::renderOperatorInstance(u)),
                        std::move(context));
        return true;
    }

    logChange(p.node, resolved[0].as<Expression>());
    p.node = std::move(resolved[0]);
    modified = true;

#ifndef NDEBUG
    const auto& new_op = p.node.as<Expression>();
    HILTI_DEBUG(logging::debug::Operator, util::fmt("=> resolved to %s (result: %s, expression is %s)", p.node.render(),
                                                    new_op, (new_op.isConstant() ? "const" : "non-const")));
#endif
    return true;
}

bool Visitor::resolveFunctionCall(const expression::UnresolvedOperator& u, position_t p) {
    auto operands = u.operands();
    if ( operands.size() != 2 )
        return false;

    auto callee = operands[0].tryAs<expression::UnresolvedID>();
    if ( ! callee )
        return false;

    auto args_ctor = operands[1].tryAs<expression::Ctor>();
    if ( ! args_ctor ) {
        p.node.addError("function call's argument must be a tuple constant");
        return true;
    }

    if ( ! type::isResolved(args_ctor->type()) )
        return true;

    auto args = args_ctor->ctor().tryAs<ctor::Tuple>();
    if ( ! args ) {
        p.node.addError("function call's argument must be a tuple constant");
        return true;
    }

    std::vector<Operator> candidates;

    for ( auto i = p.path.rbegin(); i != p.path.rend(); i++ ) {
        auto resolved = (**i).scope()->lookupAll(callee->id());
        if ( resolved.empty() )
            continue;

        for ( const auto& r : resolved ) {
            auto d = r.node->tryAs<declaration::Function>();
            if ( ! d ) {
                p.node.addError(util::fmt("ID '%s' resolves to something other than just functions", callee->id()));
                return true;
            }

            if ( r.external && d->linkage() != declaration::Linkage::Public ) {
                p.node.addError(util::fmt("function has not been declared public: %s", r.qualified));
                return true;
            }

            auto op = operator_::function::Call::Operator(r, d->function().ftype());
            candidates.emplace_back(op);
        }

        auto overloads = matchOverloads(candidates, operands, u.meta());
        if ( overloads.empty() )
            break;

        if ( overloads.size() > 1 ) {
            // Ok as long as it's all the same hook, report otherwise.
            const auto& rid = overloads.front()
                                  .template as<expression::ResolvedOperator>()
                                  .op0()
                                  .template as<expression::ResolvedID>();
            const auto& func = rid.declaration().template as<declaration::Function>().function();
            const auto& id = rid.id();

            if ( func.ftype().flavor() != type::function::Flavor::Hook ) {
                std::vector<std::string> context = {"candidate functions:"};

                for ( const auto& i : overloads )
                    context.emplace_back(
                        util::fmt("- %s", detail::renderOperatorPrototype(i.as<expression::ResolvedOperator>())));

                p.node.addError(util::fmt("call is ambiguous: %s", detail::renderOperatorInstance(u)),
                                std::move(context));
                return true;
            }

            for ( const auto& i : overloads ) {
                const auto& rid =
                    i.template as<expression::ResolvedOperator>().op0().template as<expression::ResolvedID>();
                const auto& ofunc = rid.declaration().template as<declaration::Function>().function();
                const auto& oid = rid.id();

                if ( id != oid || func.type() != ofunc.type() ) {
                    std::vector<std::string> context = {"candidate functions:"};

                    for ( const auto& i : overloads )
                        context.emplace_back(
                            util::fmt("- %s", detail::renderOperatorPrototype(i.as<expression::ResolvedOperator>())));

                    p.node.addError(util::fmt("call is ambiguous: %s", detail::renderOperatorInstance(u)),
                                    std::move(context));
                    return true;
                }
            }
        }

        auto n = std::move(overloads.front());
        const auto& r = n.as<expression::ResolvedOperator>();
        const auto& func = r.op0().as<expression::ResolvedID>().declaration().as<declaration::Function>().function();
        recordAutoParameters(func.type(), r.op1());

        if ( ! type::isResolved(r.result()) )
            // Don't do anything before we know the function's return type.
            // Note that we delay this check until we have checked for any auto
            // parameters we might be able to resolve.
            return true;

        // Found a match.
        logChange(p.node, n.as<Expression>());
        p.node = std::move(n);
        modified = true;

        return true;
    }

    // No match found.
    std::vector<std::string> context;

    if ( ! candidates.empty() ) {
        context.emplace_back("candidate functions:");
        for ( const auto& i : candidates ) {
            auto rop = i.instantiate(u.operands().copy(), u.meta()).as<expression::ResolvedOperator>();
            context.emplace_back(util::fmt("- %s", detail::renderOperatorPrototype(rop)));
        }
    }

    p.node.addError(util::fmt("call does not match any function: %s", detail::renderOperatorInstance(u)),
                    std::move(context));
    return true;
}

bool Visitor::resolveMethodCall(const expression::UnresolvedOperator& u, position_t p) {
    auto operands = u.operands();
    if ( operands.size() != 3 )
        return false;

    std::vector<Node> shadow_ops;

    auto ty0 = operands[0].type();
    if ( type::isReferenceType(ty0) )
        ty0 = *ty0.dereferencedType();

    auto stype = ty0.tryAs<type::Struct>();
    if ( ! stype ) {
        // Allow a still unresolved ID here so that we can start resolving auto parameters below.
        if ( auto id = operands[0].tryAs<expression::UnresolvedID>() ) {
            if ( auto resolved = scope::lookupID<Declaration>(id->id(), p, "declaration") ) {
                // We temporarily create our own resolved ID for overload matching.
                shadow_ops.emplace_back(Expression(expression::ResolvedID(resolved->second, NodeRef(resolved->first))));
                shadow_ops.emplace_back(operands[1]);
                shadow_ops.emplace_back(operands[2]);

                const auto& sty0 = shadow_ops[0].as<Expression>().type();
                if ( type::isReferenceType(sty0) )
                    stype = sty0.dereferencedType()->tryAs<type::Struct>();
                else
                    stype = sty0.tryAs<type::Struct>();
            }
        }
    }

    if ( ! stype && type::isResolved(operands[0].type()) && type::isReferenceType(operands[0].type()) )
        stype = derefOperand(operands[0]).type().tryAs<type::Struct>();

    if ( ! stype )
        return false;

    auto member = operands[1].tryAs<expression::Member>();
    if ( ! member )
        return false;

    auto args_ctor = operands[2].tryAs<expression::Ctor>();
    if ( ! args_ctor ) {
        p.node.addError("method call's argument must be a tuple constant");
        return true;
    }

    if ( ! type::isResolved(args_ctor->type()) )
        return true;

    auto args = args_ctor->ctor().tryAs<ctor::Tuple>();
    if ( ! args ) {
        p.node.addError("method call's argument must be a tuple constant");
        return true;
    }

    auto fields = stype->fields(member->id());
    if ( fields.empty() ) {
        p.node.addError(
            util::fmt("struct type %s does not have a method `%s`", stype->meta().location(), member->id()));
        return false; // Continue trying to find another match.
    }

    for ( const auto& f : fields ) {
        if ( ! f.type().isA<type::Function>() ) {
            p.node.addError(util::fmt("struct attribute '%s' is not a function", member->id()));
            return true;
        }
    }

    auto candidates = node::transform(fields, [&](const auto& f) -> Operator {
        auto x = operator_::struct_::MemberCall::Operator(*stype, f);
        return std::move(x);
    });

    auto overloads =
        matchOverloads(candidates, shadow_ops.empty() ? operands : node::Range<Expression>(shadow_ops), u.meta());

    if ( overloads.empty() ) {
        std::vector<std::string> context;

        if ( ! candidates.empty() ) {
            context.emplace_back("candidate methods:");
            for ( const auto& i : candidates ) {
                auto rop = i.instantiate(u.operands().copy(), u.meta()).as<expression::ResolvedOperator>();
                context.emplace_back(util::fmt("- %s", detail::renderOperatorPrototype(rop)));
            }
        }

        p.node.addError(util::fmt("call does not match any method: %s", detail::renderOperatorInstance(u)),
                        std::move(context));
        return true;
    }

    if ( overloads.size() > 1 ) {
        std::vector<std::string> context = {"candidates:"};
        for ( const auto& i : overloads )
            context.emplace_back(
                util::fmt("- %s", detail::renderOperatorPrototype(i.as<expression::ResolvedOperator>())));

        p.node.addError(util::fmt("method call is ambiguous: %s", detail::renderOperatorInstance(u)),
                        std::move(context));
        return true;
    }

    const auto& n = overloads.front().as<expression::ResolvedOperator>();
    const auto& ft = n.op1().as<expression::Member>().type().as<type::Function>();
    const auto& id = n.op1().as<expression::Member>().id();
    const auto& ftype = stype->field(id)->type();
    recordAutoParameters(ftype, n.op2());

    if ( ! type::isResolved(ft.result().type()) || shadow_ops.size() )
        // Don't do anything before we know the function's return type.
        // Note that we delay this check until we have checked for any auto
        // parameters we might be able to resolve.
        return true;

    logChange(p.node, n);
    p.node = std::move(overloads.front());
    modified = true;

    return true;
}

/** Returns a set of overload alternatives matching a given operand expression. */
bool Visitor::resolveCast(const expression::UnresolvedOperator& u, position_t p) {
    if ( ! u.areOperandsResolved() )
        return false;

    // We hardcode that a cast<> operator can always perform any
    // legal coercion. This helps in cases where we need to force a
    // specific coercion to take place.
    const auto& operands = u.operands();
    const auto& expr = operands[0];
    const auto& dst = operands[1].as<expression::Type_>().typeValue();
    const auto style = CoercionStyle::TryAllForMatching | CoercionStyle::ContextualConversion;

    if ( auto c = hilti::coerceExpression(expr, dst, style) ) {
        auto nop = operator_::generic::CastedCoercion::Operator().instantiate(u.operands().copy(), u.meta());

        logChange(p.node, nop);
        p.node = nop;
        modified = true;
        return true;
    }

    return false;
}

void Visitor::recordAutoParameters(const TypePtr& type, const Expression& args) {
    const auto& ftype = type.as<type::Function>();

    auto arg = args.as<expression::Ctor>().ctor().as<ctor::Tuple>().value().begin();
    std::vector<type::function::Parameter> params;
    for ( auto& rp : ftype.parameterRefs() ) {
        auto p = rp->as<declaration::Parameter>();
        if ( ! p.type().isA<type::Auto>() )
            continue;

        auto t = (*arg).type();
        if ( ! type::isResolved(t) )
            continue;

        assert(p.canonicalID());
        const auto& i = auto_params.find(p.canonicalID());
        if ( i == auto_params.end() ) {
            auto_params.emplace(p.canonicalID(), t);
            HILTI_DEBUG(logging::debug::Resolver,
                        util::fmt("recording auto parameter %s as of type %s", p.canonicalID(), t));
        }
        else {
            if ( i->second != t )
                const_cast<Node&>(*rp).addError("mismatch for auto parameter");
        }

        ++arg;
    }
}

#endif

} // anonymous namespace


bool hilti::detail::ast::resolve(Builder* builder, const ASTRootPtr& root) {
    util::timing::Collector _("hilti/compiler/ast/resolver");

    auto v1 = Resolver(builder, root);
    hilti::visitor::visit(v1, root);

    auto v2 = VisitorApplyAutoParameters(v1);
    hilti::visitor::visit(v2, root);

    return v1.modified || v2.modified;
}
