#include "backend/include/filereader.hpp"
#include <cmath>
#include <cstring>

using namespace DLS;

//constructor
FileReader::FileReader(const std::string& path)
{
    _file.open(path, std::ios::in | std::ios::ate);
    if(_file)
        _file_size = _file.tellg();
    else
        _file_size = 0;
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
        if( static_cast<unsigned long>( _file.tellg() ) - bytes < 0)
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

        return std::string( rtn.rbegin(), rtn.rend() );
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
