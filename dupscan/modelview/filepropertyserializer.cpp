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
#include "filepropertyserializer.hpp"
#include <algorithm>

DLS::DuplicatesContainer FilePropertySerializer::fromDisk(const QString &fileName)
{
    std::shared_ptr<FilePropertyFromDiskStore> fromDisk(new FilePropertyFromDiskStore(fileName));
    if(fromDisk->parse())
        return fromDisk->getDuplicatesContainer();
    err = fromDisk->error();
    return DLS::DuplicatesContainer();
}

bool FilePropertySerializer::toDisk(const QString &fileName, const DLS::DuplicatesContainer& container)
{
    std::shared_ptr<FilePropertyToDiskStore> toDisk(new FilePropertyToDiskStore(fileName));
    if(toDisk->serializeToDisk( container ))
        return true;
    err = "Could not open the File Specified";
    return false;
}

bool FilePropertySerializer::verifyAndFilter_inPlace(DLS::DuplicatesContainer &duplicates,
                                                     FilePropertySerializer::VerificationCallbacks& vc)
{
    QList<DLS::ptrVEC_FileProperty> groupRemovals;
    int pr = 0, prTotal = duplicates.size();
    for(auto& vecs : duplicates)
    {
        /*
        using iterator_type = decltype((*vecs).begin());
        QList<decltype((*vecs).begin())> unitRemovals;
        for(auto iter = (*vecs).begin(); iter != (*vecs).end(); iter++)
        {
            QString fileName = QString::fromStdString( (*iter).getFilePath() );
            vc.CurrentFileCallback( fileName );
            if(vc.isTaskCancelledCallback())
            {
                vc.PercentageProgressCallback(100);
                return false;
            }
            if(! QFile::exists( fileName ) )
                unitRemovals.push_back(iter);
        }

        for(int i=0; i < unitRemovals.size(); i++)
        {
            (*vecs).erase( unitRemovals[i] );
        }

        if((*vecs).size() < 2)           //Theres no point keeping One or Zero items in the list....
            groupRemovals.push_back(vecs);
        ?*/
        QList<int> unitRemovals;
        const int sz = vecs->size();
        for(int i = 0; i < sz; i++)
        {
            QString fileName = QString::fromStdString( vecs->operator[](i).getFilePath() );
            vc.CurrentFileCallback( fileName );
            if(vc.isTaskCancelledCallback())
            {
                vc.PercentageProgressCallback(100);
                return false;
            }
            if(! QFile::exists( fileName ) )
                unitRemovals.push_back(i);
        }

        int t_count = 0;
        for(int i = 0; i < unitRemovals.size(); i++)
        {
            const int m = unitRemovals[i] - (t_count++);
            if(vecs->size() >= 2)   //! We should never leave less than 1 item in vecs;
                vecs->erase(vecs->begin() + m);
        }

        if(vecs->size() < 2)
            groupRemovals.push_back(vecs);

        vc.PercentageProgressCallback( static_cast<int>( pr * 100.0 / prTotal ) );
        ++pr;
    }

    for(auto rm : groupRemovals)
        duplicates.erase( rm );
    vc.PercentageProgressCallback(100);
    return true;
}

QString FilePropertySerializer::error() const
{
    return err;
}
