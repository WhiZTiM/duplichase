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
#include "backend/include/path.hpp"

using namespace DLS;


bool Path::__FileLocking = false;

fs::path Path::fsPathFromString(const std::string path)
{
    return fs::path(path);
}

Path::Path(PathTypeOption option)
{
    _inputFileIsValid = false;
    _pathTypeOption = option;
}


Path::Path(const std::string& path, PathTypeOption option) : Path(option)
{
    _path = path;
    _ok = _validate();
}


std::string Path::toString() const
{
    return _path.generic_string();
}


Path::Path(const Path& path)
{
    _path = path._path;
    _inputFileIsValid = path._inputFileIsValid;
    _pathTypeOption   = path._pathTypeOption;
}


bool Path::exists() const
{
    return fs::exists(_path);
}


bool Path::_validate()
{
    if(fs::exists(_path))
    {
        _path.make_preferred();

        if(_pathTypeOption & PathTypeOption::AcceptBothFileAndDirectory)
        {
            //! nothing special here yet... Work needed here
            //! control will automatically work in the next condition
            _pathTypeOption = AcceptRegularFile;
        }

        if(_pathTypeOption & PathTypeOption::AcceptRegularFile)
        {
            if(fs::is_regular_file(_path))
                return true;

            return false;
        }

        if(_pathTypeOption & PathTypeOption::AcceptRegularDirectory)
        {
            if(fs::is_directory(_path))
                return true;

            return false;
        }

        else
            return false;
    }

    return false;
}

bool Path::OK() const
{
    return _ok;
}

Path::~Path()
{
    //dtor
}

/*
bool Path::__avoidTHIS__SetFileLocking(bool FileLocking)
{
    __FileLocking = FileLocking;
}
*/
