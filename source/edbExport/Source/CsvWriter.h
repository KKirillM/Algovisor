#ifndef _CSVWRITER_H__INCLUDED_
#define _CSVWRITER_H__INCLUDED_

#ifdef __unix__
#include <sys/stat.h>
#include <limits.h>
#define MAX_PATH PATH_MAX
#else
#include <direct.h>
#include <io.h>
#endif
#include <stdio.h>

#include "IWriter.h"

#include <P2SysScheme.hpp>
#include "./printers.h"

class CCsvWriter : public IWriter
{
public:
    CCsvWriter() : m_currFile(NULL), m_pPrintBuffer(NULL), m_timeHeader(false) { }
    virtual ~CCsvWriter();
    virtual P2ERR Init(const string& basename, unsigned splitBy, bool replaceNonPrintingChars,
        bool ignoreUnknownEvents, const string& schemaIni, const string& schemaName, bool timeHeader);
    virtual void Deinit();
    virtual P2ERR WriteEvent(const TEReplRec* pRec, INT64 offset);
    virtual P2ERR WriteCommit();

private:
    unsigned m_splitBy;
    string m_outputBaseFileName;
    bool m_replaceNonPrintingChars;
    bool m_bIgnoreUnknownEvents;
	bool m_timeHeader;

    char * m_pPrintBuffer;
    size_t m_printBufferSize;

    typedef struct _TEventField
    {
        P2DBFieldInfo fi;
        size_t fieldOffset;
        size_t maxSize;
        PRINTERFN printer;
    } TEventField;
    typedef std::vector< TEventField > TEventFields;
    typedef struct _TEvent
    {
        UINT32 type;
        string name;
        size_t structSize;
        TEventFields fields;
    } TEvent;
    typedef std::map< UINT32, TEvent > TEvents;
    TEvents m_events;

    FILE * m_currFile;
    unsigned m_writtenEvents; // into current file
    unsigned m_totalWrittenEvents;
    unsigned m_currFileNo;

    P2ERR LoadAndParseSchema(const string& schemaIni, const string& schemaName);
    P2ERR PrintEvent(const TEvent& eventInfo,  const TEReplRec* pEventData, size_t& outLen);
    P2ERR PrintEventNoScheme(const TEReplRec* pRec, size_t& outLen);
    P2ERR WriteBuffer(size_t bufferLen, bool bEvent);
    P2ERR CheckFile();
    P2ERR CloseCurrentFile();
};

#endif // _CSVWRITER_H__INCLUDED_
