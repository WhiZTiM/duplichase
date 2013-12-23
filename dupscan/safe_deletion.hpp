/*******************************************************************************************
**  (C) Copyright October 2013 - November 2013 by
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
#ifndef SAFE_DELETION_HPP
#define SAFE_DELETION_HPP
#include <QString>
    struct DeletionAgent
    {
        static QString toTrash(const QString& filePath);
    };

#endif // SAFE_DELETION_HPP
