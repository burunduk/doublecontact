/* Double Contact
 *
 * Module: Comma-separated values (CSV) file export/import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

// TODO csv support is very alpha

#ifndef CSVFILE_H
#define CSVFILE_H

#include "csvproiles.h"
#include "fileformat.h"

class CSVFile : public FileFormat
{
public:
    CSVFile();
    virtual ~CSVFile();
    // IFormat interface
public:
    static bool detect(const QString &url);
    static QStringList supportedExtensions();
    static QStringList supportedFilters();
    bool importRecords(const QString &url, ContactList &list, bool append);
    bool exportRecords(const QString &url, ContactList &list);
private:
    CSVProileBase* profile;
};

#endif // CSVFILE_H
