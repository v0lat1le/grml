#define BOOST_TEST_MODULE types test suite
#define BOOST_TEST_DYN_LINK

#include "types.h"

#include <boost/test/unit_test.hpp>
#include <sstream>


BOOST_AUTO_TEST_CASE(test_printing)
{
    using namespace grml;

    auto to_string = [](const auto& t)
    {
        std::ostringstream ss;
        ss << t;
        return ss.str();
    };

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
    using namespace grml;

    TypeVariable a;

    BOOST_TEST(substitute(BasicType::REAL, { { a, BasicType::INT } } ) == Type(BasicType::REAL));
    BOOST_TEST(substitute(a, { { a, BasicType::INT } } ) == Type(BasicType::INT));
    BOOST_TEST(substitute(a, {}) == Type(a));

    BOOST_TEST(
        substitute(FunctionType(a, { a }), { { a, BasicType::INT } }) ==
        Type(FunctionType(BasicType::INT, { BasicType::INT }))
    );
}

BOOST_AUTO_TEST_CASE(test_unify)
{
    using namespace grml;

    TypeVariable a, b, c;

    BOOST_TEST(unify(a, a) == Substitution());
    BOOST_TEST(unify(a, b) == (Substitution{ { a, b } }));
    BOOST_TEST(unify(a, BasicType::INT) == (Substitution{ { a, BasicType::INT } }));
    BOOST_TEST(unify(BasicType::BOOL, b) == (Substitution{ { b, BasicType::BOOL } }));

    BOOST_TEST(
        unify(FunctionType(a, { a }), FunctionType(b, { BasicType::INT })) ==
        (Substitution{ { a, BasicType::INT }, { b, BasicType::INT } })
    );

    BOOST_TEST(
        unify(FunctionType(a, { a }), FunctionType(b, { c })) ==
        (Substitution{ { a, b }, { c, b } })
    );

    BOOST_CHECK_THROW(unify(BasicType::INT, BasicType::BOOL), std::runtime_error);
}


BOOST_AUTO_TEST_CASE(test_combine)
{
    grml::TypeVariable a, b, c, d;

    grml::Substitution lhs{
        { a, grml::FunctionType(c, { grml::BasicType::INT }) },
        { b, grml::BasicType::INT } };
    grml::Substitution rhs{
        { d, grml::FunctionType(grml::BasicType::INT, { a }) },
        { b, grml::BasicType::INT } };
    grml::Substitution combined{
        { a, grml::FunctionType(c, { grml::BasicType::INT }) },
        { b, grml::BasicType::INT },
        { d, grml::FunctionType(grml::BasicType::INT, { grml::FunctionType(c, { grml::BasicType::INT }) }) } };

    BOOST_TEST(grml::combine(lhs, rhs) == combined);
}
