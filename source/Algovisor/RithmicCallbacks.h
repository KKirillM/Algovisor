#pragma once

using namespace RApi;

class Instrument;
class RTicker;
class RithmicDialog;

class MyAdmCallbacks: public AdmCallbacks
{
public :
     MyAdmCallbacks()  {};
     ~MyAdmCallbacks() {};
	 virtual int Alert(AlertInfo* pInfo, void* pContext, int* aiCode) { *aiCode = API_OK; return (OK); }

};  // MyAdmCallbacks

class RithmicCallbacks : public RCallbacks
{
public:
	RithmicCallbacks(char** _envp, REngineParams& rp, LoginParams& lp, std::string& login, std::string& password);
	virtual ~RithmicCallbacks();

	 virtual int Alert(AlertInfo* pInfo, void* pContext, int* aiCode);
     virtual int AskQuote(AskInfo* pInfo, void* pContext, int* aiCode);
     virtual int BestAskQuote(AskInfo * pInfo,                              void *    pContext,                              int *     aiCode);
     virtual int BestBidQuote(BidInfo * pInfo,                              void *    pContext,                              int *     aiCode);
	 virtual int BestBidAskQuote(BidInfo * pBid,				 AskInfo * pAsk,				 void *    pContext,				 int *     aiCode);
     virtual int BidQuote(BidInfo * pInfo,                          void *    pContext,                          int *     aiCode);
     virtual int BinaryContractList(BinaryContractListInfo * pInfo,				    void *                   pContext,				    int *                    aiCode);
     virtual int ClosePrice(ClosePriceInfo * pInfo,                            void *           pContext,                            int *            aiCode);
     virtual int ClosingIndicator(ClosingIndicatorInfo * pInfo,                                  void *                 pContext,                                  int *                  aiCode);
     virtual int EndQuote(EndQuoteInfo * pInfo,                          void *         pContext,                          int *          aiCode);
     virtual int EquityOptionStrategyList(EquityOptionStrategyListInfo * pInfo,					  void *                         pContext,				  int *                          aiCode);
	 virtual int HighPrice(HighPriceInfo * pInfo,                           void *          pContext,                           int *           aiCode);
     virtual int InstrumentByUnderlying(InstrumentByUnderlyingInfo * pInfo,					void *                       pContext,					int *                        aiCode);
     virtual int LimitOrderBook(LimitOrderBookInfo* pInfo, void* pContext, int* aiCode);
     virtual int LowPrice(LowPriceInfo * pInfo,                          void *         pContext,                          int *          aiCode);
     virtual int MarketMode(MarketModeInfo * pInfo,                            void *           pContext,                            int *            aiCode);
     virtual int OpenInterest(OpenInterestInfo * pInfo,			      void *             pContext,			      int *              aiCode);
     virtual int OpenPrice(OpenPriceInfo * pInfo,                           void *          pContext,                           int *           aiCode);
     virtual int OpeningIndicator(OpeningIndicatorInfo * pInfo,				  void *                 pContext,				  int *                  aiCode);
     virtual int OptionList(OptionListInfo * pInfo,                            void *           pContext,                            int *            aiCode);
     virtual int RefData(RefDataInfo * pInfo,                         void *        pContext,                         int *         aiCode);
     virtual int SettlementPrice(SettlementPriceInfo * pInfo,                                 void *                pContext,                                 int *                 aiCode);
     virtual int Strategy(StrategyInfo * pInfo,			  void *         pContext,			  int *          aiCode);
     virtual int StrategyList(StrategyListInfo * pInfo,			      void *             pContext,			      int *              aiCode);
     virtual int TradeCondition(TradeInfo * pInfo,                                void *      pContext,                                int *       aiCode);
     virtual int TradePrint(TradeInfo * pInfo,                            void *      pContext,                            int *       aiCode);
     virtual int TradeReplay(TradeReplayInfo * pInfo,			     void *            pContext,			     int *             aiCode);
     virtual int TradeRoute(TradeRouteInfo * pInfo,			    void *           pContext,			    int *            aiCode);
     virtual int TradeRouteList(TradeRouteListInfo * pInfo,				void *               pContext,				int *                aiCode);
     virtual int TradeVolume(TradeVolumeInfo * pInfo,                             void *            pContext,                             int *             aiCode);
	 virtual int Bar(BarInfo* pInfo, void* pContext, int* aiCode);
	 virtual int BarReplay(BarReplayInfo* pInfo, void* pContext, int* aiCode);
     virtual int AccountList(AccountListInfo * pInfo,                             void *            pContext,                             int *             aiCode);
     virtual int PasswordChange(PasswordChangeInfo * pInfo,				void *               pContext,				int *                aiCode);
     virtual int ExchangeList(ExchangeListInfo * pInfo,			      void *             pContext,			      int *              aiCode);
     virtual int ExecutionReplay(ExecutionReplayInfo * pInfo,                                 void *                pContext,                                 int *                 aiCode);
     virtual int LineUpdate(LineInfo * pInfo,                            void *     pContext,                            int *      aiCode);
     virtual int OpenOrderReplay(OrderReplayInfo * pInfo,                                 void *            pContext,                                 int *             aiCode);
     virtual int OrderReplay(OrderReplayInfo * pInfo,                             void *            pContext,                             int *             aiCode);
     virtual int PnlReplay(PnlReplayInfo * pInfo,                           void *          pContext,                           int *           aiCode);
     virtual int PnlUpdate(PnlInfo * pInfo,                           void *    pContext,                           int *     aiCode);
     virtual int PriceIncrUpdate(PriceIncrInfo * pInfo,                                 void *          pContext,                                 int *           aiCode);
     virtual int ProductRmsList(ProductRmsListInfo * pInfo,				void *               pContext,				int *                aiCode);
     virtual int SingleOrderReplay(SingleOrderReplayInfo * pInfo,				   void *                  pContext,				   int *                   aiCode);
     virtual int BustReport(OrderBustReport * pReport,                            void *            pContext,                            int *             aiCode);
     virtual int CancelReport(OrderCancelReport * pReport,                              void *              pContext,                              int *               aiCode);
     virtual int FailureReport(OrderFailureReport * pReport,                               void *               pContext,                               int *                aiCode);
     virtual int FillReport(OrderFillReport * pReport,                            void *            pContext,                            int *             aiCode);
     virtual int ModifyReport(OrderModifyReport * pReport,                              void *              pContext,                              int *               aiCode);
     virtual int NotCancelledReport(OrderNotCancelledReport * pReport,                                    void *                    pContext,                                    int *                     aiCode);
     virtual int NotModifiedReport(OrderNotModifiedReport * pReport,                                   void *                   pContext,                                   int *                    aiCode);
     virtual int RejectReport(OrderRejectReport * pReport,                              void *              pContext,                              int *               aiCode);
     virtual int StatusReport(OrderStatusReport * pReport,                              void *              pContext,                              int *               aiCode);
     virtual int TradeCorrectReport(OrderTradeCorrectReport * pReport,                                    void *                    pContext,                                   int *                     aiCode);
     virtual int TriggerPulledReport(OrderTriggerPulledReport * pReport,                                     void *                     pContext,                                     int *                      aiCode);
     virtual int TriggerReport(OrderTriggerReport * pReport,                               void *              pContext,                               int *               aiCode);
     virtual int OtherReport(OrderReport * pReport,                             void *        pContext,                             int *         aiCode);
     virtual int SodUpdate(SodReport * pReport,                           void *      pContext,                           int *       aiCode);
     virtual int Quote(QuoteReport* pReport, void* pContext, int* aiCode);
	
	void SetValues(char** _envp, REngineParams& rp, LoginParams& lp, std::string& login, std::string& password);
	int Connect();
	void Disconnect();
	void IsRealTicker(tsNCharcb& ticker, tsNCharcb& exchange);
	void UnSubscribe(tsNCharcb& ticker, tsNCharcb& exchange);
	int ToInt(double x, const int p) const;
	void InsertInstrument(RTicker* i);

	char			errstr[100];
	wchar_t			werrstr[100];

	//***API variables***
	REngine*        pEngine;
	REngineParams   rEngineParams;
	char*           envp[11];
	LoginParams     oLoginParams;
	int             errorCode;

	// переменная необходима для пропуска изменений лимитных уровней, которые идут за трейдами 
	// std::pair<double, int> - double - цена, на которой закончился трейд, int - объём
	//std::map<std::string, std::pair<double, int>> limitAfterTradesFilter;

	int	marketDataConnectionStatus;
	int historicalDataConnectionStatus;
};


