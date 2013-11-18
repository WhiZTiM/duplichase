#include "safe_deletion.hpp"
#ifdef Q_OS_WIN32
#include "windows.h"
#endif
#include <QFileInfo>
#include <QStringList>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QDateTime>

QString DeletionAgent::toTrash(const QString &filePath)
{
#ifdef Q_OS_WIN32
    QFileInfo fileinfo( filePath );
    if( !fileinfo.exists() )
        return QString( "File does not exist, Failed to move to Recycle Bin" );
    WCHAR from[ MAX_PATH ];
    memset( from, 0, sizeof( from ));
    int l = fileinfo.absoluteFilePath().toWCharArray( from );
    Q_ASSERT( 0 <= l && l < MAX_PATH );
    from[ l ] = '\0';
    SHFILEOPSTRUCT fileop;
    memset( &fileop, 0, sizeof( fileop ) );
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int rv = SHFileOperation( &fileop );
    if( 0 != rv ){
        qDebug() << rv << QString::number( rv ).toInt( 0, 8 );
        return QString( "Unknown Error: Failed to Move to Recycle bin" );
    }
#endif

#ifdef Q_OS_LINUX
    static bool TrashInitialized = false;
    QString TrashPath;
    QString TrashPathInfo;
    QString TrashPathFiles;

    #ifdef QT_GUI_LIB
        if( !TrashInitialized ){
            QStringList paths;
            const char* xdg_data_home = getenv( "XDG_DATA_HOME" );
            if( xdg_data_home ){
                qDebug() << "XDG_DATA_HOME not yet tested";
                QString xdgTrash( xdg_data_home );
                paths.append( xdgTrash + "/Trash" );
            }
            QString home = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
            paths.append( home + "/.local/share/Trash" );
            paths.append( home + "/.trash" );
            foreach( QString path, paths ){
                if( TrashPath.isEmpty() ){
                    QDir dir( path );
                    if( dir.exists() ){
                        TrashPath = path;
                    }
                }
            }
            if( TrashPath.isEmpty() )
                return QString( "Cannot detect Trash folder: For Saftey reason, File was NOT deleted" );
            TrashPathInfo = TrashPath + "/info";
            TrashPathFiles = TrashPath + "/files";
            if( !QDir( TrashPathInfo ).exists() || !QDir( TrashPathFiles ).exists() )
                return QString( "Trash does not comply with FreeDesktop.org Trash specification" );
            TrashInitialized = true;
        }
        QFileInfo original( filePath );
        if( !original.exists() )
            return QString( "File doesnt exists, cannot move to trash" );
        QString info;
        info += "[Trash Info]\nPath=";
        info += original.absoluteFilePath();
        info += "\nDeletionDate=";
        info += QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzzZ");
        info += "\n";
        QString trashname = original.fileName();
        QString infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
        QString filepath = TrashPathFiles + "/" + trashname;
        int nr = 1;
        while( QFileInfo( infopath ).exists() || QFileInfo( filepath ).exists() ){
            nr++;
            trashname = original.baseName() + "." + QString::number( nr );
            if( !original.completeSuffix().isEmpty() ){
                trashname += QString( "." ) + original.completeSuffix();
            }
            infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
            filepath = TrashPathFiles + "/" + trashname;
        }
        QDir dir;
        if( !dir.rename( original.absoluteFilePath(), filepath ) ){
            return QString( "move to trash operation FAILED" );
        }
        //File infofile;
        //infofile.createUtf8( infopath, info );
    #else
        Q_UNUSED( filePath );
        return QString( "Trash in server-mode not supported" );
    #endif
#endif
        return QString();
}
