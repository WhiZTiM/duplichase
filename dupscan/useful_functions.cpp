/*******************************************************************************************
**  (C) Copyright September 2013 - October 2013 by
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
#include <boost/algorithm/string.hpp>
#include "useful_functions.hpp"

//using namespace DLS;
using namespace boost;

    bool DLS::isTrackableTo(const std::string& child, const std::string& parent)
    {
        std::vector<std::string> vParent, vChild;
        vParent = algorithm::split(vParent, parent, algorithm::is_any_of("/"), algorithm::token_compress_on);
        vChild = algorithm::split(vChild, child, algorithm::is_any_of("/"), algorithm::token_compress_on);

        if(vParent.size() >= vChild.size())
            return false;

        std::size_t k(0);
        while(k < vParent.size())
        {
            if(vParent[k] != vChild[k])
                return false;
            ++k;
        }

        return (k == vParent.size());
    }


    /*/! A blawdy O(n^2) algorithm
    std::vector<std::string>
    overlappingPathVector(const std::vector<std::list>& newList, const std::vector<std::string>& oldList)
    {
        std::vector<std::string> rtn;

    }
    */
