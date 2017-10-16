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


    using Constraint = std::pair<Type, Type>;
    using Constraints = std::vector<Constraint>;
    struct InferenceResult
    {
        Type type;
        Constraints constraints;

        InferenceResult(Type t) : type(std::move(t)) {}
        InferenceResult(Type t, Constraints cs) : type(std::move(t)), constraints(std::move(cs)) {}
    };
    using Lookup = std::unordered_map<Identifier, InferenceResult, IdHasher>;
    InferenceResult inferHelper(const Expression& expr, const Lookup& lookup);

    struct LiteralInferer : boost::static_visitor<Type> {
        Type operator()(int) const { return BasicType::INT; }
        Type operator()(bool) const { return BasicType::BOOL; }
        Type operator()(double) const { return BasicType::REAL; }
    };

    struct DeclarationInferer : boost::static_visitor<std::pair<Identifier, InferenceResult> > {
        const Lookup& lookup;

        DeclarationInferer(const Lookup& l) : lookup(l) {}
        std::pair<Identifier, InferenceResult> operator()(const VariableDeclaration& d) const;
        std::pair<Identifier, InferenceResult> operator()(const FunctionDeclaration& d) const;
    };

    struct ExpressionInferer : boost::static_visitor<InferenceResult>
    {
        using ResultType = ExpressionInferer::result_type;
        const Lookup& lookup;

        ExpressionInferer(const Lookup& l) : lookup(l) {}

        InferenceResult operator()(const Literal& e) const { return { boost::apply_visitor(LiteralInferer(), e) }; }
        InferenceResult operator()(const Identifier& e) const { return { lookup.at(e) }; }
        InferenceResult operator()(const UnaryOperation& e) const { return inferHelper(e.rhs, lookup); }
        InferenceResult operator()(const BinaryOperation& e) const { /* TODO: impl as f-call */ return inferHelper(e.lhs, lookup); }
        InferenceResult operator()(const LetConstruct& e) const
        {
            auto scope = lookup;
            for (const auto& decl: e.declarations)
            {
                auto [ id, t ] = boost::apply_visitor(DeclarationInferer{scope}, decl);
                scope.insert_or_assign(std::move(id), std::move(t));
            }
            return inferHelper(e.expression, std::move(scope));
        }
        InferenceResult operator()(const FunctionCall& e) const
        {
            TypeVariable result;

            FunctionType::Parameters params;
            Constraints constraints;
            for (const auto& arg: e.arguments)
            {
                auto p = inferHelper(arg, lookup);
                params.push_back(p.type);
                constraints.insert(constraints.end(), p.constraints.begin(), p.constraints.end());
            }
            auto rhs = FunctionType(result, params);
            auto lhs = lookup.at(e.name);
            constraints.insert(constraints.end(), lhs.constraints.begin(), lhs.constraints.end());
            constraints.emplace_back(lhs.type, rhs);

            return { result, constraints };
        }
    };

    std::pair<Identifier, InferenceResult> DeclarationInferer::operator()(const VariableDeclaration& d) const
    {
        return std::make_pair(d.name, inferHelper(d.expression, lookup));
    }

    std::pair<Identifier, InferenceResult> DeclarationInferer::operator()(const FunctionDeclaration& d) const
    {
        auto scope = lookup;
        FunctionType::Parameters params;
        for (const auto& param: d.parameters)
        {
            params.push_back(TypeVariable());
            scope.insert_or_assign(param, params.back());
        }
        auto result = inferHelper(d.expression, std::move(scope));
        return std::make_pair(d.name, InferenceResult{FunctionType(std::move(result.type), std::move(params)), std::move(result.constraints)});
    }

    InferenceResult inferHelper(const Expression& expr, const Lookup& lookup)
    {
        return boost::apply_visitor(ExpressionInferer(lookup), expr);
    }
}

namespace grml
{
    Type infer(const Expression& expr)
    {
        auto result = inferHelper(expr, Lookup());
        Substitution substitution;
        for (const auto& [lhs, rhs]: result.constraints)
        {
            auto unified = unify(substitute(lhs, substitution), substitute(rhs, substitution));
            substitution = combine(unified, substitution);
        }
        return substitute(result.type, substitution);
    }
}
