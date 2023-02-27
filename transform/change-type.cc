
// clang-tidy --load libchange-type.dylib --checks=-*,spicy-* --list-checks

#include <utility>

#include "clang-tidy/ClangTidyCheck.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"
#include "clang-tidy/utils/TransformerClangTidyCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/Transformer/RangeSelector.h"
#include "clang/Tooling/Transformer/RewriteRule.h"
#include "clang/Tooling/Transformer/SourceCode.h"
#include "clang/Tooling/Transformer/Stencil.h"
#include "clang/Tooling/Transformer/Transformer.h"

using namespace clang::ast_matchers;
using namespace clang::tidy;
using namespace clang::tidy::utils;
using namespace clang;
using namespace transformer;

std::string replace(std::string& str, const std::string& from, const std::string& to) {
    auto i = str.find(from);
    if ( i != std::string::npos )
        return str.replace(i, from.size(), to);
    else
        return str;
}

// Adapted from clang's cat(), with all error handling removed.
class SearchAndReplaceStencil : public StencilInterface {
    RangeSelector Selector;

public:
    explicit SearchAndReplaceStencil(const std::string& node_, std::string from, std::string to)
        : Selector(node(node_)), from(std::move(from)), to(std::move(to)) {}

    std::string toString() const override { return "replace(...)"; }

    llvm::Error eval(const MatchFinder::MatchResult& Match, std::string* Result) const override {
        auto RawRange = Selector(Match);
        CharSourceRange Range = Lexer::makeFileCharRange(*RawRange, *Match.SourceManager, Match.Context->getLangOpts());

        auto txt = tooling::getText(Range, *Match.Context).str();
        txt = replace(txt, from, to);
        *Result += txt;
        return llvm::Error::success();
    }

    std::string from;
    std::string to;
};

Stencil replace(const std::string& node, const std::string& from, const std::string& to) {
    return std::make_shared<SearchAndReplaceStencil>(node, from, to);
}

RewriteRuleWith<std::string> ChangeTypeRule() {
    auto from_type_full = "class Type";
    auto from_type_short = "Type";
    auto to = "TypePtr";

    auto t1 = hasType(asString(from_type_full));
    auto t2 = hasType(references(recordDecl(hasName(from_type_short))));
    auto t3 = hasType(recordDecl(hasName(from_type_short)));

    // clang-format off
    auto r1 = makeRule(
                varDecl(t1).bind("decl"),
                changeTo(node("decl"), replace("decl", from_type_short, to)),
                cat("Change type"));

    auto r2 = makeRule(
                varDecl(t2).bind("decl"),
                changeTo(node("decl"), replace("decl", from_type_short, to)),
                cat("Change type"));

    auto r3 = makeRule(
                memberExpr(hasDescendant(declRefExpr(t1))).bind("access"),
                changeTo(node("access"), replace("access", ".", "->")),
                cat("Change access"));

    auto r4 = makeRule(
                memberExpr(hasDescendant(declRefExpr(t3))).bind("access"),
                changeTo(node("access"), replace("access", ".", "->")),
                cat("Change access"));

    // clang-format on
    return applyFirst({r1, r2, r3, r4});
}

class ChangeTypeCheck : public TransformerClangTidyCheck {
public:
    ChangeTypeCheck(StringRef Name, ClangTidyContext* Context)
        : TransformerClangTidyCheck(ChangeTypeRule(), Name, Context) {}
};

class SpicyRefactorModule : public ClangTidyModule {
public:
    void addCheckFactories(ClangTidyCheckFactories& CheckFactories) override {
        CheckFactories.registerCheck<ChangeTypeCheck>("spicy-change-type");
    }
};

static ClangTidyModuleRegistry::Add<SpicyRefactorModule> spicy_refactor_module("spicy-refactor-module",
                                                                               "Custom Spicy refactoring checks.");
