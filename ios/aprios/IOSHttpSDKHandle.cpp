//
//  IOSHttpSDK.cpp
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/21.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#include "IOSHttpSDKHandle.h"


//#include "WinHttpSDK.h"
//#include "SdkHttpTasks.h"
#include "Poco/Thread.h"
#include "Poco/Base64Decoder.h"
#include "Poco/Base64Encoder.h"
#include "HttpDataNotification.h"
//#include "Wincrypt.h"
//#include "hdr.pb.h"
//#include "RptHdrObj.h"
#include "PBUtil.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace Poco;

void IOSHttpSDKHandle::onStrategyLoaded ( const void *pSender, HttpEventArgs &args )
{
    //cout << "Success? :" << ( args.success ? "SUCCESS" : "FAIL" )
    //     << ",Response :" << args.response << endl;
    
    bool success = args.success;
    if (  success ) {
        success = m_AipaiRptStrategy.loadHttpStrategy ( args.response ) ;
    }
    
    if ( !success ) {
        success =  m_AipaiRptStrategy.loadLocalStrategy();
    }
    
    if ( !success ) {
        success =  m_AipaiRptStrategy.loadDefaultStrategy();
    }
    
    this->m_AtomicCounter = 1;
    //cout << "onStrategyLoaded" << endl;
    
    int nTotal = 0;
    Notification::Ptr pNf = m_cacheQueue.dequeueNotification();
    while ( pNf.get() ) {
        HttpDataNotification::Ptr pNf2 = pNf.cast<HttpDataNotification>();
        if ( pNf2 ) {
            nTotal++;
            m_sendQueue.enqueueNotification ( pNf, pNf2->getPriority() );
        }
        
        pNf  = m_cacheQueue.dequeueNotification();
    }
    
    //cout << "Total " << nTotal << " records moved from cache to send queue" << endl;
    
    int n = m_AipaiRptStrategy.getSendThreadNum();
    for ( int i = 0; i < n; i++ ) {
        //cout << ">>>>> starting " << i << endl;
//        AutoPtr<HttpSendQueuDataTask> task = new HttpSendQueuDataTask ( this );
//        this->m_taskManager.start ( task.duplicate() );
        //cout << ">>>>> started " << i << endl;
    }
    
    //cout << "HttpSendQueuDataTask Finished" << endl;
    
//    AutoPtr<HttpFailedQueuDataTask> sendFailDataTask = new HttpFailedQueuDataTask ( this );
//    this->m_taskManager.start ( sendFailDataTask.duplicate() );
    
    //cout << "sendFailDataTask Finished" << endl;
    
}


void IOSHttpSDKHandle::appStartLaunchWithAppKey (
                                           const std::string &appkey
                                           , const std::string &channel
                                           , HttpSdkCallbackIntf *callback )
{
    m_callback = callback;
    if ( m_callback == NULL ) {
        m_callback = &m_NullSdkCallback;
    }
    
//    RptHdrObj hdr ( appkey, channel, m_callback );
//    m_hdrString = hdr.toString();
    
//    AutoPtr<HttpStrategyTask> pTT = new HttpStrategyTask ( this );
//    this->m_taskManager.start ( pTT.duplicate() );
}

void IOSHttpSDKHandle::sendData ( const std::string &data, const std::string &actionName )
{
    HttpDataNotification::Ptr pNf = new HttpDataNotification;
    pNf->setActionName ( actionName );
    pNf->setData ( data );
    pNf->setHdrData ( m_hdrString );
    pNf->setPriority ( m_AipaiRptStrategy.getActionPriority ( actionName ) );
    
    if ( m_AtomicCounter == 1 ) {
        if ( m_sendQueue.size() > 1500 ) {
            //cout << "m_sendQueue.size() > 1500" << endl;
        }
        else {
            m_sendQueue.enqueueNotification ( pNf, pNf->getPriority() );
        }
    }
    else {
        if ( m_cacheQueue.size() > 1500 ) {
            //cout << "m_cacheQueue.size() > 1500" << endl;
        }
        else {
            m_cacheQueue.enqueueNotification ( pNf );
        }
    }
}
void IOSHttpSDKHandle::startupEvent()
{
//    ::aipai::log::action::mbstartup startupProto;
//    startupProto.set_gameid ( m_callback->getGameId() );
//    startupProto.set_userid ( m_callback->getUserId() );
//    
//    string data = PBUtil::serializeProtobufMessage ( startupProto );
//    sendData ( data, "pcstartup" );
}

void IOSHttpSDKHandle::beginLogPageView ( const std::string &pageName, const std::string &enterdata )
{
    if ( pageName.length() == 0 ) {
        return;
    }
    
//    ::aipai::log::action::mbpageview *pEvent = new ::aipai::log::action::mbpageview;
//    pEvent->set_eventid ( pageName );
//    pEvent->set_enterdata ( enterdata );
//    Poco::Timestamp ts;
//    pEvent->set_entertime ( ts.epochMicroseconds() / 1000 );
//    pEvent->set_userid ( m_callback->getUserId() );
//    pEvent->set_gameid ( m_callback->getGameId() );
//    
//    {
//        Poco::FastMutex::ScopedLock lock ( this->m_mutex );
//        this->m_eventPageViewMap[pageName] = pEvent;
//    }
}

void IOSHttpSDKHandle::endLogPageView ( const std::string &pageName, const std::string &leavedata )
{
//    ::aipai::log::action::mbpageview *pEvent = NULL;
//    {
//        Poco::FastMutex::ScopedLock lock ( this->m_mutex );
//        pEvent = this->m_eventPageViewMap[pageName];
//    }
//    
//    if ( pEvent == NULL ) {
//        return;
//    }
//    
//    Poco::Timestamp ts;
//    pEvent->set_leavedata ( leavedata );
//    pEvent->set_leavetime ( ts.epochMicroseconds() / 1000 );
//    
//    
//    string data = PBUtil::serializeProtobufMessage ( *pEvent );
//    sendData ( data, "pcpageview" );
//    
//    {
//        Poco::FastMutex::ScopedLock lock ( this->m_mutex );
//        delete pEvent;
//        this->m_eventPageViewMap.erase ( pageName );
//    }
}

void IOSHttpSDKHandle::clickEvent ( const std::string &eventId )
{
//    ::aipai::log::action::mbclick objclick;
//    
//    ::aipai::log::action::clickitem *item = objclick.add_clickitems();;
//    item->set_key ( eventId );
//    item->set_value ( 1 );
//    
//    objclick.set_userid ( this->m_callback->getUserId() );
//    objclick.set_gameid ( this->m_callback->getGameId() );
//    
//    
//    string data = PBUtil::serializeProtobufMessage ( objclick );
//    
//    sendData ( data, "pcclick" );
}

void IOSHttpSDKHandle::beginEvent ( const std::string &pageName, const std::string &enterdata )
{
//    if ( pageName.length() == 0 ) {
//        return;
//    }
//    
//    ::aipai::log::action::mbcustevent *pEvent = new ::aipai::log::action::mbcustevent;
//    pEvent->set_eventid ( pageName );
//    pEvent->set_enterdata ( enterdata );
//    Poco::Timestamp ts;
//    pEvent->set_entertime ( ts.epochMicroseconds() / 1000 );
//    pEvent->set_userid ( m_callback->getUserId() );
//    pEvent->set_gameid ( m_callback->getGameId() );
//    
//    {
//        Poco::FastMutex::ScopedLock lock ( this->m_mutex );
//        this->m_eventCustEventMap[pageName] = pEvent;
//    }
}

void IOSHttpSDKHandle::endEvent ( const std::string &pageName, const std::string &leavedata )
{
//    ::aipai::log::action::mbcustevent *pEvent = NULL;
//    {
//        Poco::FastMutex::ScopedLock lock ( this->m_mutex );
//        pEvent = this->m_eventCustEventMap[pageName];
//    }
//    
//    if ( pEvent == NULL ) {
//        return;
//    }
//    
//    Poco::Timestamp ts;
//    pEvent->set_leavedata ( leavedata );
//    pEvent->set_leavetime ( ts.epochMicroseconds() / 1000 );
//    
//    
//    string data = PBUtil::serializeProtobufMessage ( *pEvent );
//    sendData ( data, "pccustevent" );
//    
//    {
//        Poco::FastMutex::ScopedLock lock ( this->m_mutex );
//        delete pEvent;
//        this->m_eventCustEventMap.erase ( pageName );
//    }
}

void IOSHttpSDKHandle::pubVideoCost ( const std::string &avgNetSpeed, const std::string &avgTimeCost )
{
//    ::aipai::log::action::pubvideocost obj;
//    obj.set_pubspeed ( avgNetSpeed );
//    obj.set_pubtime ( avgTimeCost );
//    
//    string data = PBUtil::serializeProtobufMessage ( obj );
//    sendData ( data, "pubvideocost" );
}

void IOSHttpSDKHandle::quit()
{
    m_quitCalled = true;
    this->m_taskManager.cancelAll();
    this->m_failedQueue.wakeUpAll();
    this->m_sendQueue.wakeUpAll();
    this->m_taskManager.joinAll();
}
