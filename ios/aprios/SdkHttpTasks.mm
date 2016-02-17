#include "SdkHttpTasks.h"
#include "HttpDataNotification.h"
#include "IOSHttpRequest.h"
#include "IOSHttpSDK.h"
#include "RptUtil.h"
#include <iostream>
#include <fstream>
using namespace Poco;
using namespace std;

#pragma mark - 失败发送逻辑 -
void HttpSendQueuDataTask::pushBackData(HttpDataNotification::Ptr data)
{
//    NSLog(@"in HttpSendQueuDataTask::pushBackData");
    if( data->getFailedCount() <= m_nTrys ){
//        NSLog(@"FailedCount=%d,<=m_nTrys(%d)",data->getFailedCount(),m_nTrys);
        m_sendQueue.enqueueNotification(data, data->getPriority());
    }
    else{
//        NSLog(@"FailedCount=%d,>m_nTrys(%d)",data->getFailedCount(),m_nTrys);
        m_failQueue.enqueueNotification(data);
    }
}

void HttpFailedQueuDataTask::pushBackData(HttpDataNotification::Ptr data)
{
//    NSLog(@"in HttpFailedQueuDataTask::pushBackData");
//    NSLog(@" ... failedCount = %d",data->getFailedCount() );
    if( data->getFailedCount() <= m_nTrys+3 ){
        //Poco::Thread::sleep((1+m_nTrys)*1000);
//        NSLog(@"Sleeping ... failedCount = %d",data->getFailedCount() );
        Poco::Thread::sleep(10000);
        m_failQueue.enqueueNotification(data);
    }
    else{
//        NSLog(@"Discard ... failedCount = %d",data->getFailedCount() );
        //Just Discard it !
    }
}

void HttpFailedQueuDataTask::flushDataToLocalDisk(){
//    NSLog(@"calling HttpFailedQueuDataTask::flushDataToLocalDisk");
    if( m_failQueue.empty())
        return;
    
    string fileName = RptUtil::getFailQueueFileName();
    ofstream ofs(fileName.c_str());
    if(!ofs)
        return;
    
//    NSLog(@"flushing fail queue to disk");
    while(!m_failQueue.empty()){
        Poco::Notification::Ptr ptr = m_failQueue.dequeueNotification();
        HttpDataNotification::Ptr data = ptr.cast<HttpDataNotification>();
        if(data){
            ofs<< data->serialize()<<endl;
        }
    }
}

void HttpFailedQueuDataTask::loadDataFromLocalDisk(){
//    NSLog(@"calling HttpFailedQueuDataTask::loadDataFromLocalDisk");
    string fileName = RptUtil::getFailQueueFileName();
    {
        ifstream ifs(fileName.c_str());
        if(!ifs)
            return;
        char line[2048];
        while (ifs.getline(line, sizeof(line))) {
            HttpDataNotification::Ptr data = new HttpDataNotification;
            if( data->deserialize(line))
            {
                data->setOwner(this);
                //这一步其实没有必要，进入失败队列的已经不按照优先级发送了
                data->setPriority(this->m_strategy->getActionPriority(data->getActionName()));
                this->m_failQueue.enqueueNotification(data);
            }
        }
    }
    {
        ofstream ofs(fileName.c_str());
        ofs<<endl;
    }
}

#pragma mark - 获取发送数据 [失败队列/正常队列] -
HttpDataNotification::Ptr HttpSendQueuDataTask::getOneData(){
    Poco::Notification::Ptr ntf = m_sendQueue.waitDequeueNotification ( 1000 );
    if( !ntf )
        return HttpDataNotification::Ptr();
    return ntf.cast<HttpDataNotification>();
}

HttpDataNotification::Ptr HttpFailedQueuDataTask::getOneData(){
    Poco::Notification::Ptr ntf = m_failQueue.waitDequeueNotification ( 1000 );
    if( !ntf )
        return HttpDataNotification::Ptr();
    return ntf.cast<HttpDataNotification>();
}

void HttpSendQueuDataTask::flushDataToLocalDisk(){
//    NSLog(@"calling HttpSendQueuDataTask::flushDataToLocalDisk");
    if( m_sendQueue.empty())
        return;
    
    string fileName = RptUtil::getSendQueueFileName();
    ofstream ofs(fileName.c_str());
    if(!ofs)
        return;
//    NSLog(@"flushing send queue to disk");
   
    while(!m_sendQueue.empty()){
        Poco::Notification::Ptr ptr = m_sendQueue.dequeueNotification();
        HttpDataNotification::Ptr data = ptr.cast<HttpDataNotification>();
        if(data){
            ofs<< data->serialize()<<endl;
        }
    }
    
}

void HttpSendQueuDataTask::loadDataFromLocalDisk(){
//    NSLog(@"calling HttpSendQueuDataTask::loadDataFromLocalDisk");
    string fileName = RptUtil::getSendQueueFileName();
    {
        ifstream ifs(fileName.c_str());
        if(!ifs)
            return;
        
        char line[2048];
        while (ifs.getline(line, sizeof(line))) {
            HttpDataNotification::Ptr data = new HttpDataNotification;
            if( data->deserialize(line))
            {
                data->setOwner(this);
                //这一步有必要
                data->setPriority(this->m_strategy->getActionPriority(data->getActionName()));
                this->m_sendQueue.enqueueNotification(data,data->getPriority());
            }
        }
    }
    {
        ofstream ofs(fileName.c_str());
        ofs<<endl;
    }
}
#pragma mark - HTTP发送数据 -
void HttpTaskBase::runTask()
{
    while ( this->state() == Poco::Task::TASK_RUNNING ) {
        
        HttpDataNotification::Ptr dataPtr = getOneData();
        if ( !dataPtr ) {
            continue;
        }
        
        HttpDataNotification* ptr = dataPtr.duplicate();
        ptr->setOwner(this);
        
        ostringstream ostm;
        ostm<<"action="<<dataPtr->getActionName()
        <<"&"<<dataPtr->getHdrData()
        <<"&logid="<<dataPtr->getLogid()
        <<"&clitime="<<dataPtr->getCliTime()
        <<"&"<<dataPtr->getData();
        
        NSString* url=[NSString stringWithUTF8String:dataPtr->getSendUrl(m_strategy).c_str()];
        
//        NSLog(@"Sending to Url:%@",url);
        
        NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
        
        request.HTTPMethod = @"POST";
        request.HTTPBody = [[
                             [NSString alloc] initWithUTF8String:ostm.str().c_str()] dataUsingEncoding:NSUTF8StringEncoding];
        
        [request setValue:@"AprSdkIOS" forHTTPHeaderField:@"User-Agent"];
        NSURLResponse * response = nil;
        NSError* error = nil;
        [ NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];
        
        if( error != nil ){
            ptr->incrementFailCount();
            ptr->getOwner()->pushBackData(ptr);
        }
    }
    
    flushDataToLocalDisk();
    NSLog(@"quiting thread...");
}

#pragma mark - 获取策略数据 -
void HttpStrategyTask::runTask()
{
    //只要同步就够了，因为这里已经是一个线程在加载了
    NSURLRequest* urlRequest = [NSURLRequest requestWithURL:[ NSURL URLWithString:@"http://trans.mlog.aipai.com/kvinfo.php"] cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:10.0 ];
    
    NSURLResponse * response = nil;
    NSError* error = nil;
    NSData* data = [ NSURLConnection sendSynchronousRequest:urlRequest returningResponse:&response error:&error];
    
    if( error != nil){
        [[IOSHttpSDK shareStatSDK ] onHttpStrategyLoaded:[error localizedDescription] withSuccessFlag:FALSE];
    }
    else{
        [[IOSHttpSDK shareStatSDK] onHttpStrategyLoaded:[[ NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] withSuccessFlag:TRUE];
    }
}
