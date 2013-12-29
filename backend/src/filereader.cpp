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
#include "backend/include/filereader.hpp"
#include <cmath>
#include <cstring>
#include <algorithm>

using namespace DLS;

bool isLessThan4GB(const std::string& path);

//constructor
FileReader::FileReader(const std::string& path)
    : _file_size(0)
{
    _file.open(path, std::ios::in | std::ios::ate | std::ios::binary);
    if(_file)
    {
        if(!isLessThan4GB(path))
            _file.setstate(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
        else
            _file_size = _file.tellg();
    }

    _file_path  = path;
    _reset();
}

//protected function
void FileReader::_reset()
{
    if(_file)
        _file.seekg(0);
}

//gets file by percentage
unsigned int FileReader::getPercentageOfBytes(unsigned long Bytes) const
{
    return static_cast<int>( getPrecisePercentageOfBytes(Bytes) );
}

//gets percentage made up by bytes
float FileReader::getPrecisePercentageOfBytes(unsigned long Bytes) const
{
    if(!_file)
        return 0.0;
    if(Bytes > _file_size)
        return 0.0;
    if(_file_size == 0)
        return 0.0;

    return ( (static_cast<float>(Bytes) / _file_size) * 100);
}

//percentage
unsigned long FileReader::getBytesByPercentage(float percentage) const
{
    return static_cast<long>( std::ceil( percentage / 100 * _file_size ) );
}


//sets seekg() and returns if your position was valid
bool FileReader::setCurrentBytePosition(unsigned long bytePosition)
{
    if(bytePosition > _file_size)
        return false;
    _file.seekg(bytePosition);
    return true;
}

//A thin wraper for convenience
std::string FileReader::getStringByPercentage(float percentage, OPT::Position position)
{
    unsigned long bytes = getBytesByPercentage(percentage);
    return getStringByBytes(bytes, position);
}

//The makor aim of this class
std::string FileReader::getStringByBytes(unsigned long bytes, OPT::Position position)
{
    if(!_file)
        return std::string();

    //used to reverse the bytes in memory... if the OPT::Position::Reverse FLAG is set
    bool _reverse_this = false;

    //FLAG type 1

    //operation for the start flag
    if((position & OPT::Start) == OPT::Start)
    {
        _file.seekg(0);
        _tell_g = 0;
    }

    //operation for the Middle flag
    else if((position & OPT::Middle) == OPT::Middle)
    {
        _file.seekg( getBytesByPercentage(50.0) );
        _tell_g = ( getBytesByPercentage(50.0) );
    }

    //operation to use the last position
    else if((position & OPT::LastUsed) == OPT::LastUsed)
    {
        if(_reversed_direction)
            _reverse_this = true;
    }

    else if((position & OPT::End) == OPT::End)
    {
        _file.seekg(0, std::ios::end);
        _tell_g = _file_size;
    }


    //FLAG Type 2.

    if((position & OPT::Forward) == OPT::Forward)
    {
        _reversed_direction = false;
    }

    //if reversed flag set
    else if((position & OPT::Reversed) == OPT::Reversed)
    {
        _reversed_direction = true;
        _reverse_this = true;
        //assert(_file.tellg() >= bytes);
        if(_file.eof() || !_file || _file.bad())
        {
            _file.clear();
        }
        //check that bytes is not a relative valid range.... set ios::badbit state for illegal attempt
        if( (long long)(static_cast<unsigned long>( _file.tellg() ) - bytes) < 0)
        {
            _file.setstate(std::ios::badbit);
            return std::string();
        }
        //seek it to bytes less than current, so that we can read foward and reverse
        _file.seekg( static_cast<unsigned long>( _file.tellg() ) - bytes);
    }


    std::string rtn;

    unsigned int buffSize = _bufferSize;
    unsigned long readSoFar = 0;

    if(bytes < buffSize)
        buffSize = bytes;
    if((_file_size - _file.tellg()) < buffSize)
        buffSize = _file_size - _file.tellg();

    if(buffSize == 0)   //Very evil stuff occured
    {
        _file.setstate(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
        return "";
    }
    if(buffSize > _bufferSize)
    {
        ulong stz = _file.tellg();
        _file.setstate(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
        return "";
    }

    char buffer[buffSize];

    //size of buffer
    unsigned long sz = sizeof(buffer) / sizeof(char);

    //while bytes read are less than required AND we can still read
    while(readSoFar < bytes && _file.read(buffer, sz ))
    {
        rtn += std::string(buffer, sz );
        readSoFar += sz;
        readd += sz;

        //Optimize next read if possible
        if(readSoFar > bytes)
        {
            sz = readSoFar - bytes;
        }

    }
    //check for the last unread bytes
    if(_file.eof())
    {
        if(_file.gcount() > 0)
        {
            rtn += std::string(buffer, _file.gcount());
            readd += _file.gcount();
        }
        _file.clear();
    }

    //if the stream is to be reversed

    //NOTE, if this portion is executed, it slows down this function....
    if(_reverse_this)
    {
        unsigned long ptrPos = static_cast<unsigned long>( _tell_g ) - rtn.size();
        _file.seekg(ptrPos);

        if(ptrPos < _file_size)
            _tell_g = _file.tellg();
        else
            _tell_g = _file_size;

        std::reverse(rtn.begin(), rtn.end());
        return rtn;
        //return std::string( rtn.rbegin(), rtn.rend() );
    }

    if(!_file.eof())
        _tell_g = _file.tellg();
    else
        _tell_g = _file_size;

    return rtn;

}


unsigned long FileReader::getBytes(const char* chars, unsigned long bytes, OPT::Position position)
{
    std::string *str = new std::string( std::move( getStringByBytes(bytes, position) ) );
    std::memcpy(const_cast<char*>(chars), str->data(), str->size());
    return str->size();
}

//set buffer size...
//At users descreetion... between 1byte and 1MegaByte
bool FileReader::setBufferSize(unsigned int size)
{
    if(size > 0  && size <= 1048576)
    {
        _bufferSize = size;
        return true;
    }
    return false;
}


void FileReader::refresh()
{
    _file.clear();
    _reset();
}

FileReader::~FileReader()
{
    //nothing inferred
}

#ifdef QT_CORE_LIB
#include <QtGlobal>
#include <QFile>
#ifdef Q_OS_WIN
    #include <windows.h>
#else
    #ifdef Q_OS_LINUX
    #include <unistd.h>
    #else
    #error Only Windows and Unix have been implemented , please Implement a routine that checks \
    for files greater than 4GB
    #endif
#endif

bool isLessThan4GB(const std::string& path)
{
    QFile file(QString::fromStdString(path));
    if(file.size() > 4294967295)
        return false;
    return true;
    /*
    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        return false;
    LARGE_INTEGER l_size = 0;
    if(!GetFileSizeEx(hFile, &size))
    {
        CloseHandle(hFile);
        return false;
    }
    CloseHandle(hFile);
    std::int64_t size = l_size.QuadPart;
    if(size > 4294967295)
        return false;
    return true;

    WIN32_FIND_DATAW data;
    std::wstring w_path(path.begin(), path.end());
    HANDLE h = FindFirstFileW(w_path.c_str(), &data);
    if(h == INVALID_HANDLE_VALUE)
        return false;
    */
}
#else
#error This part needs Qt LIBRARY.. else, define a routine that checks for files greater than \
    4GB on x86 systems as above
#endif
