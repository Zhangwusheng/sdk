//
//  IOSHttpSDK.m
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/21.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AdSupport/ASIdentifierManager.h>
#import <UIKit/UIDevice.h>
#include <libkern/OSAtomic.h>
#include <execinfo.h>

#import "IOSHttpSDK.h"
#import "../Base64/Base64.h"
#import "../FCUUID/FCUUID.h"

#include "Poco/Mutex.h"
#include "Poco/AtomicCounter.h"
#include "Poco/TaskManager.h"
#include "Poco/Environment.h"
#include "Poco/NotificationQueue.h"
#include "Poco/PriorityNotificationQueue.h"

#include "SdkHttpTasks.h"
#include "RptHdrObj.h"
#include "Protocols.h"
#include "SerDerUtil.h"
#include "DefaultSdkCallback.h"
#include "HttpDataNotification.h"
#include "RptUtil.h"

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

static void InstallUncaughtExceptionHandler(void);
static void UninstallUncaughtExceptionHandler(void);
static void HandleException(NSException *exception);
static void SignalHandler(int signal);

NSString * const UncaughtExceptionHandlerSignalExceptionName = @"UncaughtExceptionHandlerSignalExceptionName";
NSString * const UncaughtExceptionHandlerSignalKey = @"UncaughtExceptionHandlerSignalKey";
NSString * const UncaughtExceptionHandlerAddressesKey = @"UncaughtExceptionHandlerAddressesKey";

volatile int32_t UncaughtExceptionCount = 0;
const int32_t UncaughtExceptionMaximum = 10;

const NSInteger UncaughtExceptionHandlerSkipAddressCount = 4;
const NSInteger UncaughtExceptionHandlerReportAddressCount = 5;

static NSUncaughtExceptionHandler* preNSUncaughtExceptionHandler = nil;


#pragma mark - SDK实现 -

@implementation IOSHttpSDK
DefaultSdkCallback m_sdkCallback;
Poco::PriorityNotificationQueue m_sendQueue;
Poco::NotificationQueue m_failedQueue;
Poco::NotificationQueue m_cacheQueue;
RptStrategy m_rptStrategy;
Poco::TaskManager* m_taskManager;
Poco::AtomicCounter m_strategyLoaded;
map<string,mbpageview> m_pageview;
HttpStrategyTask* m_StrategyHttpResuest;
Poco::FastMutex m_mutex;
RptHdrObj m_hdr ;
std::string m_playId;
NSMutableDictionary* m_CallbackDict;



#pragma mark - 单例对象 -
+ (instancetype)shareStatSDK{
    static IOSHttpSDK *share = nil;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        share = [[IOSHttpSDK alloc] init];
    });
    
    return share;
}

#pragma mark - 工具 -
-(std::string) newPlayId{
    return [[ FCUUID uuid] UTF8String];
}

-(std::string)stringFromNSString:(NSString*)nsstr{
    if( nsstr == nil)
        return "";
    return string( [nsstr UTF8String] );
}

-(NSString*)nsstringFromStdString:(const std::string&)str{
    return [NSString stringWithUTF8String:str.c_str()];
}

-(mberrorreport)nsException2mberrorreport:(NSException*) exception{
    mberrorreport s;
    s.name = [[ exception name ] UTF8String];
    s.reason = [[exception reason] UTF8String];
    
    NSArray<NSString*>* symbols =  [exception callStackSymbols];
    for (int i=0; i<[symbols count]; i++) {
        NSString* str = [symbols objectAtIndex:i ];
        //NSLog(@"%@",str);
        s.items.push_back( [ str UTF8String]);
    }
    s.itemcount = (int)s.items.size();
    
    return s;
}

-(string) dict2KVString:(NSDictionary*)dict{
    
    if (dict == nil) {
        return "";
    }
    string result;
    NSEnumerator* enumer = [ dict keyEnumerator];
    id key= [enumer nextObject];
    while (key != nil) {
        NSString* strKey = (NSString*)key;
        NSString* value = (NSString*)[ dict objectForKey:key];
        if (value == nil) {
            continue;
        }
        result.append([strKey UTF8String]);
        result.append("=");
        result.append(SerDerUtil::toBase64(  [value UTF8String] ));
        result.append("&");
    }
    
    if (result.length()>0) {
        result.erase(result.length()-1);
    }
    
    //string encodedStr;
    //SerDerUtil::encode(result, "", encodedStr);
    return result;
}

-(void)sendData:(const string&)data forAction:(const string&)actionName{
    
    int end = 1;
    if( m_hdr.hasNextKey() )
        end = 2;
    
    int start = 1;
    if (actionName == "mbstartup" )
    {
        start = 2;
        end = 2;
    }
    
    for( int n = start; n<= end;n++){
        std::string hdrString = m_hdr.toString(n);
        
        HttpDataNotification::Ptr pNf = new HttpDataNotification;
        pNf->setLogId([[ FCUUID uuid] UTF8String]);
        pNf->setActionName ( actionName );
        pNf->setData ( data );
        pNf->setHdrData ( hdrString );
        pNf->setPriority ( m_rptStrategy.getActionPriority ( actionName ) );
        
        if ( m_strategyLoaded == 1 ) {
            if ( m_sendQueue.size() > 1500 ) {
            }
            else {
                m_sendQueue.enqueueNotification ( pNf, pNf->getPriority() );
            }
        }
        else {
            if ( m_cacheQueue.size() > 1500 ) {
            }
            else {
                m_cacheQueue.enqueueNotification ( pNf );
            }
        }
    }
}

#pragma mark - 启动和退出 -
- (void)fillSdkCallbackUsingDictionary:(NSDictionary*)globalInfo{
    NSString* userid = [ globalInfo objectForKey:@"userid"];
    NSString* gameid = [ globalInfo objectForKey:@"gameid"];
    NSString* appver = [ globalInfo objectForKey:@"appver"];
    NSString* loaderver=[globalInfo objectForKey:@"loaderver"];
    NSString* addonver=[globalInfo objectForKey:@"addonver"];
    NSString* machineid = [globalInfo objectForKey:@"machineid"];
    
    if (machineid == nil) {
        machineid = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
    }
    
    string strUserId =[self stringFromNSString:userid];
    string strAddon = [ self stringFromNSString:addonver];
    string strGameId = [ self stringFromNSString:gameid];
    string strAppVer = [ self stringFromNSString:appver];
    string strLoader = [ self stringFromNSString:loaderver];
    string strMachine = [self stringFromNSString:machineid];
    
    m_sdkCallback.setUserId(strUserId);
    m_sdkCallback.setAddonVer( strAddon);
    m_sdkCallback.setAppVer(strAppVer);
    m_sdkCallback.setGameId(strGameId);
    m_sdkCallback.setLoaderVer(strLoader);
    m_sdkCallback.setMachineId(strMachine);
}

-(void) onHttpStrategyLoaded:(NSString *)repsonse withSuccessFlag:(BOOL)successLoaded{
    string strResp = [ repsonse UTF8String ];
    
    bool success = successLoaded;
    //获取HTTP请求成功
    if (  success )
    {
        success = m_rptStrategy.loadHttpStrategy ( strResp ) ;
        //解析HTTP请求成功
        if(success)
        {
            NSLog(@"load strategy from http");
        }
        else
        {
            //解析HTTP失败，从本地加载
            success =  m_rptStrategy.loadLocalStrategy();
            if(success)
            {
                NSLog(@"http request success,parse failed:%@",repsonse);
                NSLog(@"load strategy from local");
            }
            else
            {
                NSLog(@"http request success,using defaule memory configuration");
            }
        }
    }
    else{
        //获取HTTP请求失败，从本地加载
        success =  m_rptStrategy.loadLocalStrategy();
        
        if(success){
            NSLog(@"http request failed:%@",repsonse);
            NSLog(@"http request failed,load strategy from local");
        }
        else
        {
            NSLog(@"using defaule memory configuration");
        }
    }
    
    m_strategyLoaded = 1;
    
    Poco::Notification::Ptr pNf = m_cacheQueue.dequeueNotification();
    while ( pNf.get() ) {
        HttpDataNotification::Ptr pNf2 = pNf.cast<HttpDataNotification>();
        if ( pNf2 ) {
            m_sendQueue.enqueueNotification ( pNf, pNf2->getPriority() );
        }
        
        pNf  = m_cacheQueue.dequeueNotification();
    }
}

-(void) appStartLaunchWithAppKey:(NSString*)appkey
                     withChannel:(NSString*)channel
                        withData:(NSDictionary*)data{
    m_strategyLoaded = 0;
    m_CallbackDict = [NSMutableDictionary dictionary];
    [m_CallbackDict addEntriesFromDictionary:data];
    
    if(!RptUtil::ensureBaseDirExists()){
        NSLog(@"Failed to create sdk base dir");
    }
    
    m_taskManager = new Poco::TaskManager();
    
    string strappkey = [appkey UTF8String];
    string strchannel = [ channel UTF8String];
    
    [self fillSdkCallbackUsingDictionary:data];
    
    //RptHdrObj hdr ;
    m_hdr.m_appkey = strappkey;
    m_hdr.m_secondAppKey = strappkey;
    
    m_hdr.m_channel = strchannel;
    m_hdr.m_osVendor = "Apple";
    m_hdr.m_osType = "ios";
    m_hdr.m_osVersion =[[[UIDevice currentDevice] systemVersion] UTF8String];
    m_hdr.m_osName =[[[UIDevice currentDevice] systemName] UTF8String];
    m_hdr.m_machindId = [[[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString] UTF8String];
    m_hdr.m_SdkCallback = &m_sdkCallback;
    m_hdr.m_statGuid = RptUtil::getGuidString();
    
    //m_hdrString = m_hdr.toString();
    
    //启动加载策略线程，策略加载成功之前，数据保存在缓存队列
    m_StrategyHttpResuest = new HttpStrategyTask();
    m_taskManager->start(m_StrategyHttpResuest);
    
    for ( int i = 0; i < 2; i++ ) {
        
        Poco::AutoPtr<HttpSendQueuDataTask> task = new HttpSendQueuDataTask ( m_sendQueue,m_failedQueue,&m_rptStrategy);
        
        if( i == 0 ){
            //加载上次程序退出时未发送的正常队列的数据
            task->loadDataFromLocalDisk();
        }
        m_taskManager->start ( task.duplicate() );
    }
    
    Poco::AutoPtr<HttpFailedQueuDataTask> sendFailDataTask = new HttpFailedQueuDataTask (m_failedQueue,&m_rptStrategy);
    
    //加载上次程序退出时未发送的失败队列的数据
    sendFailDataTask->loadDataFromLocalDisk();
    m_taskManager->start ( sendFailDataTask.duplicate() );
    
//    InstallUncaughtExceptionHandler();
}

-(void) quit{
    m_taskManager->cancelAll();
    m_taskManager->joinAll();
    //NSLog(@"quiting stat sdk");
}
#pragma mark - 事件处理 -

-(void) startupEvent{
    mbstartup ms;
    ms.gameid = m_sdkCallback.getGameId();
    ms.userid = m_sdkCallback.getUserId();
    
    string data = ms.toString();
    [self sendData:data forAction:"mbstartup"];
}


-(void)beginPageView:(NSString *)pageName withData:(NSString *)begindata{
    Poco::FastMutex::ScopedLock lock(m_mutex);
    if (pageName == nil) {
        return;
    }
    if ([pageName length] == 0) {
        return;
    }
    
    mbpageview ms;
    ms.gameid = m_sdkCallback.getGameId();
    ms.userid = m_sdkCallback.getUserId();
    ms.eventid = [pageName UTF8String];
    
    Poco::Timestamp ts;
    long entertime = ts.epochMicroseconds() / 1000 ;
    ms.entertime = entertime;
    {
        if(begindata!= nil)
            ms.enterdata = string([begindata UTF8String]);
        m_pageview[ms.eventid]= ms;
    }
}

-(void)endPageView:(NSString *)pageName withData:(NSString *)leaveData{
    Poco::FastMutex::ScopedLock lock(m_mutex);
    if (pageName == nil) {
        return;
    }
    if ([pageName length] == 0) {
        return;
    }
    
    string eventid = [pageName UTF8String];
    {
        if (m_pageview.count(eventid) == 0) {
            return;
        }
    }
    
    Poco::Timestamp ts;
    long leavetime = ts.epochMicroseconds() / 1000 ;
    
    string data ;
    {
        mbpageview& mb = m_pageview[eventid];
        if (leaveData != nil) {
            mb.leavedata = string([ leaveData UTF8String]);
        }
        
        mb.leavetime = leavetime;
        data = mb.toString();
    }
    
    [self sendData:data forAction:"mbpageview"];
}


-(void)clickEvent:(NSString *)evendId{
    mbclick obj;
    obj.userid = m_sdkCallback.getUserId();
    obj.gameid = m_sdkCallback.getGameId();
    obj.clickid = [evendId UTF8String];
    
    string data = obj.toString();
    [self sendData:data forAction:"mbclick"];
}

-(void)beginEvent:(NSString *)pageName withData:(NSString *)begindata{
    Poco::FastMutex::ScopedLock lock(m_mutex);
    if (pageName == nil) {
        return;
    }
    if ([pageName length] == 0) {
        return;
    }
    
    mbpageview ms;
    ms.gameid = m_sdkCallback.getGameId();
    ms.userid = m_sdkCallback.getUserId();
    ms.eventid = [pageName UTF8String];
    
    Poco::Timestamp ts;
    long entertime = ts.epochMicroseconds() / 1000 ;
    ms.entertime = entertime;
    if( begindata != nil )
        ms.enterdata = [begindata UTF8String];
    
    m_pageview[ms.eventid]= ms;
    
}

-(void)endEvent:(NSString *)pageName withData:(NSString *)leaveData{
    Poco::FastMutex::ScopedLock lock(m_mutex);
    if (pageName == nil) {
        return;
    }
    if ([pageName length] == 0) {
        return;
    }
    
    string eventid = [pageName UTF8String];
    if (m_pageview.count(eventid) == 0) {
        return;
    }
    
    Poco::Timestamp ts;
    long leavetime = ts.epochMicroseconds() / 1000 ;
    
    string data;
    {
        mbpageview& mb = m_pageview[eventid];
        if( leaveData != nil )
            mb.leavedata = [ leaveData UTF8String];
        mb.leavetime = leavetime;
        data = mb.toString();
    }
    
    [self sendData:data forAction:"mbcustevent"];
}

-(void) pubVideoNetSpeed:(NSString*)speed andPubTimeCost:(NSString*)cost{
    pubvideocost pc;
    pc.pubspeed = [ speed UTF8String];
    pc.pubtime = [ cost UTF8String];
    string data = pc.toString();
    [self sendData:data forAction:"mbpubvideocost"];
}


-(void) sendKVData:(NSString*)data forAction:(NSString*)action{
    std::string sdata = [ data UTF8String];
    string saction = [ action UTF8String];
    [ self sendData:sdata forAction:saction];
}

-(void) sendDictData:(NSDictionary *)data forAction:(NSString *)action{
    std::string dictString = [self dict2KVString:data];
    string saction = [ action UTF8String];
    [ self sendData:dictString forAction:saction];
}

-(void) loginEvent:(NSString *)bid forLogName:(NSString *)logName withType:(NSString *)type andDirection:(NSString *)direction loginStatus:(NSString*)status{
    
    mblogin ml;
    ml.bid = [bid UTF8String];
    ml.loginid=[logName UTF8String ];
    ml.type = [type UTF8String];
    ml.direction = [direction UTF8String];
    ml.login_status = [status UTF8String];
    string data = ml.toString();
    [self sendData:data forAction:"mblogin"];
}

-(void) switchAppKey:(NSString*)appkey{
    string strKey = [self stringFromNSString:appkey];
    m_hdr.m_secondAppKey = strKey;
}

-(void)reportVideoPlay:(NSString*)videoId
             isNewPlay:(BOOL)isNew
          withHeadTime:(int)headTime
          withDuration:(long)duration
       fromVideoSource:(int)source
        andSourceCheck:(int)check
         forPlayerType:(int)type
            isVideoEnd:(int)endFlag{
    
    if (isNew) {
        m_playId = [self newPlayId];
    }
    
    mbvideoplayduration mbvpd;
    mbvpd.playid = m_playId;
    mbvpd.headtime =headTime;
    mbvpd.source = source;
    mbvpd.sourcecheck = check;
    mbvpd.type = 20;
    mbvpd.videoid = [ videoId UTF8String];
    mbvpd.duration = duration;
    mbvpd.endflag = endFlag;
    
    if( m_CallbackDict != nil ){
        NSString* userid = [ m_CallbackDict objectForKey:@"userid"];
        if (userid != nil) {
            mbvpd.uid = [ userid UTF8String  ];
        }
    }

    string data = mbvpd.toString();
    [self sendData:data forAction:"mbvideoplayduration"];
}

-(void) mergeData:(NSDictionary *)newData{
    [ m_CallbackDict addEntriesFromDictionary:newData ];
}


+ (NSArray *)backtrace
{
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char **strs = backtrace_symbols(callstack, frames);
    
    int i;
    NSMutableArray *backtrace = [NSMutableArray arrayWithCapacity:frames];
    for (
         i = UncaughtExceptionHandlerSkipAddressCount;
         i < UncaughtExceptionHandlerSkipAddressCount +
         UncaughtExceptionHandlerReportAddressCount;
         i++)
    {
        [backtrace addObject:[NSString stringWithUTF8String:strs[i]]];
    }
    free(strs);
    
    return backtrace;
}

-(void) logError:(NSException*)exception{
    
    mberrorreport mber = [ self nsException2mberrorreport:exception ];

    [self sendData:mber.toString() forAction:"mberror"];
    
}

- (void)handleException:(NSException *)exception
{
    mberrorreport mber = [ self nsException2mberrorreport:exception ];

    
    string actionName = "mbdumpexcept";
    
    HttpDataNotification::Ptr pNf = new HttpDataNotification;
    pNf->setLogId([[ FCUUID uuid] UTF8String]);
    pNf->setActionName ( actionName );
    pNf->setData ( mber.toString() );
    
    std::string hdrString;
    if( m_hdr.hasNextKey())
        hdrString = m_hdr.toString(2);
    else
        hdrString = m_hdr.toString(1);
    
    pNf->setHdrData ( hdrString );
    pNf->setPriority ( m_rptStrategy.getActionPriority ( actionName ) );
  
    
    string fileName = RptUtil::getExceptionLogFileName();
    {
        ofstream ofs(fileName.c_str());
        ofs << pNf->serialize()<<endl;
    }
    NSLog(@"----------------------------------");
    NSLog(@"************Exception******:\n%@",[NSString stringWithUTF8String:mber.toString().c_str()]);
    NSLog(@"----------------------------------");
}

-(void)handleSignal:(int)signal
{
    NSMutableDictionary *userInfo =[NSMutableDictionary
                                    dictionaryWithObject:[NSNumber numberWithInt:signal]
                                    forKey:UncaughtExceptionHandlerSignalKey];
    
    NSArray *callStack = [IOSHttpSDK backtrace];
    [userInfo
     setObject:callStack
     forKey:UncaughtExceptionHandlerAddressesKey];
    
    
    NSString* reason = [NSString stringWithFormat:
                        NSLocalizedString(@"Signal %d was raised.", nil),
                        signal];
    
    NSException* exception = [NSException exceptionWithName:   UncaughtExceptionHandlerSignalExceptionName
                reason:reason userInfo:userInfo];
    
    mberrorreport mber = [ self nsException2mberrorreport:exception ];
    
    string actionName = "mbdumpsignal";
    
    HttpDataNotification::Ptr pNf = new HttpDataNotification;
    pNf->setLogId([[ FCUUID uuid] UTF8String]);
    pNf->setActionName ( actionName );
    pNf->setData ( mber.toString() );
    
    std::string hdrString;
    if( m_hdr.hasNextKey())
        hdrString = m_hdr.toString(2);
    else
        hdrString = m_hdr.toString(1);
    
    pNf->setHdrData ( hdrString );
    pNf->setPriority ( m_rptStrategy.getActionPriority ( actionName ) );
    
    
    string fileName = RptUtil::getSignalLogFileName();
    {
        ofstream ofs(fileName.c_str());
        ofs << pNf->serialize()<<endl;
    }
    
    NSLog(@"----------------------------------");
    NSLog(@"************Exception******:\n%@",[NSString stringWithUTF8String:mber.toString().c_str()]);
    NSLog(@"----------------------------------");

}

-(void) enableAutoDumpUncaughtException{
    InstallUncaughtExceptionHandler();
}

-(void) disableAutoDumpUncaughtException{
    UninstallUncaughtExceptionHandler();
}

@end

void HandleException(NSException *exception )
{
    [[IOSHttpSDK shareStatSDK] handleException:exception  ];
    
    if( preNSUncaughtExceptionHandler == nil){
        [exception raise];
    }
    else{
        preNSUncaughtExceptionHandler( exception );
    }
//    int32_t exceptionCount = OSAtomicIncrement32(&UncaughtExceptionCount);
//    if (exceptionCount > UncaughtExceptionMaximum)
//    {
//        return;
//    }
    
//    NSArray *callStack = [IOSHttpSDK backtrace];
    
//    NSMutableDictionary *userInfo =
//    [NSMutableDictionary
//     dictionaryWithObject:[NSNumber numberWithInt:-1]
//     forKey:UncaughtExceptionHandlerSignalKey];
//    
//    [userInfo addEntriesFromDictionary:[exception userInfo]];
//    
//    //    userInfo =
//    //    [NSMutableDictionary dictionaryWithDictionary:[exception userInfo]];
//    [userInfo
//     setObject:callStack
//     forKey:UncaughtExceptionHandlerAddressesKey];
//    
//    NSException* mexception = [NSException exceptionWithName:[exception name] reason:[exception reason] userInfo:userInfo];
    
}

void SignalHandler(int signal)
{
//    int32_t exceptionCount = OSAtomicIncrement32(&UncaughtExceptionCount);
    [[IOSHttpSDK shareStatSDK] handleSignal:signal ];
}

void InstallUncaughtExceptionHandler(void)
{
//    NSLog(@"InstallUncaughtExceptionHandler Called......");
    
    preNSUncaughtExceptionHandler = NSGetUncaughtExceptionHandler();
    NSSetUncaughtExceptionHandler(&HandleException);
    signal(SIGABRT, SignalHandler);
    signal(SIGILL, SignalHandler);
    signal(SIGSEGV, SignalHandler);
    signal(SIGFPE, SignalHandler);
    signal(SIGBUS, SignalHandler);
    signal(SIGPIPE, SignalHandler);
}

void UninstallUncaughtExceptionHandler(void)
{
    NSSetUncaughtExceptionHandler(NULL);
    signal(SIGABRT, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
}

