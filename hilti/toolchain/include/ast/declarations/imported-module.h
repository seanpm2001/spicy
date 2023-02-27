// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/declaration.h>

namespace hilti {

class Unit;

namespace declaration {

/**
 * AST node for a declaration of imported module.
 *
 * We associate an explicit "parse extension" with an imported module that
 * specifies which plugin is to parse the code into an AST. Note that this does
 * *not* specify the semantics of the resulting AST. The imported AST will
 * always be processed by the same plugin that is in charge of the declaration
 * itself as well. This separation allows, for example, to import a piece of
 * HILTI source code into a Spicy AST.
 */
class ImportedModule : public Declaration {
public:
    hilti::rt::filesystem::path parseExtension() const { return _parse_extension; }

    const auto& path() const { return _path; }
    const auto& scope() const { return _scope; }
    const auto& searchDirectories() const { return _dirs; }
    auto unit() const { return _unit.lock(); }

    void setUnit(const std::shared_ptr<Unit>& unit) { _unit = unit; }

    std::string displayName() const final { return "imported module"; }

    node::Properties properties() const final {
        auto p = node::Properties{
            {"path", _path.native()},
            {"ext", _parse_extension.native()},
            {"scope", _scope ? _scope->str() : std::string("<n/a>")},
            {"dirs", util::join(_dirs)},
        };
        return Declaration::properties() + p;
    }

    static auto create(ASTContext* ctx, ID id, const std::string& parse_extension, Meta meta = {}) {
        return NodeDerivedPtr<ImportedModule>(
            new ImportedModule(std::move(id), {}, parse_extension, {}, {}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, ID id, const std::string& parse_extension, std::optional<ID> search_scope,
                       Meta meta = {}) {
        return DeclarationPtr(
            new ImportedModule(std::move(id), {}, parse_extension, std::move(search_scope), {}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, ID id, hilti::rt::filesystem::path path, Meta meta = {}) {
        return NodeDerivedPtr<ImportedModule>(
            new ImportedModule(std::move(id), std::move(path), {}, {}, {}, std::move(meta)));
    }

protected:
    ImportedModule(ID id, hilti::rt::filesystem::path path, const std::string& parse_extension,
                   std::optional<ID> search_scope, std::vector<hilti::rt::filesystem::path> search_dirs, Meta meta)
        : Declaration({}, std::move(id), Linkage::Private, std::move(meta)),
          _path(std::move(path)),
          _parse_extension(parse_extension),
          _scope(std::move(search_scope)),
          _dirs(std::move(search_dirs)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<ImportedModule>();
        if ( ! n )
            return false;

        return Declaration::isEqual(other) && _parse_extension == n->_parse_extension && _path == n->_path &&
               _scope == n->_scope && _dirs == n->_dirs;
    }

    HILTI_NODE(ImportedModule)

private:
    hilti::rt::filesystem::path _path;
    hilti::rt::filesystem::path _parse_extension;
    std::optional<ID> _scope;
    std::vector<hilti::rt::filesystem::path> _dirs;

    std::weak_ptr<hilti::Unit> _unit;
};

} // namespace declaration

} // namespace hilti
