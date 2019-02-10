#ifndef _P2TRACESHMEMSTORAGE_H_
#define _P2TRACESHMEMSTORAGE_H_

#include "../../P2Common/Include/P2Types.h"

#include <string>
#include <limits>
#include "../../P2SysTypes/Include/P2SysTypes.hpp"
#include "../../P2Utils/Include/CS.h"

#define P2TRACE_SHMEM_SIZE 128 * 1024 // 128 Kb
#if defined (_WIN32) || defined(__linux__)
#define P2TRACE_SHMEM_NAME_PREFIX "P2Trace_Shmem_"
#elif defined(__MACH__)
#define P2TRACE_SHMEM_NAME_PREFIX "P2" // max shmem name len is 31 for Mac
#endif // defined (_WIN32) || defined(__linux__)
// ��� ����� ���������� ������� ������ ��� ������� shared memory - ���������� ������
#define P2TRACE_SHMEM_VERSION "3" // ������, �.�. ��������� � ������������ ����� shared memory 

#if defined(__linux__) || defined(__MACH__)
#define P2LOG_SEVERITY_UNKNOWN (std::numeric_limits< UINT32 >::max())
#else
#define P2LOG_SEVERITY_UNKNOWN _UI32_MAX
#endif //defined(__linux__) || defined(__MACH__)

using std::string;

/*
��� �������������� ������:
|-------------------------------------------------------|
| CInterlockedLocker (volatile LONG) (4 �����)          |
|-------------------------------------------------------|
| ����_�_������_1 (��. CTraceStorageWalker)             |
|-------------------------------------------------------|
| ����_�_������_2 (��. CTraceStorageWalker)             |
|-------------------------------------------------------|
| ...                                                   |
|-------------------------------------------------------|
| ����_�_������_N (��. CTraceStorageWalker)             |
|-------------------------------------------------------|
| ������-"�����"  (��. CTraceStorageWalker)             |
|-------------------------------------------------------|
*/

// ��� ���������� � ������ �������� � ������.
//  {
//      INT32 traceState;    // ��������� ������. ������ ���� ��������� ����������� ���������� ��� ������� ������.
//      UINT32 severity;     // ��. P2SysLog.h. P2LOG_SEVERITY_UNKNOWN = -1 (����� ���� ����� ��������� ��� �� ���� �� ���������).
//      UINT32 categoryLen;  // ����� ������� ������ ����������� �������-���������� � ����������� �����. ������ 4 ������. ������� ���������� �����, ����� �� ���� ������� - 32 ��� 64 ����.
//      UINT32 fmtLen;       // ����� ������� ������ ����������� �������-�������� � ����������� �����. ������ 4 ������. ������� ���������� �����, ����� �� ���� ������� - 32 ��� 64 ����.
//      UINT32 sectionNameLen; // ����� ������� ������ ����������� ��������� ������ � ����������� �����. ������ 4 ������. ������� ���������� �����, ����� �� ���� ������� - 32 ��� 64 ����.
//      char data[];         // ������
//  }

class CP2TraceShmemStorage;

#define P2TRACE_STORAGE_TAIL_MARKER ((INT32)-2)

/////////////////////////////////////////////////////
class CTraceStorageWalker
{
    friend class CP2TraceShmemStorage;

public:
    inline INT32 getState() const { return m_p->traceState; }
    inline UINT32 getSeverity() const { return m_p->severity; }
    inline bool isTail() const { return P2TRACE_STORAGE_TAIL_MARKER == getState(); }
    inline volatile INT32* getPtr() const { return &m_p->traceState; }
    //inline UINT32 getCategoryLen() const { return m_p->categoryLen; }
    inline const char * getCategory() const { return m_p->data; }
    //inline UINT32 getFmtLen() const { return m_p->fmtLen; }
    inline const char * getFmt() const
    {
        return m_p->data + static_cast< ptrdiff_t >(m_p->categoryLen);
    }
    inline const char * getSectionName() const
    {
        return m_p->data + static_cast< ptrdiff_t >(m_p->categoryLen + m_p->fmtLen);
    }

    // �������� ������ ���� isTail() == false
    inline void Forward()
    {
        m_p = reinterpret_cast< TraceStateItem* >(m_p->data + static_cast< ptrdiff_t >(m_p->categoryLen + m_p->fmtLen + m_p->sectionNameLen));
    }

private:
    CTraceStorageWalker() : m_p(NULL) { }
    CTraceStorageWalker(void * p) : m_p( static_cast< TraceStateItem* >(p)) { }

    typedef struct TraceStateItem
    {
        volatile INT32 traceState;
        UINT32 severity;
        UINT32 categoryLen;
        UINT32 fmtLen;
        UINT32 sectionNameLen;
        char data[1];
    } TraceStateItem;

    inline void setCategory(const char * pCategory, size_t lenWithZero)
    {
        strcpy(m_p->data, pCategory);
        m_p->categoryLen = static_cast< UINT32 >((lenWithZero + 0x3) & ~(size_t)0x3);
    }

    inline void setCategory(const char * pCategory)
    {
        setCategory(pCategory, strlen(pCategory) + 1);
    }

    // �������� ����� ���������, ������ ����� ������ setCategory()
    inline void setFmt(const char * pFmt, size_t lenWithZero)
    {
        strcpy(m_p->data + static_cast< ptrdiff_t >(m_p->categoryLen), pFmt);
        m_p->fmtLen = static_cast< UINT32 >((lenWithZero + 0x3) & ~(size_t)0x3);
    }

    // �������� ����� ���������, ������ ����� ������ setCategory()
    inline void setFmt(const char * pFmt)
    {
        setFmt(pFmt, strlen(pFmt) + 1);
    }

    // �������� ����� ���������, ������ ����� ������ setCategory() � setFmt()
    inline void setSectionName(const char * pSectionName, size_t lenWithZero)
    {
        strcpy(m_p->data + static_cast< ptrdiff_t >(m_p->categoryLen + m_p->fmtLen), pSectionName);
        m_p->sectionNameLen = static_cast< UINT32 >((lenWithZero + 0x3) & ~(size_t)0x3);
    }

    // �������� ����� ���������, ������ ����� ������ setCategory() � setFmt()
    inline void setSectionName(const char * pSectionName)
    {
        setSectionName(pSectionName, strlen(pSectionName) + 1);
    }

    inline void setState(INT32 newState)
    {
        ::InterlockedExchange(reinterpret_cast< volatile LONG* >(&(m_p->traceState)), newState);
    }

    inline void setSeverity(UINT32 severity)
    {
        m_p->severity = severity;
    }

    inline static size_t calcStoreSize(size_t categoryLen, size_t fmtLen, size_t sectionNameLen)
    {
        return sizeof(TraceStateItem().traceState) + sizeof(TraceStateItem().severity) +
            sizeof(TraceStateItem().categoryLen) + sizeof(TraceStateItem().fmtLen) + sizeof(TraceStateItem().sectionNameLen) +
            ((categoryLen + 0x3) & ~(size_t)0x3) + ((fmtLen + 0x3) & ~(size_t)0x3) + ((sectionNameLen + 0x3) & ~(size_t)0x3);
    }

    TraceStateItem * m_p;
};

/////////////////////////////////////////////////////
class CP2TraceShmemStorage
{
public:
    CP2TraceShmemStorage();

    // ��� ������ ���������� ������ ������
    static string GenerateShmemName(const char * traceIniFileName, DWORD processId, /*OUT*/ UINT64 * pHash);

    // ������� ����������� ������
#if defined(__linux__) || defined(__MACH__)
    P2ERR Create(const char * fileMappingName);
#else
    P2ERR Create(const char * fileMappingName, const LPSECURITY_ATTRIBUTES lpAttributes);
#endif
    // ��������� ������������ ����������� ������
    P2ERR Open(const char * fileMappingName);

    void Close();

    // ������� ���������� � sharedmemory ���� ������� �� ���. ���������� ������ ���� ������� ���� �� ������ ������.
    P2ERR FillFromIni(P2INI_HANDLE hIni);

    volatile INT32 * AddOrUpdate(const char * pCategory, const char * pFmt, const char * pSectionName, INT32 state, UINT32 severity);

    // ������ �������� �� "�����" ���� �� �������
    const CTraceStorageWalker find(const char * pCategory, const char * pFmt, const char * pSectionName) const;

    // ���������� �������� �� ������
    const CTraceStorageWalker begin() const { return CTraceStorageWalker(const_cast< CInterlockedLocker * >(m_pLocker) + 1); }

private:
#if defined(__linux__) || defined(__MACH__)
    int m_hFileMapping;
	string m_sMappingName;
#else
    HANDLE m_hFileMapping;
#endif
    void * m_pMapView;
    CInterlockedLocker * m_pLocker;
    INT32 m_noMemorySubstitutor; // ����������, �� ������� ����� ��������� ��������� ���� � shared memory ������ ��� �����. ������ = 1

    inline void Init(bool bNew);

#ifdef _WIN32
    P2ERR CreateInternal(const char * fileMappingName, const LPSECURITY_ATTRIBUTES lpAttributes, /*OUT*/DWORD& osErr);
    bool GiveSeCreateGlobalPrivilege(); // ������� �������� ���������� SeCreateGlobalPrivilege
#endif // _WIN32

    struct EnumCbData
    {
        CP2TraceShmemStorage* pStorage;
        P2INI_HANDLE hIni;
        const char* section;  // ����������� � iniEnumSectionsCallback()
        const char* category; // ����������� � iniEnumSectionsCallback()
        CTraceStorageWalker* pW;
        EnumCbData(CP2TraceShmemStorage* _pStorage, P2INI_HANDLE _hIni, CTraceStorageWalker* _pW) :
            pStorage(_pStorage), hIni(_hIni), section(NULL), category(NULL), pW(_pW) { }
    };
    static P2ERR iniEnumSectionsCallback(const char* section, void* cbParams);
    static P2ERR iniEnumKeysCallbackWrap(const char* section, const char* key, const char* value, void* cbParams);
    P2ERR iniEnumKeysCallback(const char* fmt, const char* value, EnumCbData& data);
};

#endif // _P2TRACESHMEMSTORAGE_H_
