#include "inferer.h"

#include <functional>
#include <unordered_map>

namespace
{
    using namespace grml;

    struct LiteralInferer : boost::static_visitor<Type> {
        Type operator()(int) const { return BasicType::INT; }
        Type operator()(bool) const { return BasicType::BOOL; }
        Type operator()(double) const { return BasicType::REAL; }
    };

    struct DeclarationInferer : boost::static_visitor<std::pair<Identifier, Type> > {
        const Environment& env;

        DeclarationInferer(const Environment& e) : env(e) {}
        std::pair<Identifier, Type> operator()(const VariableDeclaration& d) const;
        std::pair<Identifier, Type> operator()(const FunctionDeclaration& d) const;
    };

    struct ExpressionInferer : boost::static_visitor<Type>
    {
        using ResultType = ExpressionInferer::result_type;
        const Environment& env;

        ExpressionInferer(const Environment& e) : env(e) {}

        Type operator()(const Literal& e) const { return { boost::apply_visitor(LiteralInferer(), e) }; }
        Type operator()(const Identifier& e) const { return { env.at(e) }; }
        Type operator()(const UnaryOperation& e) const { return infer(e.rhs, env); }
        Type operator()(const BinaryOperation& e) const { /* TODO: impl as f-call */ return infer(e.lhs, env); }
        Type operator()(const LetConstruct& e) const
        {
            auto scope = env;
            for (const auto& decl: e.declarations)
            {
                auto [ id, t ] = boost::apply_visitor(DeclarationInferer(scope), decl);
                scope.insert_or_assign(std::move(id), std::move(t));
            }
            return infer(e.expression, std::move(scope));
        }
        Type operator()(const IfConstruct& e) const
        {
            auto testSub = unify(infer(e.test, env), BasicType::BOOL);
            auto whenTrue = substitute(infer(e.whenTrue, env), testSub);
            auto whenFalse = substitute(infer(e.whenFalse, env), testSub);
            auto bodySub = unify(whenTrue, whenFalse);
            return substitute(whenTrue, bodySub);
        }
        Type operator()(const FunctionCall& e) const
        {
            FunctionType::Parameters params;
            for (const auto& arg: e.arguments)
            {
                auto p = infer(arg, env);
                params.push_back(p);
            }
            auto rhs = FunctionType(TypeVariable(), params);
            auto lhs = infer(e.name, env);
            auto substitution = unify(lhs, rhs);
            auto result = substitute(lhs, substitution);
            return boost::get<FunctionType>(result).result;
        }
    };

    std::pair<Identifier, Type> DeclarationInferer::operator()(const VariableDeclaration& d) const
    {
        return std::make_pair(d.name, infer(d.expression, env));
    }

    std::pair<Identifier, Type> DeclarationInferer::operator()(const FunctionDeclaration& d) const
    {
        auto scope = env;
        FunctionType::Parameters params;
        for (const auto& param: d.parameters)
        {
            params.push_back(TypeVariable());
            scope.insert_or_assign(param, params.back());
        }
        FunctionType self = FunctionType(TypeVariable(), std::move(params));
        scope.insert_or_assign(d.name, self);
        auto result = infer(d.expression, std::move(scope));
        
        return std::make_pair(d.name, FunctionType(std::move(result), std::move(self.parameters)));
    }
}

std::size_t grml::detail::IdHasher::operator()(const grml::Identifier& id) const
{
    return std::hash<std::string>{}(id.name);
}

namespace grml
{
    Type infer(const Expression& expr, const Environment& env)
    {
        return boost::apply_visitor(ExpressionInferer(env), expr);
    }
}
