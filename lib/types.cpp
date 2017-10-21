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
            return params;
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
            if constexpr (std::is_same_v<RHST, TypeVariable>)
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

grml::Substitution grml::unify(const grml::Type& lhs, const grml::Type& rhs)
{
    return boost::apply_visitor(UnificationVisitor(), lhs, rhs);
}

grml::Substitution& grml::combine(grml::Substitution& lhs, grml::Substitution&& rhs)
{
    for (auto&[tv, t] : rhs) {
        auto subd = substitute(std::move(t), lhs);
        auto pos = lhs.find(tv);
        if (pos != lhs.end())
        {
            combine(lhs, unify(pos->second, subd));
        }
        else
        {
            lhs.insert({ tv, std::move(subd) });
        }
    }
    return lhs;
}

std::ostream& grml::operator<<(std::ostream& os, const grml::BasicType& t)
{
    switch (t)
    {
        case BasicType::BOOL:
            os << "bool";
            break;
        case BasicType::INT:
            os << "int";
            break;
        case BasicType::REAL:
            os << "real";
            break;
    }
    return os;
}

std::ostream& grml::operator<<(std::ostream& os, const grml::TypeVariable& t)
{
    os << static_cast<char>('a'+t.id/10) << t.id%10;
    return os;
}

std::ostream& grml::operator<<(std::ostream& os, const FunctionType& t)
{
    os << "(";
    for (const auto& p: t.parameters)
        os << p << "->";
    os << t.result << ")";
    return os;
}
