//
//  IOSHttpSDK.h
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/21.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#ifndef iosstatsdk_IOSHttpSDK_h
#define iosstatsdk_IOSHttpSDK_h

#include "Poco/AtomicCounter.h"
#include "Poco/TaskManager.h"
#include "HttpEventArgs.h"
//#include "aprptstrategy.pb.h"
#include "Poco/Environment.h"
#include "Poco/PriorityNotificationQueue.h"
#include "Poco/NotificationQueue.h"
#include "Poco/Mutex.h"
#include "HttpSdkCallbackIntf.h"
#include "DefaultSdkCallback.h"
#include "RptStrategy.h"
//#include "mbactions.h"
//#include  "HttpSdkCallbackIntf.h"
#include <string>

class IOSHttpSDKHandle {
public:
    /**
     *
     */
    IOSHttpSDKHandle() {
        m_quitCalled = false;
    }
    
    ~IOSHttpSDKHandle() {
        if ( !m_quitCalled ) {
            this->quit();
        }
    }
    
    void appStartLaunchWithAppKey (
                                   const std::string &appkey
                                   , const std::string &channel
                                   , HttpSdkCallbackIntf *callback );
    
    /**
     *
     */
    void startupEvent();
    
    /**
     *
     */
    void beginLogPageView ( const std::string &pageName, const std::string &enterdata );
    void endLogPageView ( const std::string &pageName, const std::string &enterdata );
    void clickEvent ( const std::string &eventId );
    void beginEvent ( const std::string &pageName, const std::string &enterdata );
    void endEvent ( const std::string &pageName, const std::string &enterdata );
    void quit();
    void pubVideoCost ( const std::string &avgNetSpeed, const std::string &avgTimeCost );
    
    /**
     *
     */
    void onStrategyLoaded ( const void *pSender, HttpEventArgs &args );
    
    Poco::PriorityNotificationQueue &sendQueue() {
        return m_sendQueue;
    }
    
    Poco::NotificationQueue &failQueue() {
        return m_failedQueue;
    }
    
    const RptStrategy &rptStrategy() const {
        return m_AipaiRptStrategy;
    }
private:
    
    void sendData ( const std::string &data, const std::string &actionName );
    Poco::FastMutex m_mutex;
//    std::map < std::string, aipai::log::action::mbpageview *> m_eventPageViewMap;
//    std::map < std::string, aipai::log::action::mbcustevent *> m_eventCustEventMap;
    Poco::PriorityNotificationQueue m_sendQueue;
    Poco::NotificationQueue m_failedQueue;
    Poco::NotificationQueue m_cacheQueue;
    
    Poco::TaskManager m_taskManager;
    Poco::AtomicCounter m_AtomicCounter;
    RptStrategy m_AipaiRptStrategy;
    HttpSdkCallbackIntf *m_callback;
    std::string m_hdrString;
    DefaultSdkCallback m_NullSdkCallback;
    
    bool m_quitCalled;
};
#endif
