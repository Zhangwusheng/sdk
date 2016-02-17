//
//  RptHdrObj.h
//  aprios
//
//  Created by zhangwusheng on 15/8/22.
//  Copyright (c) 2015年 adw. All rights reserved.
//

#ifndef __aprios__RptHdrObj__
#define __aprios__RptHdrObj__

#include "GlobalConfig.h"
#include "HttpSdkCallbackIntf.h"
#include "Poco/Environment.h"

class RptHdrObj {
public:
    std::string toString();
    
    std::string m_appkey;
    std::string m_channel;
    std::string m_osVersion;
    std::string m_osName;
    std::string m_osType;
    std::string m_machindId;
    std::string m_osVendor;
    
    std::string m_statGuid;
    HttpSdkCallbackIntf* m_SdkCallback;
};

#endif /* defined(__aprios__RptHdrObj__) */
