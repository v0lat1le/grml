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
