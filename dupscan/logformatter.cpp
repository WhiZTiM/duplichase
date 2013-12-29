/*******************************************************************************************
**  (C) Copyright October 2013 by
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
#include "logformatter.hpp"

LogFormatter::LogFormatter()
{
}

void LogFormatter::formatAndWriteLogMessage(LOGType logtype, QString header, QString body)
{
    /*Hebrews 2:3 How shall we escape, if we neglect so great salvation;
     *which at the first began to be spoken by the Lord,
     *and was confirmed unto us by them that heard him;
     */

    if(body == lastMessage)
        return;
    lastMessage = body;

    const QString s = body.isEmpty() ? "" : ": ";
    static const QString K_Bold =           "<b>";
    static const QString K_Italics =        "<i>";
    static const QString K_EndColor =       "</font>";
    static const QString K_EndItalics =     "</i>";
    static const QString K_EndBold =        "</b>";
    static const QString K_BlackColor =     "<font color='#000000'>";
    static const QString K_RedColor =       "<font color='#FF0000'>";
    static const QString K_DRedColor =      "<font color='#770000'>";
    static const QString K_DGreenColor =    "<font color='#006000'>";
    static const QString K_BlueColor =      "<font color='#0000FF'>";
    static const QString K_DBlueColor =     "<font color='#404184'>";
    static const QString K_DBrownColor =    "<font color='#400320'>";
    static const QString K_MaroonColor =   "<font color='#AD0C54'>";

    QString rtn;

    switch (logtype) {
        case LOGType::None:
            rtn = header + body;
            break;
        case LOGType::DLSCoreInfo:
            rtn = K_DBrownColor +
                    header + s +
                  K_EndColor +
                    K_MaroonColor + body + K_EndColor;
            break;
        case LOGType::DLSCoreWarning:
            rtn = K_Bold + K_DGreenColor +
                    header + s +
                  K_EndColor + K_EndBold +
                    body;
            break;
        case LOGType::DLSCoreError:
            rtn = K_Bold + K_DRedColor +
                    header + s +
                  K_EndColor + K_EndBold +
                    body;
            break;
        case LOGType::Info:
            rtn = K_Bold + K_BlueColor +
                    header + s +
                  K_EndColor + K_EndBold +
                    K_Italics + body + K_EndItalics;
            break;
        case LOGType::Warning:
            rtn = K_Bold + K_DBlueColor +
                    header + s +
                  K_EndColor + K_EndBold +
                    K_Italics + body + K_EndItalics;
            break;
        case LOGType::Error:
            rtn = K_Bold + K_DRedColor +
                    header + s +
                  K_EndColor + K_EndBold +
                    K_Italics + body + K_EndItalics;
            break;
        default:
            rtn = header + body;
            break;
    }

    emit logMessage(rtn);
}
