/*******************************************************************************************
**  (C) Copyright September 2013 by
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
#include "backend/include/rule_factory/extension_aggregator.hpp"
using namespace DLS;

ConditionedList::ConditionedList(size_t threshold)
    : verge(threshold)
{
    //Nothing///
    //! Jesus rocks!! if you believe it, type YES!
    // -> YES - WhiZTiM
}


//! Adds extension in a very logical manner
void ConditionedList::add(const std::string& extension, Option opt)
{
    if(opt == Option::Inclusion)
    {
        // if we haven't reached our verge limit
        if(i_low)
        {
            i_vCon.push_back(extension);  //! add extension
            i_count++;                      // increase inclusion count

            if(i_count >= verge)            // if we have hit threshold
            {
                i_low = false;              // it is no more low
                if(!i_metamorphosed)        // just to make sure we do this only once
                {
                    for(auto& val : i_vCon) // copy out items
                    {
                        i_uCon.insert(val);
                    }
                    i_vCon.clear();         // free it
                    i_metamorphosed = true; // guard
                }
            }
        }

        else
        {
            //! unordered_set.insert returns a pair <iterator, bool>
            i_uCon.insert(extension).second ? ++i_count : i_count;
        }
    }

    else    //same as else if(opt == Option::Exclusion)
    {
        // if we haven't reached our verge limit
        if(e_low)
        {
            e_vCon.push_back(extension);  //! add extension
            e_count++;                      // increase exclusion count

            if(e_count >= verge)            // if we have hit threshold
            {
                e_low = false;              // it is no more low
                if(!e_metamorphosed)        // just to make sure we do this only once
                {
                    for(auto& val : e_vCon) // copy out items
                    {
                        e_uCon.insert(val);
                    }
                    e_vCon.clear();         // free it
                    e_metamorphosed = true; // guard
                }
            }
        }

        else
        {
            e_uCon.insert(extension).second ? ++e_count : e_count;
        }


    }
}

/*
void ConditionedList::add(const ConditionedList& conditionedList)
{

}
*/


bool ConditionedList::includes(const std::string& extension) const
{
    if(i_others)
        return true;

    if(i_metamorphosed)
        return i_uCon.find(extension) != i_uCon.end();

    //! for performance sake, I don't want to include std::find
    for(auto& val : i_vCon)
        if(val == extension)
            return true;

    return false;
}


bool ConditionedList::excludes(const std::string& extension) const
{
    if(e_others)
        return true;

    if(e_metamorphosed)
        return e_uCon.find(extension) != e_uCon.end();

    for(auto& val : e_vCon)
        if(val == extension)
            return true;

    return false;
}


bool ConditionedList::setEveryOther(Option opt, bool value)
{
    if(Option::Inclusion == opt)
        i_others = value;
    else
        e_others = value;

    return value;
}


bool ConditionedList::getEveryOther(Option opt)
{
    if(Option::Inclusion == opt)
        return i_others;
    return e_others;
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


Extension_Aggregator::Extension_Aggregator()
{
    //ctor
}

Extension_Aggregator::~Extension_Aggregator()
{
    //dtor
}
