#include "types.h"

std::atomic_int64_t grml::TypeVariable::counter{ 0 };

namespace
{
    using namespace grml;

    struct SubstitutionVisitor : boost::static_visitor<Type>
    {
        const Substitution& substitution;
        SubstitutionVisitor(const Substitution& s) : substitution(s) {}

        Type operator()(const BasicType& t) const
        {
            return t;
        }
        Type operator()(const TypeVariable& t) const
        {
            auto r = substitution.find(t);
            if (r != substitution.end()) return r->second;
            return t;
        }
        Type operator()(const FunctionType& t) const
        {
            FunctionType::Parameters params;
            for (const auto& p: t.parameters)
            {
                params.push_back(substitute(p, substitution));
            }
            return FunctionType(substitute(t.result, substitution), std::move(params));
        }
    };

    struct UnificationVisitor : boost::static_visitor<Substitution>
    {
        Substitution operator()(const FunctionType& lhs, const FunctionType& rhs) const
        {
            if (lhs.parameters.size() != rhs.parameters.size()) throw std::runtime_error("curry is not supported");

            Substitution params;
            for (std::size_t i = 0; i < lhs.parameters.size(); ++i)
            {
                auto unified = unify(lhs.parameters[i], rhs.parameters[i]);
                params.insert(unified.begin(), unified.end());
            }
            Substitution body = unify(rhs.result, substitute(lhs.result, params));

            return body;
        }

        template<typename LHST, typename RHST>
        Substitution operator()(const LHST& lhs, const RHST& rhs) const
        {
            if constexpr (std::is_same_v<LHST, RHST>)
            {
                if (lhs == rhs) return {};
            }
            if constexpr (std::is_same_v<LHST, TypeVariable>)
            {
                return { { lhs, rhs } };
            }
            if constexpr (std::is_same_v<LHST, BasicType>)
            {
                return {};
            }
            else
            {
                throw std::runtime_error("incompatible types");
            }
        }
    };
}

grml::Type grml::substitute(const grml::Type& type, const grml::Substitution& substitution)
{
    return boost::apply_visitor(SubstitutionVisitor(substitution), type);
}

grml::Substitution grml::combine(const grml::Substitution& lhs, const grml::Substitution& rhs)
{
    auto result = lhs;
    for (const auto& [tv, t]: rhs) {
        result.insert_or_assign(tv, substitute(t, lhs));
    }
    return result;
}

grml::Substitution grml::unify(const grml::Type& lhs, const grml::Type& rhs)
{
    return boost::apply_visitor(UnificationVisitor(), lhs, rhs);
}
