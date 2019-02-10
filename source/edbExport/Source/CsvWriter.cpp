#include "stdafx.h"
#include <stuff.h>
#include <P2SysRecord.hpp>
#include <CreateFileDir.hpp>

#include "./CsvWriter.h"
#include "./CheckScheme.h"

CCsvWriter::~CCsvWriter()
{
    if (NULL != m_currFile)
    {
        _ASSERT(false);
        Deinit();
    }
}

P2ERR CCsvWriter::Init(const string& basename, unsigned splitBy, bool replaceNonPrintingChars,
        bool ignoreUnknownEvents, const string& schemaIni, const string& schemaName, bool timeHeader)
{
    if (!m_outputBaseFileName.empty())
        return P2ERR_COMMON_ALREADY_INITIALIZED;
    m_pPrintBuffer = NULL;
    m_printBufferSize = 0;
    m_outputBaseFileName = basename;
    m_splitBy = splitBy;
    m_replaceNonPrintingChars = replaceNonPrintingChars;
    m_bIgnoreUnknownEvents = ignoreUnknownEvents;
    m_currFile = NULL;
    m_writtenEvents = 0;
    m_totalWrittenEvents = 0;
    m_currFileNo = 0;
	m_timeHeader = timeHeader;
    return LoadAndParseSchema(schemaIni, schemaName);
}

void CCsvWriter::Deinit()
{
    CloseCurrentFile();
    free(m_pPrintBuffer);
}

P2ERR CCsvWriter::LoadAndParseSchema(const string& schemaIni, const string& schemaName)
{
    P2ERR res;
    AutoSchemeHandle hSchemaA;
    if (P2FAILEDV(P2SchemeCreateSchemeFromIni2(&hSchemaA, schemaIni.c_str(), schemaName.c_str()), res))
    {
        P2LogErrorWithCodeEx(res, LOG_CAT_, "Cannot open schema '%s' from file '%s'", schemaName.c_str(), schemaIni.c_str());
        return res;
    }
    size_t reqStrBuffer = 0;
    size_t tblCount = 0;
    RETURN_P2ERR(P2SchemeGetTableCount(hSchemaA, &tblCount));
    for (size_t i = 0; i < tblCount; ++i)
    {
        AutoTableSchemeHandle hTblSchemaA;
        P2SchemeGetTableByIndex(hSchemaA, i, &hTblSchemaA);
        char tableName[P2DB_MAX_TABLE_NAME_SIZE];
        size_t sz = sizeof(tableName);
        RETURN_P2ERR(P2SchemeGetTableName(hTblSchemaA, tableName, &sz));
        size_t colCount;
        RETURN_P2ERR(P2SchemeGetFieldCount(hTblSchemaA, &colCount)); 
		if (m_timeHeader)
			colCount += 2;
		/*if (!SchemeBeginsWithEventFields(hTblSchemaA))
		{
			P2LogErrorEx(LOG_CAT_, "Scheme doesn't begin with event header; tbl '%s'", tableName);
			return P2ERR_COMMON_WRONG_ARGUMENTS; 
		} */

        char strEventType[32];
        size_t szET = sizeof(strEventType);
        if (P2FAILEDV(P2SchemeGetCustomField(hTblSchemaA, "msgid", strEventType, &szET), res))
        {
            if (P2FAILEDV(P2SchemeGetCustomField(hTblSchemaA, "eventType", strEventType, &szET), res))
            {
                P2LogErrorWithCodeEx(res, LOG_CAT_, "Cannot get custom field 'msgid'\\'eventType'; tbl '%s'", tableName);
                return res;
            }
        }

        TEvent eventInfo;
        eventInfo.name.assign(tableName);
        int radix = (szET > 2 && '0' == strEventType[0] && ('x' == strEventType[1] || 'X' == strEventType[1])) ? 16 : 10;
        eventInfo.type = static_cast< UINT32 >(P2AtoI64(strEventType, radix));
        eventInfo.fields.resize(colCount);

        AutoRecordHandle recA;
        std::vector< void* > recFields;
		size_t schemeColCount = m_timeHeader ? colCount-TIMEFIELDS_COUNT : colCount;
        recFields.resize(schemeColCount);
        size_t structSize;
        if (P2FAILEDV(P2RecordCreateInMemoryEx(&recA, hTblSchemaA, &recFields[0], &structSize, NULL), res))
        {
            P2LogErrorWithCodeEx(res, LOG_CAT_, "P2RecordCreateInMemoryEx failed; table '%s'", tableName);
            return res;
        }
        eventInfo.structSize =  m_timeHeader ? structSize+TIMEFIELDS_SIZE : structSize;

		if (m_timeHeader)
		{
			eventInfo.fields[0].fi.index = 0;
			P2SysSNPrintf(eventInfo.fields[0].fi.name, P2DB_MAX_FIELD_NAME_SIZE, "curr_time");
			eventInfo.fields[0].fi.precision = 0;
			eventInfo.fields[0].fi.size = 8;
			P2SysSNPrintf(eventInfo.fields[0].fi.type, P2DB_MAX_TYPE_NAME_SIZE, "i8");
			eventInfo.fields[0].fieldOffset = 0;
			eventInfo.fields[0].maxSize = 8;
			eventInfo.fields[0].printer = printer_P2Time;

			eventInfo.fields[1].fi.index = 1;
			P2SysSNPrintf(eventInfo.fields[1].fi.name, P2DB_MAX_FIELD_NAME_SIZE, "timer_time");
			eventInfo.fields[1].fi.precision = 0;
			eventInfo.fields[1].fi.size = 8;
			P2SysSNPrintf(eventInfo.fields[1].fi.type, P2DB_MAX_TYPE_NAME_SIZE, "i8");
			eventInfo.fields[1].fieldOffset = 8;
			eventInfo.fields[1].maxSize = 8;
			eventInfo.fields[1].printer = P2TypeToStr;
		}

        // размер буфера под строку
        // множитель 10 взят с большим запасом (думаю, достаточно и 4) - перевод двоичного типа в строку
        reqStrBuffer = std::max(reqStrBuffer, structSize * 10 + colCount * 2 /*кавычки*/ + 200 /*тип строки,ревижен,тип эвента*/);

        for (size_t j = 0, fieldNumber = m_timeHeader ? TIMEFIELDS_COUNT : 0; j < schemeColCount; ++j, ++fieldNumber)
        {
            P2DBFieldInfo& fi = eventInfo.fields[fieldNumber].fi;
            if ( P2FAILEDV(P2SchemeGetFieldInfoByIndex(hTblSchemaA, j, &fi), res) )
            {
                P2LogErrorWithCodeEx(res, LOG_CAT_, "Couldn't get field info;table '%s';field %P", tableName, j);
                return res;
            }

            eventInfo.fields[fieldNumber].fieldOffset = reinterpret_cast< char* >(recFields.at(fi.index)) - reinterpret_cast< char* >(recFields.at(0));
			if (m_timeHeader)
				eventInfo.fields[fieldNumber].fieldOffset += TIMEFIELDS_SIZE;
            size_t sizeInSource, dummy;
            if (P2FAILEDV(P2SysTypeGetSizeAndAlign(fi.type[0], fi.size, fi.precision, &sizeInSource, &dummy), res))
            {
                P2LogErrorWithCodeEx(res, LOG_CAT_, "Cannoе get type size in memory;table '%s';field %P", tableName, j);
                return res;
            }
            eventInfo.fields[fieldNumber].maxSize = sizeInSource;

            switch( fi.type[0] ) 
            {
                T_CASE_B
                T_CASE_Z
                    eventInfo.fields[fieldNumber].printer = printerBinary;
                break;
                T_CASE_C
                    eventInfo.fields[fieldNumber].printer = m_replaceNonPrintingChars ? printerC_replaceNonPrint : printerC;
                break;
                T_CASE_W
                    eventInfo.fields[fieldNumber].printer = printerW; // юникод выводится как бинарный блок
                break;
                T_CASE_T
                    eventInfo.fields[fieldNumber].printer = printerT;
                break;
                T_CASE_A
                    eventInfo.fields[fieldNumber].printer = m_replaceNonPrintingChars ? printerA_replaceNonPrint : printerA;
                break;
                T_CASE_I
                T_CASE_U
                T_CASE_D
                T_CASE_S
                T_CASE_F
                    eventInfo.fields[fieldNumber].printer = P2TypeToStr;
                break;
            default:
                T_CHECK
                    return P2ERR_COMMON_INTERNAL_ERROR;
            }
        }
        m_events[eventInfo.type] = eventInfo;
    }
    // выделение буфера для печати строки
    m_pPrintBuffer = static_cast< char * >(malloc(reqStrBuffer));
    if (!m_pPrintBuffer)
    {
        return P2ERR_COMMON_NO_MEMORY;
    }
    m_printBufferSize = reqStrBuffer;
    return P2ERR_OK;
}

P2ERR CCsvWriter::WriteEvent(const TEReplRec* pRec, INT64 offset)
{
    P2ERR res;
    size_t outLen;
    TEvents::const_iterator eiIt = m_events.find(pRec->eventType);
    if (m_events.end() != eiIt)
    {
		size_t size = m_timeHeader ? TEReplRec::getSize(pRec->dataSize - sizeof(TEReplRec)) : TEReplRec::getSize(pRec->dataSize);
        if (eiIt->second.structSize != size)
        {
            P2LogErrorEx(LOG_CAT_, "Wrong event data size %P; type %u, rev " P2EREPL_REVISION_PRINTF_FORMAT " at offset %I64d",
                TEReplRec::getSize(pRec->dataSize), pRec->eventType, pRec->rev, offset);
            return P2ERR_COMMON_WRONG_ARGUMENTS;
        }
        if ( P2FAILEDV(PrintEvent(eiIt->second, pRec, outLen), res) ) {
            return res;
        }
    }
    else
    {
        if (!m_bIgnoreUnknownEvents) {
            P2LogErrorEx(LOG_CAT_, "Unknown event type %u, rev " P2EREPL_REVISION_PRINTF_FORMAT " at offset %I64d",
                pRec->eventType, pRec->rev, offset);
            return P2ERR_COMMON_WRONG_ARGUMENTS;
        }
        if ( P2FAILEDV(PrintEventNoScheme(pRec, outLen), res) ) {
            return res;
        }
    }
    return WriteBuffer(outLen, true);
}

P2ERR CCsvWriter::PrintEvent(const TEvent& eventInfo, const TEReplRec* pRec, size_t& outLen)
{
	//  Формат вывода. 
	// "DATA", "rev", "type", "event_field3", "event_field4" ...
	//  Поле "dataSize" пропускается для совместимости со старыми форматами.
    P2ERR res;
    m_pPrintBuffer[0] = 0;
    strcpy(m_pPrintBuffer, "\"DATA\",");
    size_t curLen = 7;
    curLen += P2SysSNPrintf(m_pPrintBuffer + curLen, static_cast< int >(m_printBufferSize - curLen), "\"" P2EREPL_REVISION_PRINTF_FORMAT "\",\"%u\"", pRec->rev, pRec->eventType);
    size_t fldCnt = eventInfo.fields.size();

	size_t startColumn = m_timeHeader ? 0 : 3;
	size_t offset = m_timeHeader ? sizeof(TEReplRec) : 0;
    for (size_t j = startColumn; j < fldCnt; ++j)
    {
        m_pPrintBuffer[curLen] = ',';
        m_pPrintBuffer[curLen + 1] = '"';
        m_pPrintBuffer[curLen + 2] = 0;
        curLen += 2;
        const TEventField& ef = eventInfo.fields[j];
        size_t sz = m_printBufferSize - curLen;
        if (P2FAILEDV(ef.printer(m_pPrintBuffer + curLen, &sz,
            ef.fi.type, ef.fi.size, ef.fi.precision, reinterpret_cast< const char * >(pRec) + ef.fieldOffset + offset, ef.maxSize, 0), res))
        {
            P2LogErrorWithCodeEx(res, LOG_CAT_, "Couldn't print field %P; rev " P2EREPL_REVISION_PRINTF_FORMAT, j, pRec->rev);
            return res;
        }
        curLen += sz - 1;
        m_pPrintBuffer[curLen++] = '"';
        m_pPrintBuffer[curLen] = 0;
    }
    m_pPrintBuffer[curLen++] = '\n';
    m_pPrintBuffer[curLen] = 0;
    outLen = curLen;
    return P2ERR_OK;
}

P2ERR CCsvWriter::PrintEventNoScheme(const TEReplRec* pRec, size_t& outLen)
{
    P2ERR res;
    // estimate required buffer size
    size_t reqStrBuffer = pRec->dataSize * 2 + 200;
    // reallocate if need
    if (reqStrBuffer > m_printBufferSize) {
        m_pPrintBuffer = static_cast< char * >(realloc(m_pPrintBuffer, reqStrBuffer));
        if (!m_pPrintBuffer)
        {
            return P2ERR_COMMON_NO_MEMORY;
        }
        m_printBufferSize = reqStrBuffer;
    }

    m_pPrintBuffer[0] = 0;
    strcpy(m_pPrintBuffer, "\"DATA-RAW\",");
    size_t curLen = 11;
    curLen += P2SysSNPrintf(m_pPrintBuffer + curLen, static_cast< int >(m_printBufferSize - curLen),
        "\"" P2EREPL_REVISION_PRINTF_FORMAT "\",\"%u\",\"", pRec->rev, pRec->eventType);
    size_t sz = m_printBufferSize - curLen;
    if (P2FAILEDV(printerBinary(m_pPrintBuffer + curLen, &sz,
        "b", 0, 0, reinterpret_cast<const char*>(pRec) + sizeof(TEReplRec), pRec->dataSize, 0), res))
    {
        P2LogErrorWithCodeEx(res, LOG_CAT_, "Couldn't print event w/o scheme; rev " P2EREPL_REVISION_PRINTF_FORMAT, pRec->rev);
        return res;
    }
    curLen += sz - 1;
    m_pPrintBuffer[curLen++] = '"';
    m_pPrintBuffer[curLen++] = '\n';
    m_pPrintBuffer[curLen] = 0;
    outLen = curLen;
    return P2ERR_OK;
}

P2ERR CCsvWriter::WriteCommit()
{
	P2ERR err = P2ERR_OK;
	if (!m_timeHeader)	//мне не нужны эти сообщения в выходном файле
	{
		strcpy(m_pPrintBuffer, "\"COMMIT\"\n");
		err = WriteBuffer(9, false);
	}
    return err;
}

P2ERR CCsvWriter::WriteBuffer(size_t bufferLen, bool bEvent)
{
    RETURN_P2ERR(CheckFile());
    if (sizeof(char) * bufferLen != fwrite(m_pPrintBuffer, sizeof(char), bufferLen, m_currFile))
    {
        P2LogErrorEx(LOG_CAT_, "Couldn't write %P bytes", sizeof(char) * bufferLen);
        return P2ERR_COMMON_SYSCALL_FAILED;
    }
    if (bEvent)
    {
        ++m_writtenEvents;
        ++m_totalWrittenEvents;
    }
    //P2TRACE_INFO(( "%s", m_pPrintBuffer))
    return P2ERR_OK;
}

P2ERR CCsvWriter::CheckFile()
{
    if (m_splitBy > 0 && m_writtenEvents >= m_splitBy)
    {
        RETURN_P2ERR(CloseCurrentFile());
    }
    if (NULL == m_currFile)
    {
        char filename[MAX_PATH];
        int printRes;
        if (m_currFileNo > 99999)
        {
            P2LogErrorEx(LOG_CAT_, "Too many files were generated. Try to increase events per file value (current: %u)", m_splitBy);
            return P2ERR_COMMON_INTERNAL_ERROR;
        }
        if (0 == m_splitBy)
        {
            printRes = P2SysSNPrintf(filename, sizeof(filename), "%s.csv", m_outputBaseFileName.c_str());
        }
        else
        {
            printRes = P2SysSNPrintf(filename, sizeof(filename), "%s.%05u.csv", m_outputBaseFileName.c_str(), ++m_currFileNo);
        }
        if (printRes <= 0)
        {
            P2LogErrorEx(LOG_CAT_, "Couldn't create file name; baseName '%s', number %u", m_outputBaseFileName.c_str(), m_currFileNo);
            return P2ERR_COMMON_INTERNAL_ERROR;
        }
        if (!CreateFileDir(filename))
        {
            P2LogErrorEx(LOG_CAT_, "CreateFileDir failed");
            return P2ERR_COMMON_SYSCALL_FAILED;
        }
        m_currFile = fopen(filename, "wtS");
        if (NULL != m_currFile)
        {
            P2TRACE_INFO_EX((LOG_CAT_, "Created output file '%s'", filename))
        }
        else
        {
            P2LogErrorEx(LOG_CAT_, "Couldn't create file '%s' for writing", filename);
            return P2ERR_COMMON_SYSCALL_FAILED;
        }
    }
    return P2ERR_OK;
}

P2ERR CCsvWriter::CloseCurrentFile()
{
    P2ERR res = P2ERR_OK;
    if (NULL != m_currFile)
    {
        if (0 != fflush(m_currFile))
        {
            P2LogErrorEx(LOG_CAT_, "fflush() failed");
            res = P2ERR_COMMON_SYSCALL_FAILED;
        }
        if (0 != fclose(m_currFile))
        {
            P2LogErrorEx(LOG_CAT_, "fclose() failed");
            res = P2ERR_COMMON_SYSCALL_FAILED;
        }
        else
        {
            P2TRACE_INFO_EX((LOG_CAT_, "Output file closed; %u total events exported", m_totalWrittenEvents))
        }
        m_currFile = NULL;
    }
    m_writtenEvents = 0;
    return res;
}
