#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdexcept>
#include <string>

#include "header_defines.hpp"

namespace DLS
{

    class Exception : public std::exception
    {
        public:
            explicit Exception(const std::string& what)
                : _what(what)
            {
                //
            }
            explicit Exception(const char* what)
                : _what(what)
            {
                //
            }
            const char* what() const throw()
            {
                return _what.c_str();
            }
            virtual ~Exception() throw()
            {

            }
        private:
            const std::string _what;
    };
}
#endif // EXCEPTION_H
