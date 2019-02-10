#ifndef _P2DBSTUFF_H_ 
#define _P2DBSTUFF_H_
/*

  � ���� ����� ���������� ������� ��� �������� ���������.

  �����! ��� �������, ������������ STL �� ������ ���������� ������� DLL.

 */


////////////////////////////////////////////////////////////////////////////////
// where ����� ���� NULL. � ���� ������ ���� �������� �� ������������
inline P2ERR CreateDeleteSQLStatement(const char *name, P2DBWhere *where, std::string& sql)
{
	sql = "DELETE FROM \"";
	sql += name;
	if (where != NULL)
	{ // �� ���� �� ������� � ������, �� ����� ������� �����
		char buf[P2DB_MAX_SQL_STATEMENT_LENGTH];
		char *p = NULL;
		size_t size = sizeof(buf);
		P2ERR err = where->GetSQL(buf, &size);
		if (err == P2ERR_COMMON_BUFFER_TOO_SMALL)
		{
			p = static_cast<char *>(malloc(size));
			if (p == NULL)
			{
				return P2ERR_COMMON_NO_MEMORY;
			}
			err = where->GetSQL(p, &size);
			if (err != P2ERR_OK)
			{
				free(p);
				return err;
			}
		}
		else if (err != P2ERR_OK)
		{
			return err;
		}
		sql += std::string("\" WHERE ") + (p ? p : buf);
		if (p != NULL)
		{
			free(p);
		}
	}
    else
        sql += '"';
	return P2ERR_OK;
}

// ����� ��������� ������ �������:
//   - ���� *ppHeapBuf != 0, �� ������ �������� � *ppHeapBuf, ����� ��� �������� � pBuf
//   - ���� *ppHeapBuf != 0, �� ����� ������������� ����� ������� free(*ppHeapBuf)
template <class ClausePart>
inline P2ERR GetSqlFromClausePart(ClausePart* pClause, char* pBuf, size_t* pSize, char** ppHeapBuf)
{
    P2ERR err = pClause->GetSQL(pBuf, pSize);
    if (err == P2ERR_COMMON_BUFFER_TOO_SMALL)
    {
	    *ppHeapBuf = static_cast<char*>(malloc(*pSize));
	    if (*ppHeapBuf == NULL)
        {
            return P2ERR_COMMON_NO_MEMORY;
        }
	    err = pClause->GetSQL(*ppHeapBuf, pSize);
	    if (P2FAILED(err))
        {
            free(*ppHeapBuf);
        }
    }
    else
    {
        *ppHeapBuf = 0;
    }
	return err;
}

// set ������ ���� �����, where ����� ���� NULL
inline P2ERR CreateUpdateSQLStatement(const char *name, P2DBSet *set, P2DBWhere *where,  std::string& sql)
{
	sql = "UPDATE \"";
	sql += name;
    sql += "\" SET ";

	char buf[P2DB_MAX_SQL_STATEMENT_LENGTH];

    size_t size = sizeof(buf);
    char* pHeapBuf = 0;
    RETURN_P2ERR(GetSqlFromClausePart(set, buf, &size, &pHeapBuf));
    sql += (pHeapBuf ? pHeapBuf : buf);
    if (pHeapBuf) free(pHeapBuf);

    if (where)
    {
        size = sizeof(buf);
        pHeapBuf = 0;
        RETURN_P2ERR(GetSqlFromClausePart(where, buf, &size, &pHeapBuf));
        sql += std::string(" WHERE ") + (pHeapBuf ? pHeapBuf : buf);
        if (pHeapBuf) free(pHeapBuf);
    }

    return P2ERR_OK;
}
////////////////////////////////////////////////////////////////////////////////

#endif // _P2DBSTUFF_H_
