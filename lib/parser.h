#pragma once

#include "ast.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace grml
{
    namespace qi = boost::spirit::qi;
    namespace phx   = boost::phoenix;
    namespace ascii = boost::spirit::ascii;

    template <typename Iterator>
    struct Parser : qi::grammar<Iterator, Expression(), ascii::space_type>
    {
        using ExprRule = qi::rule<Iterator, Expression(), ascii::space_type>;

        Parser() : Parser::base_type(start)
        {
            qi::real_parser<double, qi::strict_real_policies<double> > strict_double;
            quoted_string %= qi::lexeme['"' >> +(ascii::char_ - '"') >> '"'];
            literal = strict_double | qi::int_ | qi::bool_;
            identifier = qi::as_string[qi::lexeme[ascii::char_("a-zA-Z") >> *ascii::char_("0-9a-zA-Z")]];
            simple = '(' >> expression >> ')' | literal | letConstruct | ifConstruct | funcCall | identifier;
            valueDecl = ("val" >> identifier >> "=" >> expression)[qi::_val = phx::construct<VariableDeclaration>(qi::_1, qi::_2)];
            
            funcParams = -(identifier % ",");
            funcDecl = ("fun" >> identifier >> "(" >> funcParams >> ")" >> "=" >> expression)[qi::_val = phx::construct<FunctionDeclaration>(qi::_1, qi::_2, qi::_3)];
            declaration = valueDecl | funcDecl;
            letConstruct = ("let" >> *declaration >> "in" >> expression >> "end")[qi::_val = phx::construct<LetConstruct>(qi::_1, qi::_2)];
            ifConstruct = ("if" >> expression >> "then" >> expression >> "else" >> expression)[qi::_val = phx::construct<IfConstruct>(qi::_1, qi::_2, qi::_3)];

            addSubOp.add
                ("+", BinaryOperator::ADD)
                ("-", BinaryOperator::SUBTRACT);
            
            mulDivModOp.add
                ("*", BinaryOperator::MULTIPLY)
                ("/", BinaryOperator::DIVIDE)
                ("%", BinaryOperator::MODULO);

            mulDivMod = simple[qi::_val = qi::_1] >> -(mulDivModOp >> mulDivMod)[qi::_val = phx::construct<BinaryOperation>(qi::_1, qi::_val, qi::_2)];
            addSub = mulDivMod[qi::_val = qi::_1] >> -(addSubOp >> addSub)[qi::_val = phx::construct<BinaryOperation>(qi::_1, qi::_val, qi::_2)];
            
            funcArgs = -(expression % ",");
            funcCall = (identifier >> "(" >> funcArgs >> ")")[qi::_val = phx::construct<FunctionCall>(qi::_1, qi::_2)];

            expression = addSub;
            start = expression >> ';';
        }
        qi::symbols<char, BinaryOperator> addSubOp, mulDivModOp;
        qi::rule<Iterator, Literal, ascii::space_type> literal;
        qi::rule<Iterator, Identifier, ascii::space_type> identifier;
        qi::rule<Iterator, VariableDeclaration, ascii::space_type> valueDecl;
        qi::rule<Iterator, std::vector<Identifier>(), ascii::space_type> funcParams;
        qi::rule<Iterator, FunctionDeclaration, ascii::space_type> funcDecl;
        qi::rule<Iterator, Declaration, ascii::space_type> declaration;
        qi::rule<Iterator, std::vector<Expression>(), ascii::space_type> funcArgs;
        ExprRule simple, letConstruct, ifConstruct, addSub, mulDivMod, funcCall, expression, start;
    
        qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
    };
}
