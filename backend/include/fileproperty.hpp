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
#ifndef FILEPROPERTY_H
#define FILEPROPERTY_H
#include <string>

typedef unsigned long ulong;

namespace DLS
{

    class FileProperty
    {
        public:
            FileProperty();
            virtual ~FileProperty();

            //SETTERS
            inline void setFilePath(const std::string& path) { _filePath = path; }
            inline void setCategory(const std::string& category) { _category = category; }
            inline void setHashA(const std::string& value) { _hashA = value; }
            inline void setHashB(const std::string& value) { _hashB = value; }
            inline void setHashC(const std::string& value) { _hashC = value; }
            inline void setSize(unsigned long size) { _fileSize = size; }
            inline void setF_A(float f) { _a = f; }
            inline void setF_B(float f) { _b = f; }
            inline void setTag(int value) { _tag = value; }

            //GETTERS
            inline unsigned long getSize() const { return _fileSize; }
            inline float getF_A() const { return _a; }
            inline float setF_B() const { return _b; }
            inline int getTag() const { return _tag; }
            inline std::string getFilePath() const { return _filePath; }
            inline std::string getCategory() const { return _category; }
            inline std::string getHashA() const { return _hashA; }
            inline std::string getHashB() const { return _hashB; }
            inline std::string getHashC() const { return _hashC; }
            inline std::string getFileName() const
            {
                std::string::size_type st = _filePath.rfind('/');
                if(st == std::string::npos) return "";
                return _filePath.substr(st);
            }

            inline std::string getFileExtension() const
            {
                std::string::size_type st = _filePath.rfind('.');
                if(st == std::string::npos) return "";
                return std::string( _filePath.substr(st));
            }

        private:
        unsigned long _fileSize = 0;
        float _a = 0.0, _b = 0.0;
        int _tag = -1;
        std::string _category;
        std::string _filePath;
        std::string _hashA;
        std::string _hashB;
        std::string _hashC;
    };

}
#endif // FILEPROPERTY_H
