#include "stdafx.h"
#include "CheckScheme.h"
bool SchemeBeginsWithEventFields(P2DB_TABLE_SCHEME_HANDLE sch) 
{ 
	// rev, type, size
	size_t field_size[3] = {8, 4, 4};
	P2DBFieldInfo fi; 
	for (size_t i = 0; i != 3; ++i)
	{
		if (P2FAILED(P2SchemeGetFieldInfoByIndex(sch, i, &fi)) || fi.size != field_size[i])
		{
			return false; 
		}
	}
	return true;
}
