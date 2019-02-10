#ifndef _P2TBL_IP2ISAMTBL_H_
#define _P2TBL_IP2ISAMTBL_H_

#include "../../P2Tbl/Include/IP2Tbl.hpp"

// бэкдор в чистый исам. интерфейс к которому можно кастить хэндл таблицы для чистого исама. 
// в интерфейс будут добавляться функции, которые не имеет смысла выносить в P2Tbl
struct P2ISAMTblTable : public P2TblTable
{
	virtual P2ERR GetRecPtr(size_t indexId, const void *rec, const void **pTblRec) = 0;
};


#endif