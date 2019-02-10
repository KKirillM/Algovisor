#include "stdafx.h"
#include "Plaza2_Connector.h"
#include "IPlaza2_Connector.h"
#include "Plaza2_Tables.h"

#include "GetValuesHelper.h"

using namespace std;

extern volatile bool stopFlag;
extern volatile int global_int;
extern string global_string;
extern FORTS_FUTINFO_REPL::fut_sess_contents contents;
extern map<string, FORTS_FUTINFO_REPL::fut_sess_contents> allInstrumentContents;	//сделать одноразовое заполнение этого контейнера при подключении

GetValuesHelper::GetValuesHelper(IPlaza2ConnectorType2* _connector) : connector(_connector)
{
}

GetValuesHelper::~GetValuesHelper()
{
}

CG_RESULT GetValuesHelper::SetStopFlagCallback(int type, msg_t* data, IListener* flow)
{
    switch(type)
    {
    case CG_MSG_CLOSE: stopFlag = true; break;
	}

	return CG_ERR_OK;
}

CG_RESULT GetValuesHelper::GetInstrumentCallback(msg_streamdata_t* data, IListener* flow)
{
    using namespace FORTS_FUTINFO_REPL;

    fut_sess_contents& fs = *(fut_sess_contents*)data->data;
    if (global_string.compare(fs.isin) == 0)
    {
        contents = fs;
    }

    return CG_ERR_OK;
}

CG_RESULT GetValuesHelper::GetTickerCallback(msg_streamdata_t* data, IListener* flow)
{
    using namespace FORTS_FUTINFO_REPL;

    fut_sess_contents& fs = *(fut_sess_contents*)data->data;
    if (global_int == fs.isin_id)
    {
        global_string.assign(fs.isin);
        stopFlag = true;
    }

    return CG_ERR_OK;
}

CG_RESULT GetValuesHelper::GetIsinIdCallback(msg_streamdata_t* data, IListener* flow)
{
    using namespace FORTS_FUTINFO_REPL;

    fut_sess_contents& fs = *(fut_sess_contents*)data->data;
    if (global_string.compare(fs.isin) == 0)
    {
        global_int = fs.isin_id;
        stopFlag = true;
    }

    return CG_ERR_OK;
}

CG_RESULT GetValuesHelper::GetAllContentsCallback(msg_streamdata_t* data, IListener* flow)
{
	using namespace FORTS_FUTINFO_REPL;

	fut_sess_contents& fs = *(fut_sess_contents*)data->data;
	auto iter = allInstrumentContents.find(fs.isin);
	if (iter == allInstrumentContents.end())
	{
		allInstrumentContents.insert(make_pair(fs.isin, fs));
	}

	return CG_ERR_OK;
}
