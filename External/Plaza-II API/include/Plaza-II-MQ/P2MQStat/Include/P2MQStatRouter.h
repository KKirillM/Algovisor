#ifndef _P2MQSTAT_ROUTER_H_
#define _P2MQSTAT_ROUTER_H_

#ifdef P2MQSTAT_EXPORTS
#define P2MQSTAT_ROUTER_API __declspec(dllexport)
#else
#define P2MQSTAT_ROUTER_API __declspec(dllimport)
#endif

#define P2MQSTAT_ROUTER_APP_NAME "@MQ_STAT@"
#define P2MSG_CATEGORY_MQSTAT_ROUTER "P2_MQ_STAT"

#endif // _P2MQSTAT_ROUTER_H_
