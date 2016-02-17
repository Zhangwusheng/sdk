//
//  RptStrategy.cpp
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/20.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#include "RptStrategy.h"
#include "SerDerUtil.h"
#include "RptUtil.h"
#include "Poco/StringTokenizer.h"
#include "Poco/NumberParser.h"

#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

#if DATA_SERDER_CUR == DATA_SERDER_KV
#include "StrategyProtocol_KV.cpp"
#else
#include "StrategyProtocol_PB.cpp"
#endif

#pragma mark - 策略中心 -

bool RptStrategy::loadLocalStrategy()
{
    string result;

    if (!RptUtil::getLocalStrategyString(result)) {
        return false;
    }
    
    try {
        m_protocol->deserialize(result);
        return true;
    }
    catch ( ... ) {
        return false;
    }
}

bool RptStrategy::loadHttpStrategy ( const std::string &httpResponse )
{
    try {
        m_protocol->deserialize(httpResponse);
        writeStrategyToLocal();
        return true;
    }
    catch ( ... ) {
        return false;
    }
    return false;
}

bool RptStrategy::writeStrategyToLocal()
{
    string base64 = m_protocol->serialize();
    return RptUtil::writeStrategyStringToLocal(base64);
}
