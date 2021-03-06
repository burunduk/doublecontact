/* Double Contact
 *
 * Module: Data and container structures
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QByteArray>
#include <QDateTime>
#include <QStringList>
#include "globals.h"

#define MAX_COMPARE_PRIORITY_LEVEL 5
#define COUNTRY_RULES_COUNT 3
#define MAX_NAMES 5
// According vCard 4.0, contact can have only one anniversary
#define MAX_ANN 1

struct TagValue { // for non-editing ang unknown tags
    QString tag, value;
    TagValue(const QString& _tag, const QString& _value);
};

// vCard item with one of more types (one phone, email, impp, etc.)
struct TypedDataItem {
    QString value;
    QStringList types;
    // Phone: some devices & addressbooks may allow create any tel type (not RFC, but...)
    // Email: according RFC 2426, may be non-standard
    int syncMLRef;
};

struct Phone: public TypedDataItem {
    Phone();
    Phone(const QString& _value, const QString& type1 = "", const QString& type2 = "");
    bool operator ==(const Phone& p);
    // international number representation from particular
    static QStringList availableCountryRules();
    QString expandNumber(int countryRule) const;
    static QString expandNumber(const QString& number, int countryRule);
    // standart types
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
        void fill();
    } standardTypes;
};

struct Email: public TypedDataItem {
    Email();
    Email(const QString& _value, const QString& type1 = "", const QString& type2 = "");
    bool operator ==(const Email& e);
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
        void fill();
    } standardTypes;
};

struct DateItem { // Birthday and anniversaries
    QDateTime value; // date & time
    bool hasTime; // false if date only was in file
    bool hasTimeZone; // record contains TZ info
    short zoneHour, zoneMin; // TZ value
    enum DateFormat {
        ISOBasic,    // basic ISO 8601 format (in general, for vCard 2.1)
        ISOExtended, // extended ISO 8601 format (in general, for vCard 3.0)
        Local        // human-readable, according current locale
    };
    bool operator ==(const DateItem& d);
    void clear();
    QString toString(DateFormat format) const;
    inline bool isEmpty() const {return value.isNull(); }
};

struct PostalAddress {
    QString offBox, extended, street, city, region, postalCode, country;
    QStringList paTypes; // home, work, dom, postal, etc.
    bool operator ==(const PostalAddress& a);
    void clear();
    QString toString() const;
    bool isEmpty() const;
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
        void fill();
    } standardTypes;
};

struct Photo {
    QString pType; // URL, JPEG, PNG or unsupported, but stored value
    QByteArray data;
    QString url;
    bool operator ==(const Photo& p) const;
    void clear();
    bool isEmpty() const;
    QString detectFormat() const;
};

struct ContactItem {
    QString fullName;
    QStringList names;
    QList<Phone> phones;
    QList<Email> emails;
    DateItem birthday;
    QList<DateItem> anniversaries;
    QString sortString;
    QString description;
    Photo photo;
    // Work
    QString organization, title;
    // TODO role, logo?
    // Addresses
    PostalAddress addrHome, addrWork; // TODO are vCards with more addresses exists in wild nature?
    // Internet
    QString nickName, url, jabberName, icqName, skypeName;
    // Format internals
    QString id; // optional record unique id (udx Sequence, vcf X-IRMC-LUID, etc)
    QString originalFormat;
    QString version, subVersion;
    QList<TagValue> otherTags;   // Known but un-editing tags
    QList<TagValue> unknownTags; // specific tags for any file format, i.e. vcf
    // Calculated fields for higher perfomance
    QString visibleName, prefPhone, prefEmail;
    // Calculated fields for list comparison
    enum PairState {
        PairNotFound,
        PairSimilar,
        PairIdentical
    } pairState;
    ContactItem* pairItem;
    int pairIndex;
    // Editing
    void clear();
    bool swapNames();
    bool splitNames(); // TODO int index
    bool dropSlashes();
    bool intlPhonePrefix(int countryRule);
    // Aux methods
    void calculateFields();
    QString formatNames() const;
    QString makeGenericName() const;
    void reverseFullName();
    void dropFinalEmptyNames(); // If empty parts not in-middle, remove it
    bool similarTo(const ContactItem& pair, int priorityLevel);
    bool identicalTo(const ContactItem& pair);
};

// MPB-specific storage
struct CallInfo {
    QString cType, timeStamp, duration, number, name;
};

struct MPBExtra {
    QString model, timeStamp;
    QStringList organizer, notes, SMS, SMSArchive;
    QList<CallInfo> calls;
    void clear();
};

// Entire address book
class ContactList : public QList<ContactItem>
{
public:
    ContactList();
    int findById(const QString& idValue);
    void compareWith(ContactList& pairList);
    void clear();
    QString statistics();
    MPBExtra extra;
};

#endif // CONTACTLIST_H
