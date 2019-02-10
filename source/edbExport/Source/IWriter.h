#ifndef _IWRITER_H__INCLUDED_
#define _IWRITER_H__INCLUDED_

#include <P2SysTypes.hpp>
#include <P2eReplTypes.h>

class IWriter
{
public:
    virtual ~IWriter() { };
    virtual P2ERR Init(const string& basename, unsigned splitBy, bool replaceNonPrintingChars,
        bool ignoreUnknownEvents, const string& schemaIni, const string& schemaName, bool timeHeader) = 0;
    virtual void Deinit() = 0;
    virtual P2ERR WriteEvent(const TEReplRec* pRec, INT64 offset) = 0;
    virtual P2ERR WriteCommit() = 0;
};

#endif // _IWRITER_H__INCLUDED_
