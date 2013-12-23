/*******************************************************************************************
**  (C) Copyright August 2013 by
**  @author: Ibrahim Timothy Onogu {WhiZTiM}
**  @email: <ionogu@acm.org>
**
**	Provided this copyright notice appears on all derived works;
**  Use, modification and distribution are subject to the Boost Software License,
**  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
**  http://www.boost.org/LICENSE_1_0.txt).
**
**  Project DupLichaSe...2013
**  See http://github.com/WhiZTiM/duplichase for most recent version including documentation.
********************************************************************************************/
#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdexcept>
#include <string>

#include "header_defines.hpp"

typedef unsigned long ulong;

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
