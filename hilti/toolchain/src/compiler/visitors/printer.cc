// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <cstdio>

#include <hilti/ast/all.h>
#include <hilti/ast/function.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/auto.h>
#include <hilti/ast/visitor.h>
#include <hilti/base/logger.h>
#include <hilti/base/util.h>
#include <hilti/compiler/detail/visitors.h>
#include <hilti/compiler/plugin.h>
#include <hilti/compiler/printer.h>

#include "base/timing.h"

using namespace hilti;
using util::fmt;

// Global state storing any scopes we are currently in during printing.
// Maintaining this globally isn't great, but because of various independent
// `printAST()` calls happening recursively through `operator<<` and `fmt()`,
// we can't easily pass this state around.
static std::vector<ID> _scopes = {""};

static const ID& _currentScope() { return _scopes.back(); }
static void _pushScope(ID id) { _scopes.push_back(std::move(id)); }
static void _popScope() { _scopes.pop_back(); }

static std::string renderOperator(operator_::Kind kind, const std::vector<std::string>& ops) {
    switch ( kind ) {
        case operator_::Kind::Add: return fmt("add %s[%s]", ops[0], ops[1]);
        case operator_::Kind::Begin: return fmt("begin(%s)", ops[0]);
        case operator_::Kind::BitAnd: return fmt("%s & %s", ops[0], ops[1]);
        case operator_::Kind::BitOr: return fmt("%s | %s", ops[0], ops[1]);
        case operator_::Kind::BitXor: return fmt("%s ^ %s", ops[0], ops[1]);
        case operator_::Kind::Call: return fmt("%s%s", ops[0], ops[1]);
        case operator_::Kind::Cast: return fmt("cast<%s>(%s)", ops[1], ops[0]);
        case operator_::Kind::CustomAssign: return fmt("%s = %s", ops[0], ops[1]);
        case operator_::Kind::DecrPostfix: return fmt("%s--", ops[0]);
        case operator_::Kind::DecrPrefix: return fmt("--%s", ops[0]);
        case operator_::Kind::Delete: return fmt("delete %s[%s]", ops[0], ops[1]);
        case operator_::Kind::Deref: return fmt("(*%s)", ops[0]);
        case operator_::Kind::Difference: return fmt("%s - %s", ops[0], ops[1]);
        case operator_::Kind::DifferenceAssign: return fmt("%s -= %s", ops[0], ops[1]);
        case operator_::Kind::Division: return fmt("%s / %s", ops[0], ops[1]);
        case operator_::Kind::DivisionAssign: return fmt("%s /= %s", ops[0], ops[1]);
        case operator_::Kind::Equal: return fmt("%s == %s", ops[0], ops[1]);
        case operator_::Kind::End: return fmt("end(%s)", ops[0]);
        case operator_::Kind::Greater: return fmt("%s > %s", ops[0], ops[1]);
        case operator_::Kind::GreaterEqual: return fmt("%s >= %s", ops[0], ops[1]);
        case operator_::Kind::HasMember: return fmt("%s?.%s", ops[0], ops[1]);
        case operator_::Kind::In: return fmt("%s in %s", ops[0], ops[1]);
        case operator_::Kind::IncrPostfix: return fmt("%s++", ops[0]);
        case operator_::Kind::IncrPrefix: return fmt("++%s", ops[0]);
        case operator_::Kind::Index: return fmt("%s[%s]", ops[0], ops[1]);
        case operator_::Kind::IndexAssign: return fmt("%s[%s] = %s", ops[0], ops[1], ops[2]);
        case operator_::Kind::Lower: return fmt("%s < %s", ops[0], ops[1]);
        case operator_::Kind::LowerEqual: return fmt("%s <= %s", ops[0], ops[1]);
        case operator_::Kind::Member: return fmt("%s.%s", ops[0], ops[1]);
        case operator_::Kind::MemberCall: return fmt("%s.%s%s", ops[0], ops[1], ops[2]);
        case operator_::Kind::Modulo: return fmt("%s %% %s", ops[0], ops[1]);
        case operator_::Kind::Multiple: return fmt("%s * %s", ops[0], ops[1]);
        case operator_::Kind::MultipleAssign: return fmt("%s *= %s", ops[0], ops[1]);
        case operator_::Kind::Negate: return fmt("~%s", ops[0]);
        case operator_::Kind::New: return fmt("new %s%s", ops[0], ops[1]);
        case operator_::Kind::Pack: return fmt("pack%s", ops[0]);
        case operator_::Kind::Power: return fmt("%s ** %s", ops[0], ops[1]);
        case operator_::Kind::ShiftLeft: return fmt("%s << %s", ops[0], ops[1]);
        case operator_::Kind::ShiftRight: return fmt("%s >> %s", ops[0], ops[1]);
        case operator_::Kind::SignNeg: return fmt("-%s", ops[0]);
        case operator_::Kind::SignPos: return fmt("+%s", ops[0]);
        case operator_::Kind::Size: return fmt("|%s|", ops[0]);
        case operator_::Kind::Sum: return fmt("%s + %s", ops[0], ops[1]);
        case operator_::Kind::SumAssign: return fmt("%s += %s", ops[0], ops[1]);
        case operator_::Kind::TryMember: return fmt("%s.?%s", ops[0], ops[1]);
        case operator_::Kind::Unequal: return fmt("%s != %s", ops[0], ops[1]);
        case operator_::Kind::Unpack: return fmt("unpack<%s>(%s)", ops[0], ops[1]);
        case operator_::Kind::Unknown: logger().internalError("\"unknown\" operator");
        case operator_::Kind::Unset: return fmt("unset %s", ops[0]);
    }

    util::cannot_be_reached();
}

static std::string renderExpressionType(const ExpressionPtr& e) {
    auto const_ = (e->isConstant() && type::isMutable(e->type()) ? "const " : "");
    return fmt("%s%s", const_, e->type());
}

static std::string renderOperand(const operator_::Operand& op, const node::Range<Expression>& exprs) {
    auto t = op.type;
    std::string s = (t ? fmt("%s", *t) : "<no-type>");

    if ( op.default_ )
        s = fmt("%s=%s", s, *op.default_);

    if ( op.optional || op.default_ )
        s = fmt("[%s]", s);

    return s;
}

namespace {

struct Printer : visitor::PreOrder {
    Printer(printer::Stream& out) : out(out) {} // NOLINT

    void printFunctionType(const type::Function& ftype, const std::optional<ID>& id) {
        if ( ftype.isWildcard() ) {
            out << "<function>";
            return;
        }

        if ( ftype.flavor() != type::function::Flavor::Standard )
            out << to_string(ftype.flavor()) << ' ';

        out << ftype.result() << ' ';

        if ( id )
            out << *id;

        out << '(';
        out << std::make_pair(ftype.parameters(), ", ");
        out << ')';
    }

    void printDoc(const std::optional<DocString>& doc) {
        if ( doc && *doc ) {
            out.emptyLine();
            doc->print(out);
        }
    }

    auto linkage(declaration::Linkage l) {
        switch ( l ) {
            case declaration::Linkage::Init: return "init ";
            case declaration::Linkage::PreInit: return "preinit ";
            case declaration::Linkage::Struct: return "method ";
            case declaration::Linkage::Private: return ""; // That's the default.
            case declaration::Linkage::Public: return "public ";
            default: util::cannot_be_reached();
        }
    }

    auto const_(const QualifiedType* t) {
        return (out.isCompact() && t->isConstant() && type::isMutable(*t)) ? "const " : "";
    }

    void operator()(Attribute* n) final {
        out << n->tag();

        if ( n->hasValue() )
            out << "=" << n->value();
    }

    void operator()(AttributeSet* n) final {
        bool first = true;
        for ( const auto& a : n->attributes() ) {
            if ( ! first )
                out << ' ';
            else
                first = false;

            out << a;
        }
    }

    void operator()(Function* n) final {
        if ( n->callingConvention() != function::CallingConvention::Standard )
            out << to_string(n->callingConvention()) << ' ';

        printFunctionType(*n->ftype(), n->id());

        if ( n->attributes() )
            out << ' ' << std::make_pair(n->attributes()->attributes(), " ");

        if ( n->body() )
            out << ' ' << n->body();
        else
            out << ';' << out.newline();
    }

    void operator()(ID* n) {
        if ( n->namespace_() == _currentScope() )
            out << std::string(n->local());
        else
            out << std::string(*n);
    }

    void operator()(declaration::Module* n) final {
        printDoc(n->documentation());
        out.beginLine();
        out << "module " << n->id() << " {" << out.newline();
        out.endLine();

        _pushScope(n->id());

        auto printDecls = [&](const auto& decls) {
            for ( const auto& d : decls )
                out << d;

            if ( decls.size() )
                out.emptyLine();
        };

        printDecls(util::filter(n->declarations(),
                                [](const auto& d) { return d->template isA<declaration::ImportedModule>(); }));
        printDecls(util::filter(n->declarations(), [](const auto& d) { return d->template isA<declaration::Type>(); }));
        printDecls(
            util::filter(n->declarations(), [](const auto& d) { return d->template isA<declaration::Constant>(); }));
        printDecls(util::filter(n->declarations(),
                                [](const auto& d) { return d->template isA<declaration::GlobalVariable>(); }));
        printDecls(
            util::filter(n->declarations(), [](const auto& d) { return d->template isA<declaration::Function>(); }));

        for ( const auto& s : n->statements()->statements() )
            out << s;

        if ( ! n->statements()->statements().empty() )
            out.emptyLine();

        _popScope();

        out.beginLine();
        out << "}";
        out.endLine();
    }

    ////// Ctors

    void operator()(ctor::Address* n) override { out << n->value(); }

    void operator()(ctor::Bool* n) final { out << (n->value() ? "True" : "False"); }

    void operator()(ctor::Bytes* n) final { out << "b\"" << util::escapeUTF8(n->value(), true) << '"'; }

    void operator()(ctor::Coerced* n) final { out << n->originalCtor(); }

    void operator()(ctor::Default* n) final {
        out << "default<" << n->type() << ">(" << std::make_pair(n->typeArguments(), ", ") << ")";
    }

    void operator()(ctor::Enum* n) final {
        assert(n->type()->type()->typeID());
        out << *n->type()->type()->typeID() << "::" << n->value()->id();
    }

    void operator()(ctor::Error* n) final { out << "error(\"" << n->value() << "\")"; }

    void operator()(ctor::Interval* n) final { out << "interval_ns(" << n->value().nanoseconds() << ")"; }

    void operator()(ctor::List* n) final { out << '[' << std::make_pair(n->value(), ", ") << ']'; }

    void operator()(ctor::Map* n) final {
        auto elems = node::transform(n->value(),
                                     [](const auto& e) -> std::string { return fmt("%s: %s", e->key(), e->value()); });
        out << "map(" << std::make_pair(elems, ", ") << ')';
    }

    void operator()(ctor::Network* n) final { out << n->value(); }

    void operator()(ctor::Null* n) final { out << "Null"; }

    void operator()(ctor::Optional* n) final {
        if ( n->value() )
            out << n->value();
        else
            out << "Null";
    }

    void operator()(ctor::Port* n) final { out << n->value(); }

    void operator()(ctor::Real* n) final {
        // We use hexformat for lossless serialization-> Older platforms like
        // centos7 have inconsistent support for that in iostreams so we use
        // C99 snprintf instead.
        constexpr size_t size = 256;
        char buf[size];
        assert(std::snprintf(buf, size, "%a", n->value()) >= 0);
        out << buf;
    }

    void operator()(ctor::StrongReference* n) final { out << "Null"; }

    void operator()(ctor::RegExp* n) final {
        out << std::make_pair(util::transform(n->value(), [](auto p) { return fmt("/%s/", p); }), " |");
    }

    void operator()(ctor::Result* n) final {
        if ( n->value() )
            out << n->value();
        else
            out << n->error();
    }

    void operator()(ctor::Set* n) final { out << "set(" << std::make_pair(n->value(), ", ") << ')'; }

    void operator()(ctor::SignedInteger* n) final {
        if ( n->width() < 64 )
            out << fmt("int%d(%" PRId64 ")", n->width(), n->value());
        else
            out << n->value();
    }

    void operator()(ctor::Stream* n) final { out << "stream(" << util::escapeUTF8(n->value(), true) << ')'; }

    void operator()(ctor::String* n) final { out << '"' << util::escapeUTF8(n->value(), true) << '"'; }

    void operator()(ctor::Struct* n) final {
        out << "[";

        bool first = true;
        for ( const auto& f : n->fields() ) {
            if ( ! first )
                out << ", ";
            else
                first = false;

            out << '$' << f->id() << "=" << f->expression();
        }

        out << "]";
    }

    void operator()(ctor::Time* n) final { out << "time_ns(" << n->value().nanoseconds() << ")"; }

    void operator()(ctor::Tuple* n) final { out << '(' << std::make_pair(n->value(), ", ") << ')'; }

    void operator()(ctor::UnsignedInteger* n) final {
        if ( n->width() < 64 )
            out << fmt("uint%d(%" PRId64 ")", n->width(), n->value());
        else
            out << n->value();
    }

    void operator()(ctor::Vector* n) final { out << "vector(" << std::make_pair(n->value(), ", ") << ')'; }

    void operator()(ctor::WeakReference* n) final { out << "Null"; }

    void operator()(ctor::ValueReference* n) final { out << "value_ref(" << n->expression() << ')'; }

    ////// Declarations

    void operator()(declaration::Constant* n) final {
        printDoc(n->documentation());
        out.beginLine();
        out << linkage(n->linkage()) << "const ";
        out << n->type();
        out << ' ' << n->id() << " = " << n->value() << ';';
        out.endLine();
    }

    void operator()(declaration::Expression* n) final { out << n->expression(); }

    void operator()(declaration::Field* n) final {
        out << "    ";

        if ( auto ft = n->type()->tryAs<type::Function>() ) {
            out << to_string(ft->flavor()) << " ";

            if ( auto cc = n->callingConvention(); cc && *cc != function::CallingConvention::Standard )
                out << to_string(*cc) << ' ';

            out << ft->result()->type() << " " << n->id() << "(" << std::make_pair(ft->parameters(), ", ") << ")";
        }

        else
            out << n->type() << ' ' << n->id();

        if ( n->attributes() )
            out << ' ' << n->attributes();

        if ( auto f = n->inlineFunction(); f && f->body() ) {
            const auto& block = f->body()->tryAs<statement::Block>();
            if ( block && block->statements().empty() ) {
                out << " {}";
                out.endLine();
            }
            else if ( block && block->statements().size() == 1 ) {
                auto old_compact = out.setCompact(true);
                out << " { " << *block->statements().begin() << " }";
                out.setCompact(old_compact);
                out.endLine();
            }
            else {
                out.incrementIndent();
                out << ' ' << f->body();
                out.decrementIndent();
            }
        }
        else
            out << ";" << out.newline();
    }

    void operator()(declaration::Parameter* n) final {
        auto kind = [&](auto k) {
            switch ( k ) {
                case declaration::parameter::Kind::Copy: return "copy ";
                case declaration::parameter::Kind::In: return "";
                case declaration::parameter::Kind::InOut: return "inout ";
                case declaration::parameter::Kind::Unknown: logger().internalError("parameter kind not set");
            }

            util::cannot_be_reached();
        };

        out << kind(n->kind()) << n->type() << ' ' << n->id();

        if ( n->default_() )
            out << " = " << n->default_();

        if ( const auto attrs = n->attributes(); attrs && ! attrs->attributes().empty() )
            out << ' ' << attrs;
    }

    void operator()(declaration::Function* n) final {
        const auto& func = n->function();

        if ( ! func->body() ) {
            printDoc(n->documentation());
            out.beginLine();
            out << "declare ";
        }
        else {
            out.emptyLine();
            printDoc(n->documentation());
            out.beginLine();
        }

        out << linkage(n->linkage());

        if ( n->linkage() != declaration::Linkage::Struct )
            out << "function ";

        out << n->function();
    }

    void operator()(declaration::ImportedModule* n) final {
        out.beginLine();
        if ( n->scope() )
            out << "import " << n->id() << " from " << *n->scope() << ';';
        else
            out << "import " << n->id() << ';';

        out.endLine();
    }

    void operator()(declaration::Type* n) final {
        printDoc(n->documentation());
        out.beginLine();
        for ( const auto& comment : n->meta().comments() )
            out << "# " << comment << '\n';
        out << linkage(n->linkage()) << "type " << n->id() << " = ";
        out.setExpandSubsequentType(true);
        out << n->type();

        if ( n->attributes() )
            out << ' ' << n->attributes();

        out << ';';
        out.endLine();
    }

    void operator()(declaration::LocalVariable* n) final {
        // Will be printed through a statement, hence no outer formatting.
        out << "local ";

        if ( n->type() )
            out << n->type() << ' ';

        out << n->id();

        if ( n->typeArguments().size() )
            out << '(' << std::make_pair(n->typeArguments(), ", ") << ')';

        if ( n->init() )
            out << " = " << n->init();
    }

    void operator()(declaration::GlobalVariable* n) final {
        printDoc(n->documentation());
        out.beginLine();
        out << linkage(n->linkage()) << "global ";

        if ( n->type() )
            out << n->type() << ' ';

        out << n->id();

        if ( n->typeArguments().size() )
            out << '(' << std::make_pair(n->typeArguments(), ", ") << ')';

        if ( n->init() )
            out << " = " << n->init();

        out << ';';
        out.endLine();
    }

    ////// Expressions

    void operator()(expression::Assign* n) final { out << n->target() << " = " << n->source(); }

    void operator()(expression::BuiltInFunction* n) final {
        out << n->name() << "("
            << util::join(node::transform(n->arguments(), [](auto& p) { return fmt("%s", p); }), ", ") << ")";
    }

    void operator()(expression::Coerced* n) final { out << n->expression(); }

    void operator()(expression::Ctor* n) final { out << n->ctor(); }

    void operator()(expression::Grouping* n) final { out << '(' << n->expression() << ')'; }

    void operator()(expression::Keyword* n) final {
        switch ( n->kind() ) {
            case expression::keyword::Kind::Self: out << "self"; break;
            case expression::keyword::Kind::DollarDollar: out << "$$"; break;
            case expression::keyword::Kind::Captures:
                out << "$@"; // this is technically not valid source code; we don't expose this to users
                break;
            case expression::keyword::Kind::Scope: out << "$scope"; break;
        }
    }

    void operator()(expression::ListComprehension* n) final {
        out << '[' << n->output() << " for " << n->local() << " in " << n->input();

        if ( n->condition() )
            out << " if " << n->condition();

        out << ']';
    }

    void operator()(expression::LogicalAnd* n) final { out << n->op0() << " &* " << n->op1(); }

    void operator()(expression::LogicalNot* n) final { out << "! " << n->expression(); }

    void operator()(expression::LogicalOr* n) final { out << n->op0() << " || " << n->op1(); }

    void operator()(expression::Member* n) final { out << n->id(); }

    void operator()(expression::Move* n) final { out << "move(" << n->expression() << ")"; }

    void operator()(expression::Name* n) final { out << n->id(); }

    void operator()(expression::Ternary* n) final {
        out << n->condition() << " ? " << n->true_() << " : " << n->false_();
    }

    void operator()(expression::Type_* n) final {
        if ( auto id = n->typeValue()->type()->typeID() )
            out << *id;
        else
            out << n->typeValue();
    }

    void operator()(expression::TypeInfo* n) final { out << "typeinfo(" << n->expression() << ")"; }

    void operator()(expression::TypeWrapped* n) final { out << n->expression(); }

    void operator()(expression::Void* n) final {
        out << "<void expression>"; // Shouldn't really happen->
    }

    ////// Statements

    void operator()(statement::Assert* n) final {
        out.beginLine();

        if ( n->expectsException() )
            out << "assert-exception ";
        else
            out << "assert ";

        out << n->expression();
        if ( n->message() )
            out << " : " << n->message();
        out << ";";
        out.endLine();
    }

    void operator()(statement::Block* n) final {
        if ( out.indent() == 0 || n->statements().size() > 1 )
            out << "{";

        out.endLine();
        out.incrementIndent();

        const auto& stmts = n->statements();
        for ( const auto&& [i, s] : util::enumerate(stmts) ) {
            out.setPositionInBlock(i == 0, i == (stmts.size() - 1));

            if ( s->isA<statement::Block>() )
                out.beginLine();

            out << s;

            if ( s->isA<statement::Block>() )
                out.endLine();
        }

        out.decrementIndent();

        if ( out.indent() == 0 || n->statements().size() > 1 ) {
            out.beginLine();
            out << "}";
            out.endLine();
        }
    }

    void operator()(statement::Break* n) final {
        out.beginLine();
        out << "break;";
        out.endLine();
    }

    void operator()(statement::Continue* n) final {
        out.beginLine();
        out << "continue;";
        out.endLine();
    }

    void operator()(statement::Comment* n) final {
        if ( (n->separator() == hilti::statement::comment::Separator::Before ||
              n->separator() == hilti::statement::comment::Separator::BeforeAndAfter) &&
             ! out.isFirstInBlock() )
            out.emptyLine();

        out.beginLine();
        out << "# " << n->comment();
        out.endLine();

        if ( (n->separator() == hilti::statement::comment::Separator::After ||
              n->separator() == hilti::statement::comment::Separator::BeforeAndAfter) &&
             ! out.isLastInBlock() )
            out.emptyLine();
    }

    void operator()(statement::Declaration* n) final {
        out.beginLine();
        out << n->declaration() << ';';
        out.endLine();
    }

    void operator()(statement::Expression* n) final {
        out.beginLine();
        out << n->expression() << ';';
        out.endLine();
    }

    void operator()(statement::For* n) final {
        out.emptyLine();
        out.beginLine();
        out << "for ( " << n->local()->id() << " in " << n->sequence() << " ) " << n->body();
        out.endLine();
    }

    void operator()(statement::If* n) final {
        out.emptyLine();
        out.beginLine();
        out << "if ( ";

        if ( auto e = n->init() )
            out << e << "; ";

        if ( auto e = n->condition() )
            out << e;

        out << " ) " << n->true_();

        if ( n->false_() ) {
            out.beginLine();
            out << "else " << n->false_();
        }

        out.endLine();
    }

    void operator()(statement::SetLocation* n) final {
        out.beginLine();
        out << "# " << n->expression();
        out.endLine();
    }

    void operator()(statement::Return* n) final {
        out.beginLine();
        out << "return";

        if ( auto e = n->expression() )
            out << ' ' << e;

        out << ';';
        out.endLine();
    }

    void operator()(statement::Switch* n) final {
        out.emptyLine();
        out.beginLine();
        out << "switch ( ";

        if ( const auto& cond = n->condition(); cond->id().str() != "__x" )
            out << cond;
        else
            out << cond->init();

        out << " ) {";
        out.incrementIndent();
        out.endLine();

        for ( const auto& c : n->cases() ) {
            out.beginLine();

            if ( ! c->isDefault() )
                out << "case " << std::make_pair(c->expressions(), ", ") << ": ";
            else
                out << "default: ";

            out << c->body();
            out.endLine();
        }

        out.decrementIndent();
        out.beginLine();
        out << "}";
        out.endLine();
    }

    void operator()(statement::Throw* n) final {
        out.beginLine();
        out << "throw";

        if ( auto e = n->expression() )
            out << fmt(" %s", *e);

        out << ";";
        out.endLine();
    }

    void operator()(statement::try_::Catch* n) final {
        out.beginLine();
        out << "catch ";

        if ( auto p = n->parameter() )
            out << "( " << p << " ) ";

        out << n->body();
    }

    void operator()(statement::Try* n) final {
        out.beginLine();
        out << "try " << n->body();

        for ( const auto& c : n->catches() )
            out << c;

        out.endLine();
    }

    void operator()(statement::While* n) final {
        out.emptyLine();
        out.beginLine();
        out << "while ( ";

        if ( auto e = n->init() )
            out << e << "; ";

        if ( auto e = n->condition() )
            out << e;

        out << " ) " << n->body();

        if ( n->else_() ) {
            out.beginLine();
            out << "else " << n->else_();
        }

        out.endLine();
    }

    void operator()(statement::Yield* n) final {
        out.beginLine();
        out << "yield";
        out.endLine();
    }

#if 0
    void operator()(expression::ResolvedOperator* n) final {
        out << renderOperator(n->operator_().kind(), node::transform(n->operands(), [](auto o) { return fmt("%s", o); }));
    }
#endif

    void operator()(expression::UnresolvedOperator* n) final {
        out << renderOperator(n->kind(),
                              node::transform(n->operands(), [](auto& o) -> std::string { return fmt("%s", *o); }));
    }

    ////// Types

    void operator()(QualifiedType* n) final { out << const_(n) << n->type(); }

    void operator()(type::Any* n) final { out << "any"; }

    void operator()(type::Address* n) final { out << "addr"; }

    void operator()(type::Auto* n) final { out << "auto"; }

    void operator()(type::Bool* n) final { out << "bool"; }

    void operator()(type::Bytes* n) final { out << "bytes"; }

    void operator()(type::enum_::Label* n) final { out << n->id() << " = " << n->value(); }

    void operator()(type::Enum* n) final {
        if ( ! out.isExpandSubsequentType() ) {
            out.setExpandSubsequentType(false);
            if ( auto id = n->typeID() ) {
                out << *id;
                return;
            }
        }

        out.setExpandSubsequentType(false);

        auto x = util::transform(util::filter(n->labels(), [](const auto& l) { return l.get()->id() != ID("Undef"); }),
                                 [](const auto& l) { return l.get(); });

        out << "enum { " << std::make_pair(std::move(x), ", ") << " }";
    }

    void operator()(type::Error* n) final { out << "error"; }

    void operator()(type::Exception* n) final {
        out << "exception";

        if ( auto t = n->baseType() ) {
            out << " : ";
            if ( auto id = t->typeID() )
                out << *id;
            else
                out << t;
        }
    }

    void operator()(type::Function* n) final {
        out << "function ";
        printFunctionType(*n, {});
    }

    void operator()(type::Interval* n) final { out << "interval"; }

    void operator()(type::Member* n) final { out << n->id(); }

    void operator()(type::Name* n) final { out << n->id(); }

    void operator()(type::Network* n) final { out << "net"; }

    void operator()(type::Null* n) final { out << "<null type>"; }

    void operator()(type::OperandList* n) final { out << "<operand list>"; }

    void operator()(type::Optional* n) final {
        if ( n->isWildcard() )
            out << "optional<*>";
        else {
            out << "optional<" << n->dereferencedType() << ">";
        }
    }

    void operator()(type::Port* n) final { out << "port"; }

    void operator()(type::Real* n) final { out << "real"; }

    void operator()(type::StrongReference* n) final {
        if ( n->isWildcard() )
            out << "strong_ref<*>";
        else
            out << "strong_ref<" << n->dereferencedType() << ">";
    }

    void operator()(type::Stream* n) final { out << "stream"; }

    void operator()(type::bytes::Iterator* n) final { out << "iterator<bytes>"; }

    void operator()(type::list::Iterator* n) final {
        if ( n->isWildcard() )
            out << "iterator<list<*>>";
        else
            out << fmt("iterator<list<%s>>", *n->dereferencedType());
    }

    void operator()(type::stream::Iterator* n) final { out << "iterator<stream>"; }

    void operator()(type::vector::Iterator* n) final {
        if ( n->isWildcard() )
            out << "iterator<vector<*>>";
        else
            out << fmt("iterator<vector<%s>>", *n->dereferencedType());
    }

    void operator()(type::stream::View* n) final { out << "view<stream>"; }

    void operator()(type::Library* n) final {
        if ( auto id = n->typeID() )
            out << *id;
        else
            out << fmt("__library_type(\"%s\")", n->cxxName());
    }

    void operator()(type::List* n) final {
        if ( n->isWildcard() )
            out << "list<*>";
        else {
            out << "list<" << n->elementType() << ">";
        }
    }

    void operator()(type::map::Iterator* n) final {
        if ( n->isWildcard() )
            out << "iterator<map<*>>";
        else
            out << fmt("iterator<map<%s>>", *n->dereferencedType());
    }

    void operator()(type::Map* n) final {
        if ( n->isWildcard() )
            out << "map<*>";
        else {
            out << "map<" << n->keyType() << ", " << n->valueType() << ">";
        }
    }

    void operator()(type::RegExp* n) final { out << "regexp"; }

    void operator()(type::Result* n) final {
        if ( n->isWildcard() )
            out << "result<*>";
        else {
            out << "result<" << n->dereferencedType() << ">";
        }
    }

    void operator()(type::set::Iterator* n) final {
        if ( n->isWildcard() )
            out << "iterator<set<*>>";
        else
            out << fmt("iterator<set<%s>>", *n->dereferencedType());
    }

    void operator()(type::Set* n) final {
        if ( n->isWildcard() )
            out << "set<*>";
        else {
            out << "set<" << n->elementType() << ">";
        }
    }

    void operator()(type::SignedInteger* n) final {
        if ( n->isWildcard() )
            out << "int<*>";
        else
            out << fmt("int<%d>", n->width());
    }

    void operator()(type::String* n) final { out << "string"; }

    void operator()(type::Struct* n) final {
        if ( ! out.isExpandSubsequentType() ) {
            if ( auto id = n->typeID() ) {
                out << *id;

                if ( n->parameters().size() )
                    out << '(' << std::make_pair(n->parameters(), ", ") << ')';

                return;
            }
        }

        out.setExpandSubsequentType(false);

        out << "struct";

        if ( n->parameters().size() )
            out << " (" << std::make_pair(n->parameters(), ", ") << ')';

        auto printFields = [&](const auto& fields) {
            for ( const auto& f : fields )
                out << f;
        };

        out << " {" << out.newline();
        printFields(
            util::filter(n->fields(), [](const auto& f) { return ! f->type()->template isA<type::Function>(); }));
        printFields(util::filter(n->fields(), [](const auto& f) { return f->type()->template isA<type::Function>(); }));
        out << "}";
    }

    void operator()(type::Time* n) final { out << "time"; }

    void operator()(type::Type_* n) final { out << fmt("type<%s>", n->typeValue()); }

    void operator()(type::Union* n) final {
        if ( ! out.isExpandSubsequentType() ) {
            if ( auto id = n->typeID() ) {
                out << *id;
                return;
            }
        }

        out.setExpandSubsequentType(false);

        out << "union {" << out.newline();

        for ( const auto& f : n->fields() )
            out << f;

        out << "}";
    }

    void operator()(type::Unknown* n) final { out << "<unknown type>"; }

    void operator()(type::UnsignedInteger* n) final {
        if ( n->isWildcard() )
            out << "uint<*>";
        else
            out << fmt("uint<%d>", n->width());
    }

    void operator()(type::Tuple* n) final {
        if ( n->isWildcard() )
            out << "tuple<*>";
        else {
            out << "tuple<";

            auto types = node::transform(n->elements(), [](const auto& x) -> std::string {
                return x->id() ? fmt("%s: %s", *x->id(), *x->type()) : fmt("%s", *x->type());
            });

            out << util::join(types, ", ") << '>';
        }
    }

    void operator()(type::Vector* n) final {
        if ( n->isWildcard() )
            out << "vector<*>";
        else {
            out << "vector<" << n->elementType() << ">";
        }
    }

    void operator()(type::Void* n) final { out << "void"; }

    void operator()(type::WeakReference* n) final {
        if ( n->isWildcard() )
            out << "weak_ref<*>";
        else
            out << "weak_ref<" << n->dereferencedType() << ">";
    }

    void operator()(type::ValueReference* n) final {
        if ( n->isWildcard() )
            out << "value_ref<*>";
        else
            out << "value_ref<" << n->dereferencedType() << ">";
    }

private:
    printer::Stream& out;
};

} // anonymous namespace

void hilti::detail::printAST(const NodePtr& root, std::ostream& out, bool compact) {
    auto stream = printer::Stream(out, compact);
    printAST(root, stream);
}

void hilti::detail::printAST(const NodePtr& root, printer::Stream& stream) {
    util::timing::Collector _("hilti/printer");

    if ( auto t = root->tryAs<QualifiedType>() ) {
        if ( ! stream.isExpandSubsequentType() ) {
            if ( auto id = t->type()->typeID() ) {
                stream << *id;
                return;
            }
        }
    }

    if ( auto t = root->tryAs<UnqualifiedType>() ) {
        if ( ! stream.isExpandSubsequentType() ) {
            if ( auto id = t->typeID() ) {
                stream << *id;
                return;
            }
        }
    }

    for ( auto& p : plugin::registry().plugins() ) {
        if ( ! p.ast_print )
            continue;

        if ( (*p.ast_print)(root, stream) )
            return;
    }

    Printer(stream).dispatch(root);
}

std::string hilti::detail::renderOperatorPrototype(const NodeDerivedPtr<expression::ResolvedOperator>& o) {
    const auto& op = o->operator_();
    const auto& exprs = o->operands();

    switch ( op.kind() ) {
        case operator_::Kind::Call: {
            assert(exprs.size() == 2);
            auto id = exprs[0];
            auto ops = o->operator_().operands()[1].type->as<type::OperandList>()->operands();
            auto args =
                util::join(util::transform(ops, [&](auto x) { return fmt("<%s>", renderOperand(x, exprs)); }), ", ");
            return fmt("%s(%s)", id, args);
        }

        case operator_::Kind::MemberCall: {
            assert(exprs.size() == 3);
            auto self = exprs[0];
            auto id = exprs[1];
            auto ops = o->operator_().operands()[2].type->as<type::OperandList>()->operands();
            auto args =
                util::join(util::transform(ops, [&](auto x) { return fmt("<%s>", renderOperand(x, exprs)); }), ", ");
            return fmt("<%s>.%s(%s)", renderExpressionType(self), id, args);
        }

        default:
            return renderOperator(op.kind(), util::transform(op.operands(), [&](auto x) {
                                      return fmt("<%s>", renderOperand(x, exprs));
                                  }));
    }
}

static std::string _renderOperatorInstance(operator_::Kind kind, const node::Range<Expression>& exprs) {
    switch ( kind ) {
        case operator_::Kind::Call: {
            assert(exprs.size() == 2);
            const auto& id = exprs[0];
            auto ops = exprs[1]->as<expression::Ctor>()->ctor()->as<ctor::Tuple>()->value();
            auto args =
                util::join(node::transform(ops, [&](auto x) { return fmt("<%s>", renderExpressionType(x)); }), ", ");
            return fmt("%s(%s)", id, args);
        }

        case operator_::Kind::MemberCall: {
            assert(exprs.size() == 3);
            const auto& self = exprs[0];
            const auto& id = exprs[1];
            auto ops = exprs[2]->as<expression::Ctor>()->ctor()->as<ctor::Tuple>()->value();
            auto args =
                util::join(node::transform(ops, [&](auto x) { return fmt("<%s>", renderExpressionType(x)); }), ", ");
            return fmt("<%s>.%s(%s)", renderExpressionType(self), id, args);
        }

        default:
            return renderOperator(kind,
                                  node::transform(exprs, [&](auto x) { return fmt("<%s>", renderExpressionType(x)); }));
    }
}

std::string hilti::detail::renderOperatorInstance(const NodeDerivedPtr<expression::ResolvedOperator>& o) {
    return _renderOperatorInstance(o->operator_().kind(), o->operands());
}

std::string hilti::detail::renderOperatorInstance(const NodeDerivedPtr<expression::UnresolvedOperator>& o) {
    return _renderOperatorInstance(o->kind(), o->operands());
}
