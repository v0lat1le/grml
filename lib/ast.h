#pragma once


#include <boost/variant.hpp>

namespace grml
{
    using Literal = boost::variant<int, double, bool>;
    using Expression = boost::variant<Literal>;
}
