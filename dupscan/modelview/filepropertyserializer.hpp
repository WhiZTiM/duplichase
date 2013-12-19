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
