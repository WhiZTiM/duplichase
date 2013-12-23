/*******************************************************************************************
**  (C) Copyright July 2013 by
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
#ifndef PATHHELPER_H
#define PATHHELPER_H

#include "path.hpp"
#include <string>

namespace DLS
{

class PathHelper
{
    public:
        PathHelper();
        ~PathHelper();
    protected:
    private:
};

class PathListMaker
{
    public:
    PathListMaker();

    bool addPath(const std::string path);


    std::list<Path> getPathList() const;

    ~PathListMaker();
    private:

    std::list<Path> _paths;
};

}
#endif // PATHHELPER_H
