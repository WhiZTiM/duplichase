#include "filepropertyserializer.hpp"

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

        if((*vecs).size() < 2)              //Theres no point keeping One or Zero items in the list....
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
