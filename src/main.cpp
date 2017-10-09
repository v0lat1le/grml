#include "grammar.h"

int main(int, char**)
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tAn employee parser for Spirit...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";

    std::cout
        << "Give me an employee of the form :"
        << "employee{age, \"surname\", \"forename\", salary } \n";
    std::cout << "Type [q or Q] to quit\n\n";

    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef grml::Grammar<iterator_type> employee_parser;

    employee_parser g; // Our grammar
    std::string str;
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        grml::Employee emp;
        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = phrase_parse(iter, end, g, space, emp);

        if (r && iter == end)
        {
            std::cout << boost::fusion::tuple_open('[');
            std::cout << boost::fusion::tuple_close(']');
            std::cout << boost::fusion::tuple_delimiter(", ");

            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "got: " << boost::fusion::as_vector(emp) << std::endl;
            std::cout << "\n-------------------------\n";
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
