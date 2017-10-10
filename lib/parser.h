#pragma once


#include "ast.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

namespace grml
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    template <typename Iterator>
    struct Parser : qi::grammar<Iterator, Expression(), ascii::space_type>
    {
        Parser() : Parser::base_type(start)
        {
            qi::real_parser<double, qi::strict_real_policies<double> > strict_double;
            quoted_string %= qi::lexeme['"' >> +(ascii::char_ - '"') >> '"'];
            literal %= strict_double | qi::int_ | qi::bool_;

            start = literal >> ';';
        }

        qi::rule<Iterator, Literal, ascii::space_type> literal;
        qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
        qi::rule<Iterator, Expression(), ascii::space_type> start;
    };
}
