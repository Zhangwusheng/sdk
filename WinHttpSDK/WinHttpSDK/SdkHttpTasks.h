// WinHttpTest.cpp : 定义控制台应用程序的入口点。
//
#ifndef SdkHttpTasks_H
#define SdkHttpTasks_H

#include "SdkHttpRequests.h"
#include "WinHttpSDK.h"

#include "Poco/SharedPtr.h"
#include "Poco/Delegate.h"
#include "Poco/PriorityNotificationQueue.h"


class HttpStrategyTask : public Poco::Task {
    public:
        HttpStrategyTask ( WinHttpSDK *sdk ) : Poco::Task ( "HttpStrategyTask" ) {
            m_sdk = sdk;
            m_StrategyHttpRequest = new StrategyHttpRequest(this);
            m_StrategyHttpRequest->RequestFinished += delegate ( m_sdk, &WinHttpSDK::onStrategyLoaded );
        }

        void runTask() {
            m_StrategyHttpRequest->execute();
        }

    private:
        Poco::SharedPtr<StrategyHttpRequest> m_StrategyHttpRequest;
        WinHttpSDK *m_sdk;
};

class HttpSendQueuDataTask : public Poco::Task {
    public:
        HttpSendQueuDataTask ( WinHttpSDK *sdk )
            : Poco::Task ( "HttpSendQueuDataTask" )
            , m_sendQueue ( sdk->sendQueue() )
            , m_failQueue ( sdk->failQueue() ) {
            m_sdk = sdk;
            m_nTrys = m_sdk->rptStrategy().getHttpRetryNum();
        }

        void runTask();
    private:
        Poco::PriorityNotificationQueue &m_sendQueue;
        Poco::NotificationQueue &m_failQueue;
        int m_nTrys;
        WinHttpSDK *m_sdk;
};

class HttpFailedQueuDataTask : public Poco::Task {
    public:
        HttpFailedQueuDataTask ( WinHttpSDK *sdk )
            : Poco::Task ( "HttpFailedQueuDataTask" )
            , m_failQueue ( sdk->failQueue() ) {
            m_sdk = sdk;
        }
        void runTask();
    private:
        Poco::NotificationQueue &m_failQueue;
        WinHttpSDK *m_sdk;
};
#endif
