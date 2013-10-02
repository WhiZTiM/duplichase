#include "backend/include/pathhelper.hpp"

using namespace DLS;

PathHelper::PathHelper()
{
    //ctor
}

PathHelper::~PathHelper()
{
    //dtor
}


PathListMaker::PathListMaker()
{

}

bool PathListMaker::addPath(const std::string path)
{
    Path _pt(path);
    _paths.push_back(_pt);
    return _pt.exists();
}
