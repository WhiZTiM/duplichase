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
#include "backend/include/duplicatefinder.hpp"

namespace DLS
{
    namespace Test
    {
        void filereader()
        {
            FileReader reader1("/home/whiztim/Downloads/others/May/Lib/crisscross-0.8.0/doc/html/classCrissCross_1_1Crypto_1_1MD4Hash-members.html");
            const std::string md5_1 =  Hash::md5( reader1.getStringByPercentage(100, FileReader::OPT::Position::End) );

            FileReader reader2("/home/whiztim/Documents/developer/C++/Qt/external/relax/Relax-build-Desktop_Qt_5_0_0_GCC_32bit_SDK-Debug/moc_watcherthread.cpp");
            const std::string md5_2 = Hash::md5( reader2.getStringByPercentage(100, FileReader::OPT::Position::End) );

            std::cout << reader1.getFilePath() << "\nSize: " << reader1.size() << " bytes\nmd5: " << md5_1 << std::endl;
            std::cout << reader2.getFilePath() << "\nSize: " << reader2.size() << " bytes\nmd5: " << md5_2 << std::endl;
        }
    }
}
