//
//  HttpSdkCallbackIntf.h
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/20.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#ifndef iosstatsdk_HttpSdkCallbackIntf_h
#define iosstatsdk_HttpSdkCallbackIntf_h

#include <string>

class HttpSdkCallbackIntf {
public:
    virtual ~HttpSdkCallbackIntf() {}
    virtual std::string getLoaderVer() = 0;
    virtual std::string getAddonVer() = 0;
    virtual std::string getAppVer() = 0;
    virtual std::string getGameId() = 0;
    virtual std::string getUserId() = 0;
    virtual std::string getMachineId() = 0;
};

#endif
