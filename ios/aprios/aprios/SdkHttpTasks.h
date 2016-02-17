
//
#ifndef SdkHttpTasks_H
#define SdkHttpTasks_H

#include "Poco/SharedPtr.h"
#include "Poco/Delegate.h"
#include "Poco/BasicEvent.h"
#include "Poco/PriorityNotificationQueue.h"
#include "Poco/NotificationQueue.h"
#include "Poco/Task.h"
#include "RptStrategy.h"
#include "HttpDataNotification.h"

class HttpTaskBase : public Poco::Task
{
public:
    HttpTaskBase(RptStrategy* r,const std::string& name):Poco::Task ( name )
    {
        m_strategy = r;
        m_nTrys = r->getHttpRetryNum();
    }
    
    virtual ~HttpTaskBase(){}
    void runTask();
    /**
     * 从队列里面获取一条数据发送http请求
     */
    virtual HttpDataNotification::Ptr getOneData() = 0;
    /**
     * 把发送失败的数据返回到队列里面去，继续发送
     */
    virtual void pushBackData(HttpDataNotification::Ptr data) = 0;
    
    /**
     * 把没有发送的数据保存到本地磁盘
     */
    virtual void flushDataToLocalDisk() = 0;
    /**
     * 把上次没有发送的数据从本次磁盘加载进来
     */
    virtual void loadDataFromLocalDisk() = 0;
protected:
    int m_nTrys;
    RptStrategy* m_strategy;
};

class HttpStrategyTask : public Poco::Task
{
public:
    HttpStrategyTask():Poco::Task ( "HttpStrategyTask" ){}
//    Poco::BasicEvent<HttpEventArgs> HttpRequestFinished;
    void runTask();
    
};

class HttpSendQueuDataTask : public HttpTaskBase{
public:
    HttpSendQueuDataTask ( Poco::PriorityNotificationQueue & sendQueue,
                          Poco::NotificationQueue & failQueue,
                          RptStrategy* r)
    : HttpTaskBase(r, "HttpSendQueuDataTask" )
    ,  m_sendQueue(sendQueue),m_failQueue(failQueue)
    {
    }
    
    virtual HttpDataNotification::Ptr getOneData();
    void pushBackData(HttpDataNotification::Ptr data);
    void flushDataToLocalDisk();
    void loadDataFromLocalDisk();
    
private:
    Poco::PriorityNotificationQueue &m_sendQueue;
    Poco::NotificationQueue &m_failQueue;
    
};

class HttpFailedQueuDataTask : public HttpTaskBase {
public:
    HttpFailedQueuDataTask (Poco::NotificationQueue & failQueue,
                            RptStrategy* r )
    : HttpTaskBase( r, "HttpFailedQueuDataTask" ), m_failQueue( failQueue )
    {
    }
    virtual HttpDataNotification::Ptr getOneData();
    
    void pushBackData(HttpDataNotification::Ptr data);
    void flushDataToLocalDisk();
    void loadDataFromLocalDisk();
    
private:
    Poco::NotificationQueue &m_failQueue;
};
#endif
