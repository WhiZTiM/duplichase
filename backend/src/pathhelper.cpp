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
#include "backend/include/pathhelper.hpp"

using namespace DLS;

PathHelper::PathHelper()
{
    //ctor
}

PathHelper::~PathHelper()
{
    //dtor
}


PathListMaker::PathListMaker()
{

}

bool PathListMaker::addPath(const std::string path)
{
    Path _pt(path);
    _paths.push_back(_pt);
    return _pt.exists();
}
