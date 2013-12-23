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
#ifndef SAMPLE_FILEPROPERTY_HPP
#define SAMPLE_FILEPROPERTY_HPP

#include "backend/include/container_helpers/fp_holders.hpp"
#include <vector>

std::vector<DLS::FileProperty> sampleFileProperties()
{
    std::vector<DLS::FileProperty> rtn;

    rtn.push_back( DLS::FileProperty() );
    rtn.back().setSize(432453);
    rtn.back().setFilePath("/home/whiztim/Documents/developer");

    rtn.push_back( DLS::FileProperty() );
    rtn.back().setSize(654653255);
    rtn.back().setFilePath("/home/whiztim/Documents/developer/C++/Qt/eret/343533/eretdgerererer/ erere ergegergerr/33433ertrt/rgrfgfgd/fdg/er/ertrertegfdgd/dfgrthtrteer");

    rtn.push_back( DLS::FileProperty() );
    rtn.back().setSize(45465325);
    rtn.back().setFilePath("/home/whiztim/Documents/developer/C++/Qt");

    return rtn;
}

DLS::DuplicatesContainer sampleDuplicateContainer()
{
    DLS::DuplicatesContainer rtn;
    DLS::ptrVEC_FileProperty prop1( new DLS::VEC_FileProperty );
    DLS::ptrVEC_FileProperty prop2( new DLS::VEC_FileProperty );

    prop1->push_back( DLS::FileProperty() );
    prop1->back().setSize( 2355642 );
    prop1->back().setFilePath("/home/whiztim/Documents/test/kppt.out");

    prop1->push_back( DLS::FileProperty() );
    prop1->back().setSize( 2355642 );
    prop1->back().setFilePath("/home/whiztim/Downloads/test2/kppt.out");


    prop2->push_back( DLS::FileProperty() );
    prop2->back().setSize( 653243232 );
    prop2->back().setFilePath("/home/whiztim/Documents/recors/tvt.out");

    prop2->push_back( DLS::FileProperty() );
    prop2->back().setSize( 653243232 );
    prop2->back().setFilePath("/home/whiztim/Music/wow/ohmy.tre");

    rtn.insert( prop1 );
    rtn.insert( prop2 );

    return rtn;
}

#endif // SAMPLE_FILEPROPERTY_HPP
