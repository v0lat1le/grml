#define BOOST_TEST_MODULE types test suite
#define BOOST_TEST_DYN_LINK

#include "types.h"

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_printing)
{
    using namespace grml;
    auto to_string = [](const auto& t) { return boost::lexical_cast<std::string>(t); };

    BOOST_TEST(to_string(BasicType::INT) == "int");
    BOOST_TEST(to_string(BasicType::BOOL) == "bool");
    BOOST_TEST(to_string(BasicType::REAL) == "real");
    BOOST_TEST(to_string(TypeVariable(2)) == "a2");
    BOOST_TEST(to_string(TypeVariable(12)) == "b2");
    BOOST_TEST(to_string(TypeVariable(252)) == "z2");
    BOOST_TEST(to_string(FunctionType(BasicType::INT, { })) == "(int)");
    BOOST_TEST(to_string(FunctionType(BasicType::INT, { BasicType::BOOL })) == "(bool->int)");
    BOOST_TEST(to_string(FunctionType(BasicType::INT, { BasicType::BOOL, BasicType::REAL })) == "(bool->real->int)");

    BOOST_TEST(to_string(Type(BasicType::INT)) == "int");
}

BOOST_AUTO_TEST_CASE(test_substitute)
{
    auto a = grml::TypeVariable();

    BOOST_TEST(grml::substitute(grml::BasicType::REAL, { { a, grml::BasicType::INT } } ) == grml::Type(grml::BasicType::REAL));
    BOOST_TEST(grml::substitute(a, { { a, grml::BasicType::INT } } ) == grml::Type(grml::BasicType::INT));
    BOOST_TEST(grml::substitute(a, {}) == grml::Type(a));

    BOOST_TEST(grml::substitute(grml::FunctionType(a, { a }), { { a, grml::BasicType::INT } }) ==
        grml::Type(grml::FunctionType(grml::BasicType::INT, { grml::BasicType::INT }))
    );
}

BOOST_AUTO_TEST_CASE(test_unify)
{
    grml::TypeVariable a, b;

    BOOST_TEST(grml::unify(a, a) == grml::Substitution());
    BOOST_TEST(grml::unify(a, b) == (grml::Substitution{ { a, b } }));
    BOOST_TEST(grml::unify(a, grml::BasicType::INT) == (grml::Substitution{ { a, grml::BasicType::INT } }));
    BOOST_TEST(grml::unify(grml::BasicType::BOOL, b) == grml::Substitution());

    BOOST_TEST(grml::unify(grml::FunctionType(a, { a }), grml::FunctionType(b, { grml::BasicType::INT })) ==
        (grml::Substitution{ { a, grml::BasicType::INT } })
    );

    BOOST_CHECK_THROW(grml::unify(grml::BasicType::INT, grml::BasicType::BOOL), std::runtime_error);
}
