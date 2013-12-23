/*******************************************************************************************
**  (C) Copyright July 2013 by
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
#ifndef PATH_H
#define PATH_H

#include <boost/filesystem.hpp>
#include <fstream>
#include <string>
#include <list>
#include "exception.hpp"

/*! \namespace \a DLS
 *  The namespace of DupLiChaSe
 */
namespace DLS
{

namespace fs = boost::filesystem;   //!< for convenience purposes

/*! \typedef \a _boost_path_list
 *  for convenience purposes
 */
typedef std::list<fs::path> _boost_path_list;

/*! \class Path
 *  \brief A thin wrapper of boost::filesystem::path
 */
class Path
{
    public:

    enum StartFrom{ Beginning, End};
    enum PathTypeOption{    AcceptRegularFile = 0x01,
                            AcceptRegularDirectory = 0x02,
                            AcceptBothFileAndDirectory = 0x03
                        };

        Path(PathTypeOption option = AcceptRegularFile);

        Path(const std::string& path, PathTypeOption option = AcceptRegularFile);

        /*! \fn DLS::Path::Path(const Path& path)
         *  \brief Copy constructor
         *  Does not copy stream state of the file, i.e if \enum AcceptRegularFile
         */
        explicit Path(const Path& path);


        /*! \fn Path::exists() const
         *  \brief
         */
        bool exists() const;

        /*! \fn Path::OK() const
         *  \brief returns true if Everything is OK
         *  This member function returns the state of the Path object
         *  based on WHATEVER internal mutating operation done on it, and its never stale nor obsolete
         *  \return \a true if is ok else \a false
         */
        bool OK() const;


        /*! \fn Path::toString() const
         *  \brief returns absolute file path as std::string
         *  This returns it as a preferred file path
         */
        std::string toString() const;


        /*! \fn Path::__avoidTHIS__SetFileLocking(bool fileLocking) = delete
         *  \brief Whether to use Operating system's Kernel or Filesystem's Mandatory Locking
         *  \par Details
         *  I am not sure, I know why I have this option, but currently, I have disabled it's useage
         *  It can be enabled if you think its relevant for your environment
         *  But, for the usecases I know, You don't need this
         *  \param \a fileLocking ...set it to true if you want this feature
         *  or set it to \a false. \b Default is \a false
         *  \note \NOTE: as of this version, this feature is not yet implemented
         */
        static void __avoidTHIS__SetFileLocking(bool fileLocking) = delete;


        static fs::path fsPathFromString(const std::string path);


        std::string getHash(ulong percentage, StartFrom startfrom = Beginning) const;


        PathTypeOption getPathTypeOption() const { return _pathTypeOption; }


        fs::path fsPath() const { return _path; }


        ~Path();

    protected:

    private:

    bool _isValid() const;
    bool _lockFile();
    bool _validate();

    //! WARNING! Update COPY-CONSTRUCTOR and other constructors upon modification to private members
    bool _inputFileIsValid;
    bool _ok = false;

    PathTypeOption _pathTypeOption;

    fs::path _path;

    //std::ifstream _file_descriptor;

    static bool __FileLocking;
};

/*! \fn PathLister
 *  \brief This class is responsible for creating and managing the Paths to be listed
 *  Details:
 *  This class creates a Path list from an initial supply of either paths to scan or
 *  paths to skip...
 *  A path list
 */


}

#endif // PATH_H
