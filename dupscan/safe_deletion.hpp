#ifndef SAFE_DELETION_HPP
#define SAFE_DELETION_HPP
#include <QString>
    struct DeletionAgent
    {
        static QString toTrash(const QString& filePath);
    };

#endif // SAFE_DELETION_HPP
