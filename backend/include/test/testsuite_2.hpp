#ifndef TESTSUITE_2_H
#define TESTSUITE_2_H

#include <iostream>
#include <thread>
#include <chrono>
#include "backend/include/duplicatefinder.hpp"

namespace Test
{

    using namespace DLS;
    /*
    void print(DLS::RecursiveDirectoryIterator iterator)
    {
        int count = 0;
        while(!iterator.isEnded())
        {
            if(boost::filesystem::is_regular_file(iterator.getDirectoryEntry().path()))
            {
                std::cout << iterator.getDirectoryEntry().path().generic_string() << std::endl;
                ++count;
            }
            ++iterator;
        }

        std::cout << "\nCounted: " << count << std::endl;
    }
    */

    /*
    void run_it()
    {

        DLS::Tree::DirectoryElementHelper helper(true);

        helper.addPath("/home/whiztim/Documents");
        helper.addPath("/etc/");
        helper.addPath("/usr/lib");
        helper.addPath("/usr/lib64");
        helper.addPath("/home/whiztim/Documents/developer/C++/LibreCAD/librecad/res");
        helper.addPath("/home/whiztim/Pictures/Computer");

        const std::string rootPath = "/home/whiztim/Documents/developer/C++/LibreCAD/librecad";

        DLS::ExclusionHandler exclusion( helper.getDirectoryElement() );
        DLS::RecursiveDirectoryIterator iterator(rootPath, exclusion);

        print(iterator);
    }
    */

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
