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
#ifndef FILEREADER_H
#define FILEREADER_H

#include <fstream>
#include <string>

/*! \namespace \a DLS
 *  The namespace of DupLiChaSe
 */

typedef unsigned long ulong;

namespace DLS
{

/*! \class FileReader
 *  \brief This class is responsible for extracting Data from a file in a very flexible way
 *  \par \b Details:
 *  All method are self documented... this class is guaranteed not to throw an exception
 *  rather, Upon error, it's member funtions returns \var 0, \var false, or an empty \var string
 *  where apropriate ... it is the responsibility of the
 *  programmer to check for error with eof() or bad() accordingly... or write sane codes!
 *  .....
 */
class FileReader
{
    public:

        /*! \struct OPT \enum Postion
         *  \brief A Relative Position Marker
         *  \a Start : Starts from the absolute beginning of the file
         *  \a Middle : uses \fn static_cast<int>( FileReader::size() / 2 ) as position
         *  \a Foward : reads the stream in a forward direction
         *  \a Reversed : reads the stream in a reversed direction and returns the bytes in reversed manner
         *  \a End : reads the operation from the End...
         *  \note \param End is expressly equivalent to the combination of (End | Reversed)
         */
         struct OPT{
            enum Position
                {
                    Start = 0x01,
                    LastUsed = 0x02,
                    Middle = 0x04,
                    Forward = 0x09,
                    Reversed = 0x10,
                    End = 0x30
                };
         };

        /*! \fn FileReader()
         *  \brief Default ctor
         */
        FileReader() = delete;

        /*! \fn FileReader(const std::string& path)
         *  \brief Construct a FileReader and instantiate stream from path
         *  if the path doesn't exist, there's nothrow
         */
        FileReader(const std::string& path);


        /*! \fn FileReader(const std::ifstream& stream)
         *  \brief Construct a FileReader with a reference to an std::ifstream
         *  \b PreCondition The stream must be instantiated
         */
        FileReader(std::ifstream& stream) = delete;


        /*! \fn size() const
         *  \note if the file doesn't exist, it returns 0
         *  \return the file size in bytes as \var unsigned long
         */
        inline unsigned long size() const { return _file_size; }

        //unsigned long getBytes() const;

        /*! \fn getPercentageOfBytes(unsigned long Bytes) const
         *  \note if \param Bytes is greater than \fn size()
         *  then, this funtion returns 0;
         *  \return percentage of bytes as \var unsigned int
         */
        inline unsigned int getPercentageOfBytes(unsigned long Bytes) const;

        /*! \fn getPrecisePercentageOfBytes(unsigned long Bytes) const;
         *  \note the precise percentage of bytes constituted by \param Bytes
         *  if \param Bytes is greater than \fn size() , this function returns 0
         *  \return percentage of bytes
         */
        inline float getPrecisePercentageOfBytes(unsigned long Bytes) const;


        /*! \fn getBytesByPercentage(float percentage) const;
         *  \brief to determine the number of bytes made by a certain percentage
         *  \return the number of bytes made by \param percentage
         */
        unsigned long getBytesByPercentage(float percentage) const;


        /*! \fn unsigned long getBytes(const char* chars, unsigned long bytes,  OPT::Position position = OPT::Position::Start) const;
         *  \brief reads into \param chars, bytes represented by \param bytes
         *  the \param chars is the character array were \a bytes of character read are written to.
         *  NOTE: it is the duty of the caller to have enough space to hold
         *  bytes\a chars is NOT a null terminated string!
         *  the \param position is used to determine the starting position
         *  \return the number of bytes read into \a chars
         */
        unsigned long getBytes(const char* chars, unsigned long bytes,  OPT::Position position = OPT::Position::Start);


        /*! \fn getStringByPercentage(float percentage, OPT::Position position = OPT::Position::Start) const;
         *  \brief gets the actual bytes represented by a certain \a percentage (between 0.0 and 100.0)
         *  the \param position is used to determine the starting position
         *  \return the number of bytes made by \param percentage
         */
        std::string getStringByPercentage(float percentage, OPT::Position position = OPT::Position::LastUsed);


        /*! \fn getStringByBytes(unsigned long bytes, OPT::Position position = OPT::Position::Start) const;
         *  \brief gets the actual bytes represented by a certain \a bytes
         *  the \param position is used to determine the starting position
         *  \return the number of bytes made by \param percentage
         */
        std::string getStringByBytes(unsigned long bytes, OPT::Position position = OPT::Position::LastUsed);


        //! returns the file path represented by this stream
        inline std::string getFilePath() const
        { return _file_path; }

        /*! \fn refresh()
         *  \brief resets the stream state and seeks to beginning
         */
        void refresh();


        /*! \fn refresh()
         *  \brief gets the current seek pointer position
         *  \return seek pointer position \var unsigned long
         */
        inline unsigned long getCurrentBytePosition() const { return _tell_g; }


        /*! \fn setCurrentBytePosition(unsigned long bytePosition)
         *  \brief sets the absolute position of the seek pointer
         *  \return \a true if the position is within size range... else false
         */
        bool setCurrentBytePosition(unsigned long bytePosition);


        /*! \fn eof()
         *  \brief checks whether seek pointer is at End of File
         *  \return true if EOF, otherwise, false
         */
        inline bool eof() const { return _file.eof(); }


        /*! \fn bad()
         *  \brief returns true if something terrible happened like, file stream error.. disk crash, etc
         *  \return \a true if bad... \var bool
         */
        inline bool bad() const { return _file.bad(); }

        //! failed state... could not perform I/O operation
        inline bool fail() const { return _file.fail(); }

        //! Everything is in good state... permission granted to roger ahead
        inline bool good () const { return _file.good(); }

        //! Self explanatory :-) ... if file stream is opened
        inline bool is_open() const { return _file.is_open(); }

        //! this grants us joyride to using boolean expresions e.g \c if(FileReader)
        inline operator bool () const { return !!_file; }

        //! sets the internal buffer size used by the stream...  though this may be temporarily overriden
        bool setBufferSize(unsigned int size);

        //! Destructor... does nothing special other than destroy ... :-)
        ~FileReader();

        //I really want to remove this... but may be useful...
        mutable ulong readd = 0;
    protected:


        /*! \fn _reset()
         *  \brief internally used to clear stream states and reset seek pointer
         *  \note Does not reload stream data
         */
        void _reset();


        /*! \fn _getReversedStringByBytes(unsigned long bytes) const
         *  \brief internally used to reverse the string
         */
        std::string _getReversedStringByBytes(unsigned long bytes) const = delete;

    private:

        std::ifstream _file;        //!< holds a reference to the filestream object

        std::string _file_path;      //!< holds the file path;

        bool _reversed_direction = false;

        unsigned long _file_size;           //!< holds the filesize.... assuming the file isn't modified by another process

        unsigned int _bufferSize = 1048576;    //!< buffer size... from 0 to a maximum of 1048576 bytes

        unsigned long _tell_g = 0;

        //bool _needToDelete = false;         //!< not needed
};

}
#endif // FILEREADER_H
