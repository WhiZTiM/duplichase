#ifndef PATHHELPER_H
#define PATHHELPER_H

#include "path.hpp"
#include <string>

namespace DLS
{

class PathHelper
{
    public:
        PathHelper();
        ~PathHelper();
    protected:
    private:
};

class PathListMaker
{
    public:
    PathListMaker();

    bool addPath(const std::string path);


    std::list<Path> getPathList() const;

    ~PathListMaker();
    private:

    std::list<Path> _paths;
};

}
#endif // PATHHELPER_H
