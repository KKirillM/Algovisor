#pragma once

#pragma warning(push)
#pragma warning( disable : 4251 )
#include "Plaza2_Connector.h"
#pragma warning(pop)

#pragma warning(push)
#pragma warning( disable : 4121 )
#include "Plaza2_Tables.h"
#pragma warning(pop)


class Plaza2Callbacks
{
    static const char* moduleName;
    static int sess_id;

public:
	Plaza2Callbacks();
	~Plaza2Callbacks();

	/*static CG_RESULT FORTS_DEALS_REPL_Callback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT FORTS_ORDLOG_REPL_Callback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT FORTS_FUTINFO_REPL_Callback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT FORTS_OPTINFO_REPL_Callback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT FORTS_FUTCOMMON_REPL_Callback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT FORTS_OPTCOMMON_REPL_Callback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT FORTS_MM_REPL_Callback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT RTS_INDEX_REPL_Callback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT FORTS_MISCINFO_REPL_Callback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT FORTS_VOLAT_REPL_Callback(msg_streamdata_t* data, IListener* listener);*/

    static CG_RESULT StreamDataCallback(msg_streamdata_t* data, IListener* listener);
	static CG_RESULT MsgTypeCallback(const int type, msg_t* data, IListener* listener);
};

