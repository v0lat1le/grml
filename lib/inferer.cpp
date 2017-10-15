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

        Type operator()(const Literal& e) const { return boost::apply_visitor(LiteralInferer(), e); }
        Type operator()(const Identifier& e) const { return lookup.at(e); }
        Type operator()(const UnaryOperation& e) const { return inferHelper(e.rhs, lookup); }
        Type operator()(const BinaryOperation& e) const { return convertTypes(inferHelper(e.lhs, lookup), inferHelper(e.rhs, lookup)); }
        Type operator()(const LetConstruct& e) const
        {
            auto scope = lookup;
            for (const auto& decl: e.declarations)
            {
                auto [ id, t ] = boost::apply_visitor(DeclarationInferer{scope}, decl);
                scope.insert_or_assign(std::move(id), std::move(t));
            }
            return inferHelper(e.expression, std::move(scope));
        }
        Type operator()(const FunctionCall& e) const
        {
            TypeVariable result;

            FunctionType::Parameters params;
            for (const auto& arg: e.arguments)
            {
                params.push_back(inferHelper(arg, lookup));
            }
            auto rhs = FunctionType(result, params);
            auto lhs = lookup.at(e.name);

            // TODO: return constraint lhs = rhs
            auto substitution = unify(lhs, rhs);
            return substitute(result, substitution);
        }
    };

    std::pair<Identifier, Type> DeclarationInferer::operator()(const VariableDeclaration& d) const
    {
        return std::make_pair(d.name, inferHelper(d.expression, lookup));
    }

    std::pair<Identifier, Type> DeclarationInferer::operator()(const FunctionDeclaration& d) const
    {
        auto scope = lookup;
        FunctionType::Parameters params;
        for (const auto& param: d.parameters)
        {
            params.push_back(TypeVariable());
            scope.insert_or_assign(param, params.back());
        }
        Type result = inferHelper(d.expression, std::move(scope));
        return std::make_pair(d.name, FunctionType(std::move(result), std::move(params)));
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
