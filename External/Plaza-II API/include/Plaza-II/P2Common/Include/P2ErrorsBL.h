#ifndef _P2ERRORSBL_H_
#define _P2ERRORSBL_H_

//
// При добавлении нового кода ошибки необходимо 
// (Plaza-II\P2SysTypes\Source\P2ErrGetDescr.cpp):35
//

#include "P2Errors.h"

// P2ERR_BL
enum{
    // BL Errors
    P2ERR_BL_UNKNOWN_MESSAGE = P2ERR_BL_BEGIN, // Couldn't create socket
    P2ERR_BL_CAN_NOT_ADD_GROUP_AREA,              // Can't add group area
    P2ERR_BL_UNKNOWN_GROUP_AREA,                  // Unknown group area
    P2ERR_BL_INDEX_DOES_NOT_EXIST,                // Index doesn't exist
    P2ERR_BL_REPLICATION_SCHEME_IS_NOT_SPECIFIED, // Replication scheme is not specified
    P2ERR_BL_CAN_NOT_CREATE_TIMER,                // Can't create timer
    P2ERR_BL_ARRAY_OUT_OF_BOUNDS                  // Array out of bounds
};


//
// При добавлении нового кода ошибки необходимо 
// (Plaza-II\P2SysTypes\Source\P2ErrGetDescr.cpp):35
//

#endif // _P2ERRORSBL_H_