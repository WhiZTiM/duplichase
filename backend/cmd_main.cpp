/*******************************************************************************************
**  (C) Copyright August 2013 - September 2013 by
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
//! THIS FILE WAS Supposed to be for the CommandLine Non-Qt version!!
#include <iostream>
#include <string>
#include "backend/include/path.hpp"
#include "backend/include/rule_facade/rule_facade.hpp"
//#include "backend/include/test/testsuite_1.hpp"
//#include "backend/include/filereader.hpp"
//#include "backend/include/test/testsuite_2.hpp"
#include "backend/include/test/testsuite_filereader_1.hpp"

using namespace std;

void test1()
{
    DLS::Path path("Yo!");
    cout << "Size of DLS::Path: " << sizeof(fstream) << endl;
}

int main2()
{
    //Test::test1();
    //Test::run_it();

    //Test::test2();
    //DLS::Test::filereader();

    cout << endl;

    //DLS::FileReader reader("/home/whiztim/Documents/developer/C++/Codeblocks/Duplichase/don.rar");
    //cout << "Size of don.rar: " << reader.size() << endl;
    return 0;
}
