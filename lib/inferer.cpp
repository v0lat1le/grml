#include "inferer.h"

namespace
{
    using namespace grml;

    struct LiteralInferer : boost::static_visitor<Type> {
        Type operator()(int) const { return BasicType::INT; }
        Type operator()(bool) const { return BasicType::BOOL; }
        Type operator()(double) const { return BasicType::REAL; }
    };

    struct ExpressionInferer : boost::static_visitor<Type> {
        Type operator()(Literal         const& e) const { return boost::apply_visitor(LiteralInferer(), e); }
        Type operator()(Identifier      const& e) const { return TypeVariable(); }
        Type operator()(UnaryOperation  const& e) const { return TypeVariable(); }
        Type operator()(BinaryOperation const& e) const { return TypeVariable(); }
        Type operator()(LetConstruct    const& e) const { return TypeVariable(); }
    };
}

namespace grml
{
    Type infer(const Expression& expr)
    {
      return boost::apply_visitor(ExpressionInferer(), expr);
    }
}
