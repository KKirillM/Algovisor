#pragma once

#include "Plaza2_defines.h"

//интерфейс данного класса не является потокобезопасным

struct P2Price;
class IPlaza2ConnectorType2;
class IListener;

//
class GetValuesHelper
{
public:
	typedef std::pair<int64_t, int8_t> PricePair;

	GetValuesHelper(IPlaza2ConnectorType2* _connector);
	~GetValuesHelper();

    static CG_RESULT EmptyCallback(const int /*type*/, msg_t* /*data*/, IListener* /*flow*/) { return CG_ERR_OK; }
    static CG_RESULT SetStopFlagCallback(const int type, msg_t* data, IListener* flow);
    static CG_RESULT GetIsinIdCallback(msg_streamdata_t* data, IListener* flow);
	static CG_RESULT GetTickerCallback(msg_streamdata_t* data, IListener* flow);
	static CG_RESULT GetAllContentsCallback(msg_streamdata_t* data, IListener* flow);
	static CG_RESULT GetInstrumentCallback(msg_streamdata_t* data, IListener* flow);

private:
	IPlaza2ConnectorType2* connector;
};

