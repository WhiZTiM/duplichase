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

int m()
{
    Rule_X a, s, d;

    sizer sz = &Rule_X::getSize;
    bool k = (a.*sz)(23,'k');
    bool l = (s.*sz)(283,'l');
    bool m = (d.*sz)(73,'m');
    cout << "Bool = " << k << " " << l << " " << m << endl;

    return 0;
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
