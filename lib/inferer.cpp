#include "inferer.h"

#include <functional>
#include <unordered_map>

namespace
{
    using namespace grml;

    struct IdHasher
    {
        std::size_t operator()(const Identifier& id) const
        {
            return std::hash<std::string>{}(id.name);
        }
    };

    using Lookup = std::unordered_map<Identifier, Type, IdHasher>;

    struct LiteralInferer : boost::static_visitor<Type> {
        Type operator()(int) const { return BasicType::INT; }
        Type operator()(bool) const { return BasicType::BOOL; }
        Type operator()(double) const { return BasicType::REAL; }
    };

    struct DeclarationInferer : boost::static_visitor<std::pair<Identifier, Type> > {
        const Lookup& lookup;

        DeclarationInferer(const Lookup& l) : lookup(l) {}
        std::pair<Identifier, Type> operator()(const VariableDeclaration& d) const;
    };

    struct ExpressionInferer : boost::static_visitor<Type> {
        Lookup lookup;

        ExpressionInferer(Lookup l) : lookup(std::move(l)) {}

        Type operator()(Literal         const& e) const { return boost::apply_visitor(LiteralInferer(), e); }
        Type operator()(Identifier      const& e) const { return lookup.at(e); }
        Type operator()(UnaryOperation  const& e) const { return grml::infer(e.rhs); }
        Type operator()(BinaryOperation const& e) const { return TypeVariable(); }
        Type operator()(LetConstruct    const& e) const
        {
            auto scope = lookup;
            for (const auto& decl: e.declarations)
            {
                auto [ id, t ] = boost::apply_visitor(DeclarationInferer{scope}, decl);
                scope.insert_or_assign(std::move(id), std::move(t));
            }
            return boost::apply_visitor(ExpressionInferer(std::move(scope)), e.expression);
        }
    };

    std::pair<Identifier, Type> DeclarationInferer::operator()(const VariableDeclaration& d) const
    {
        auto t = boost::apply_visitor(ExpressionInferer(lookup), d.expression);
        return std::make_pair(d.identifier, t);
    }
}

namespace grml
{
    Type infer(const Expression& expr)
    {
      return boost::apply_visitor(ExpressionInferer(Lookup()), expr);
    }
}
