//
//  IOSHttpRequest.h
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/21.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#ifndef iosstatsdk_IOSHttpRequest_h
#define iosstatsdk_IOSHttpRequest_h


#include <string>

#include "Poco/BasicEvent.h"
//#include "HttpEventArgs.h"
#include "RptStrategy.h"
//#import "AFNetworking.h"

class IOSHttpRequest
{
public:
    enum RequestType {
        GET = 0,
        POST = 1
    };
    
    
    IOSHttpRequest (int method, const std::string &url, const std::string &data = "" ) {
        m_method = method;
        m_url = url;
        m_data = data;
//        m_manager = manager;
    }

    virtual ~IOSHttpRequest() {}
    
    bool execute();
    
    std::string getLastError() {
        return m_error;
    };
protected:
    int m_method;
    std::string m_url;
    std::string m_data;
    std::string m_error;
    std::string m_response;
//    AFHTTPRequestOperationManager* m_manager;
};

class ActionDataHttpRequest: public IOSHttpRequest {
public:
    ActionDataHttpRequest (const std::string &data ) :
        IOSHttpRequest ( IOSHttpRequest::POST, "http://mlog.aipai.com/i.gif", data ) {}
};

class FailedActionDataHttpRequestBase {
public:
    FailedActionDataHttpRequestBase ( RptStrategy *strategy ) {
        m_IpUrl = "http://" + strategy->getRandomIPStr() + "/i.gif";
    }
protected:
    std::string m_IpUrl;
};

class FailedActionDataHttpRequest
:       private FailedActionDataHttpRequestBase,
        public IOSHttpRequest {
public:
    FailedActionDataHttpRequest (RptStrategy *strategy, const std::string &data ) :
    FailedActionDataHttpRequestBase ( strategy ),
    IOSHttpRequest ( IOSHttpRequest::POST, m_IpUrl, data ) {}
};

#endif
