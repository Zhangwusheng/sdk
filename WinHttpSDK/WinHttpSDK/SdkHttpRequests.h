// WinHttpTest.cpp : 定义控制台应用程序的入口点。
//
#ifndef SdkHttpRequests_H
#define SdkHttpRequests_H

#include <windows.h>
#include <Winhttp.h>
#include <string>
#include <vector>
#include "WinHttpRequest.h"
#include "RptStratrgy.h"


class StrategyHttpRequest: public WinHttpRequest {
    public:
        StrategyHttpRequest(Poco::Task* task) :
			WinHttpRequest(WinHttpRequest::GET, "http://trans.mlog.aipai.com/kvinfo.php", "", task) {}
};

class ActionDataHttpRequest: public WinHttpRequest {
    public:
        ActionDataHttpRequest ( const std::string &data ) :
            WinHttpRequest ( WinHttpRequest::POST, "http://mlog.aipai.com/i.gif", data ) {}
};

class FailedActionDataHttpRequestBase {
    public:
        FailedActionDataHttpRequestBase (  RptStrategy *strategy ) {
            m_IpUrl = "http://" + strategy->getRandomIPStr() + "/i.gif";
        }
    protected:
        std::string m_IpUrl;
};

class FailedActionDataHttpRequest
    : 	private FailedActionDataHttpRequestBase,
       public WinHttpRequest {
    public:
        FailedActionDataHttpRequest (  RptStrategy *strategy, const std::string &data ) :
            FailedActionDataHttpRequestBase ( strategy ),
            WinHttpRequest ( WinHttpRequest::POST, m_IpUrl, data ) {}
};
#endif
