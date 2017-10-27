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

    struct InferenceResult
    {
        InferenceResult(Type t) : type(std::move(t)) {}
        InferenceResult(Type t, Substitution s) : type(std::move(t)), substitution(std::move(s)) {}

        Type type;
        Substitution substitution;
    };

    InferenceResult inferImpl(const Expression& expr, const Environment& env);

    struct DeclarationInferer : boost::static_visitor<std::pair<Identifier, InferenceResult> > {
        const Environment& env;

        DeclarationInferer(const Environment& e) : env(e) {}
        std::pair<Identifier, InferenceResult> operator()(const VariableDeclaration& d) const;
        std::pair<Identifier, InferenceResult> operator()(const FunctionDeclaration& d) const;
    };

    struct ExpressionInferer : boost::static_visitor<InferenceResult>
    {
        using ResultType = ExpressionInferer::result_type;
        const Environment& env;

        ExpressionInferer(const Environment& e) : env(e) {}

        InferenceResult operator()(const Literal& e) const { return { boost::apply_visitor(LiteralInferer(), e) }; }
        InferenceResult operator()(const Identifier& e) const { return { env.at(e) }; }
        InferenceResult operator()(const UnaryOperation& e) const { return inferImpl(e.rhs, env); }
        InferenceResult operator()(const BinaryOperation& e) const { /* TODO: impl as f-call */ return inferImpl(e.lhs, env); }
        InferenceResult operator()(const LetConstruct& e) const
        {
            auto scope = env;
            for (const auto& decl: e.declarations)
            {
                auto [ id, t ] = boost::apply_visitor(DeclarationInferer(scope), decl);
                scope.insert_or_assign(std::move(id), std::move(t.type));
            }
            return inferImpl(e.expression, std::move(scope));
        }
        InferenceResult operator()(const IfConstruct& e) const
        {
            auto [test, substitution] = inferImpl(e.test, env);
            substitution = combine(unify(test, BasicType::BOOL), substitution);
            auto [whenTrue, tSub] = inferImpl(e.whenTrue, env);
            auto [whenFalse, fSub] = inferImpl(e.whenFalse, env);
            substitution = combine(tSub, substitution);
            substitution = combine(fSub, substitution);

            substitution = combine(unify(whenTrue, whenFalse), substitution);
            return { substitute(whenTrue, substitution), substitution };
        }
        InferenceResult operator()(const FunctionCall& e) const
        {
            Substitution substitution;
            FunctionType::Parameters params;
            for (const auto& arg: e.arguments)
            {
                auto [t, s] = inferImpl(arg, env);
                params.push_back(std::move(t));
                substitution = combine(s, substitution);
            }
            TypeVariable a;
            auto rhs = FunctionType(a, params);
            auto scope = env;
            for (auto& [id, t]: scope)
            {
                t = substitute(t, substitution);
            }
            const auto& [lhs, s] = inferImpl(e.name, scope);
            substitution = combine(s, substitution);
            auto unifier = unify(substitute(lhs, s), rhs);
            auto pos = unifier.find(a);
            return { pos == unifier.end() ? a : pos->second, combine(unifier, substitution) };
        }
    };

    std::pair<Identifier, InferenceResult> DeclarationInferer::operator()(const VariableDeclaration& d) const
    {
        const auto& [t, s] = inferImpl(d.expression, env);
        auto sub = unify(d.type, t);
        return { d.name, { substitute(t, sub), combine(sub, s) } };
    }

    std::pair<Identifier, InferenceResult> DeclarationInferer::operator()(const FunctionDeclaration& d) const
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
        auto body = inferImpl(d.expression, std::move(scope));
        
        self.result = std::move(body.type);
        auto result = substitute(self, body.substitution);
        return { d.name, { result, std::move(body.substitution) } };
    }

    InferenceResult inferImpl(const Expression& expr, const Environment& env)
    {
        return boost::apply_visitor(ExpressionInferer(env), expr);
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
        auto[t, s] = inferImpl(expr, env);
        return substitute(t, s);
    }
}
