
#ifndef _P2SYSIPARESR_H_
#define _P2SYSIPARESR_H_

#include "../../../Plaza-II/P2Utils/Include/IRc.h"



struct IP2SysComposer : public IRc
{
	virtual P2ERR SetOptions(UINT32 flags) = 0;

	virtual P2ERR Compose(const char *format, va_list args) = 0;
	virtual P2ERR ComposeAtIndex(size_t index, const char *format, va_list args) = 0;

	virtual P2ERR GetBuffer(const void **ppBuf) = 0;
	virtual P2ERR GetIndex(size_t *pIndex) = 0;

    // сбрасывать состояние, но не освобождать выделенный буфер
    virtual P2ERR Reset() = 0;
};


struct IP2SysParser : public IRc
{
	virtual P2ERR SetOptions(UINT32 flags) = 0;

	virtual P2ERR Init(const void *pBuf, size_t size) = 0;
	virtual P2ERR Reset(void) = 0;

	virtual P2ERR Parse(const char *format, va_list args) = 0;

	virtual P2ERR GetCurrentIndex(size_t *pIndex) = 0;

};


struct IP2SysParserManager
{
	virtual P2ERR GetComposer(IP2SysComposer **pComposer) = 0;
	virtual P2ERR GetParser(IP2SysParser **pParser) = 0;
};

#endif
