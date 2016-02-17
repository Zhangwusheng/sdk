//
//  HttpDataNotification.h
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/20.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#ifndef iosstatsdk_HttpDataNotification_h
#define iosstatsdk_HttpDataNotification_h

#include "Poco/Notification.h"
#include "Poco/Timestamp.h"
#include "Poco/NumberFormatter.h"
#include "Poco/AutoPtr.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timezone.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "RptStrategy.h"

class HttpTaskBase;

class HttpDataNotification: public Poco::Notification
{
public:
    typedef Poco::AutoPtr<HttpDataNotification> Ptr;
    HttpDataNotification() {
        Poco::Timestamp ts;
        m_clitime = ts.epochMicroseconds() / 1000;
        
        m_nFailedCount = 0;
    }

    
    void setHdrData ( const std::string &hdrstr ) {
        m_hdrstr = hdrstr;
    }
    std::string getHdrData() const {
        return m_hdrstr;
    }
    
    void setActionName ( const std::string &data ) {
        m_action = data;
    }
    std::string getActionName() const {
        return m_action;
    }
    
    void setLogId(const std::string& id){
        m_logid = id;
    }
    std::string getLogid() {
        return m_logid;
    }
    
    std::string getCliTime() {
        return Poco::NumberFormatter::format ( m_clitime );
    }
    
    std::string getData() const {
        return m_data;
    }
    
    void setData ( const std::string &data ) {
        m_data = data;
    }
    
    void setPriority ( int n ) {
        m_priority = n;
    }
    
    int getPriority() {
        return m_priority;
    }
    
    void setOwner(HttpTaskBase* task){
        m_owner = task;
    }
    HttpTaskBase* getOwner(){
        return m_owner;
    }
    
    std::string serialize() ;
    bool deserialize(const std::string& data);
    
    int getFailedCount() {
        return m_nFailedCount;
    }
    
    void incrementFailCount() {
        m_nFailedCount++;
    }
    
    std::string getSendUrl(RptStrategy* strategy)
    {
        if( m_nFailedCount < 2 )
            return "http://mlog.aipai.com/i.gif";
        else{
            return "http://" + strategy->getRandomIPStr() + "/i.gif";
        }
    }
protected:
    virtual ~HttpDataNotification() {};
    /// Destroys the TaskNotification.
    
private:
    int m_priority;
    int m_nFailedCount;
    std::string m_action;
    std::string m_hdrstr;
    Poco::Timestamp::TimeDiff m_clitime;
    std::string m_logid;
    std::string m_data;
    HttpTaskBase* m_owner;
};

#endif
