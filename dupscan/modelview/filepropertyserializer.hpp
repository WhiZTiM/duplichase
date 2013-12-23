/*******************************************************************************************
**  (C) Copyright December 2013 by
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
#ifndef FILEPROPERTYSERIALIZER_HPP
#define FILEPROPERTYSERIALIZER_HPP
#include "filepropertystoremanagement.hpp"
#include <functional>
#include <atomic>

class FilePropertySerializer
{
public:
    FilePropertySerializer() = default;
    DLS::DuplicatesContainer fromDisk(const QString& fileName);
    bool toDisk(const QString& fileName, const DLS::DuplicatesContainer &container);
    QString error() const;

    struct VerificationCallbacks
    {
        std::function<void(int)> PercentageProgressCallback;
        std::function<void(QString)> CurrentFileCallback;
        std::function<bool()> isTaskCancelledCallback = false;
    };

    static bool verifyAndFilter_inPlace(DLS::DuplicatesContainer &duplicates, VerificationCallbacks& vc);

private:
    QString err;
};

#endif // FILEPROPERTYSERIALIZER_HPP
