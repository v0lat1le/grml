#include "ast.h"

#include <boost/config/warning_disable.hpp>

#include <boost/spirit/include/qi.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    grml::Employee,
    (int, age)
    (std::string, surname)
    (std::string, forename)
    (double, salary)
)

namespace grml
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    template <typename Iterator>
    struct Grammar : qi::grammar<Iterator, Employee(), ascii::space_type>
    {
        Grammar() : Grammar::base_type(start)
        {
            using qi::int_;
            using qi::lit;
            using qi::double_;
            using qi::lexeme;
            using ascii::char_;

            quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];

            start %=
                lit("employee")
                >> '{'
                >>  int_ >> ','
                >>  quoted_string >> ','
                >>  quoted_string >> ','
                >>  double_
                >>  '}'
                ;
        }

        qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
        qi::rule<Iterator, Employee(), ascii::space_type> start;
    };
}
