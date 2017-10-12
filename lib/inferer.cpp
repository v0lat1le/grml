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

    Type inferHelper(const Expression& expr, const Lookup& lookup);

    Type convertTypes(const Type& lhs, const Type& rhs)
    {
        if (lhs == rhs) return lhs;
        
        //TODO: can of worms
        return TypeVariable();
    }

    struct LiteralInferer : boost::static_visitor<Type> {
        Type operator()(int) const { return BasicType::INT; }
        Type operator()(bool) const { return BasicType::BOOL; }
        Type operator()(double) const { return BasicType::REAL; }
    };

    struct DeclarationInferer : boost::static_visitor<std::pair<Identifier, Type> > {
        const Lookup& lookup;

        DeclarationInferer(const Lookup& l) : lookup(l) {}
        std::pair<Identifier, Type> operator()(const VariableDeclaration& d) const;
        std::pair<Identifier, Type> operator()(const FunctionDeclaration& d) const;
    };

    struct ExpressionInferer : boost::static_visitor<Type> {
        const Lookup& lookup;

        ExpressionInferer(const Lookup& l) : lookup(l) {}

        Type operator()(Literal         const& e) const { return boost::apply_visitor(LiteralInferer(), e); }
        Type operator()(Identifier      const& e) const { return lookup.at(e); }
        Type operator()(UnaryOperation  const& e) const { return inferHelper(e.rhs, lookup); }
        Type operator()(BinaryOperation const& e) const { return convertTypes(inferHelper(e.lhs, lookup), inferHelper(e.rhs, lookup)); }
        Type operator()(LetConstruct    const& e) const
        {
            auto scope = lookup;
            for (const auto& decl: e.declarations)
            {
                auto [ id, t ] = boost::apply_visitor(DeclarationInferer{scope}, decl);
                scope.insert_or_assign(std::move(id), std::move(t));
            }
            return inferHelper(e.expression, std::move(scope));
        }
        Type operator()(FunctionCall    const& e) const
        {
            return TypeVariable();
        }
    };

    std::pair<Identifier, Type> DeclarationInferer::operator()(const VariableDeclaration& d) const
    {
        return std::make_pair(d.identifier, inferHelper(d.expression, lookup));
    }

    std::pair<Identifier, Type> DeclarationInferer::operator()(const FunctionDeclaration& d) const
    {
        return std::make_pair(d.name, FunctionType());
    }

    Type inferHelper(const Expression& expr, const Lookup& lookup)
    {
        return boost::apply_visitor(ExpressionInferer(lookup), expr);
    }
}

namespace grml
{
    Type infer(const Expression& expr)
    {
      return inferHelper(expr, Lookup());
    }
}
