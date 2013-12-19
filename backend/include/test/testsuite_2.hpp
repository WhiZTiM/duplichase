#ifndef TESTSUITE_2_H
#define TESTSUITE_2_H

#include <iostream>
#include <thread>
#include <chrono>
#include "backend/include/duplicatefinder.hpp"

namespace Test
{

    using namespace DLS;

    void tryBottleNeck()
    {
        std::string p1("/home/whiztim/Documents/developer/C++/Codeblocks/Duplichase/include/FileReader.h.gch");
        FileProperty pr1, pr2;
        pr1.setFilePath(p1);

        FR_by_size frS(8388608UL, 25165823UL);
        frS(p1);
        frS.getVerdict(pr1);

        FR_by_hashA frA(15);
        frA(p1);
        frA.getVerdict(pr1);

        FR_by_hashB frB(19);
        frB(p1);
        frB.getVerdict(pr1);

        using namespace std;

        cout << "Path:  " << pr1.getFilePath() << endl;
        cout << "Size:  " << pr1.getSize() << endl;
        cout << "hashA: " << pr1.getHashA() << endl;
        cout << "hashB: " << pr1.getHashB() << endl;
        cout << "\n" << endl;

        //FileReader r1(p1);
        //Hash::md5();
    }

    void printFileProperty(const DLS::FileProperty& fp, int k = 0)
    {
        std::cout << "(" << k << "): " << fp.getFilePath() << std::endl;
        std::cout << "File Size:  " << fp.getSize() << " bytes" << std::endl;
        std::cout << "File hashB: " << fp.getHashB() << std::endl;
    }


    //
    void test2()
    {
        DLS::Tree::DirectoryElementHelper helper(true);

        //helper.addPath("/home/whiztim/Documents");
        helper.addPath("/etc/");
        helper.addPath("/usr/lib");
        helper.addPath("/usr/lib64");
        helper.addPath("/home/whiztim/Documents/developer/C++/LibreCAD/librecad/res/");
        helper.addPath("/home/whiztim/Documents/developer/C++/LibreCAD/librecad/src");
        helper.addPath("/home/whiztim/Pictures/Computer");

        DLS::ExclusionHandler exclusion( helper.getDirectoryElement() );
        DLS::DuplicateFinderConstruct con(exclusion);

        const std::string pt1 = "/home/whiztim/Documents/developer";
        const std::string pt2 = "/home/whiztim/Downloads/";
        //const std::string pt1 = "/home/whiztim/Documents/developer/C++/LibreCAD/librecad";
        con.addPath(pt1);
        con.addPath(pt2);

        DLS::DuplicateFinder dFinder(con);

        while(!dFinder.endend())
        {
            //printFileProperty(dFinder.nextFile(), ++vb);
            dFinder.nextFile();
        }

        //! print them
        DLS::DuplicatesContainer duplicates = dFinder.getDuplicatesContainer();

        int lk = int(), mk = int();
        //*
        for(auto& iter : duplicates)
        {
            std::cout << "CATEGORY <<(" << ++lk << ")>>" << std::endl;
            for(auto& k : *iter)
            {
                //std::cout << k.getFilePath() << std::endl;
                printFileProperty(k, ++mk);
            }
        }


        std::cout << "\n\nPREPARING COUNTS:" << std::endl;

        long kc = 0;
        while(!dFinder.CountsPrepared())
        {
            std::cout << ++kc;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "\b\b";
        }
        //*/

        if(dFinder.CountsPrepared())
            std::cout << "\nCOUNTS PREPARED!" << std::endl;

    }


}

#endif
