#ifndef PATHLISTER_H
#define PATHLISTER_H

#include "path.hpp"
#include <list>
#include <unordered_set>
//#include <boost/filesystem.hpp>

/*! \namespace \a DLS
 *  The namespace of DupLiChaSe
 */
namespace DLS
{

/*! \typedef
 *  \brief A a list of \var DLS::Path objects used to bookkeep all files in Disk \
 *  made from \var std::list
 */
typedef std::list<Path> PathList;

/*! \class PathLister
 *  \brief This class is responsible for creating and managing the Paths to be listed
 *  Details:
 *  This class creates a Path list from an initial supply of either paths to scan or
 *  paths to skip...
 *  A path list
 */
class PathLister
{
    public:
        PathLister();
        ~PathLister();
        PathLister(const PathLister& other);
        void addPathException(const Path& path);
        void addPath(const Path& path);
    protected:
    private:
        PathList _paths;
};


}
#endif // PATHLISTER_H
