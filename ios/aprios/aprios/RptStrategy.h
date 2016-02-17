//
//  RptStrategy.h
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/20.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#ifndef __iosstatsdk__RptStrategy__
#define __iosstatsdk__RptStrategy__

#include "GlobalConfig.h"
#if DATA_SERDER_CUR == DATA_SERDER_KV
#include "StrategyProtocol_KV.h"
#else
#include "StrategyProtocol_PB.h"
#endif

#include "Poco/SharedPtr.h"

class	RptStrategy {
    
public:
    
    RptStrategy(){
        m_curIpIndex = 0;
#if DATA_SERDER_CUR == DATA_SERDER_PB
        m_protocol = new PBStrategyProtocol();
#else
        m_protocol = new KVStrategyProtocol;
#endif
    }
    
    bool loadHttpStrategy ( const std::string &httpResponse );
    bool loadLocalStrategy();
    bool writeStrategyToLocal();
    
    std::string getRandomIPStr(){
        m_curIpIndex++;
        if(m_curIpIndex > 100000 )
            m_curIpIndex = 0;
        
        size_t n = m_protocol->iplist.size();
        if ( n == 0 ) {
            return "mlog.aipai.com";
        }
        else
            n = (m_curIpIndex % n );
        
        return m_protocol->iplist[n];
    }
    
    int getActionPriority ( const std::string &action ) {
        std::map<std::string,int>::iterator it = m_protocol->actionPriority.find(action);
        if( it != m_protocol->actionPriority.end())
            return it->second;
        
        return m_protocol->defaultPriority;
    }
    
    int getSendThreadNum() const {
        if (m_protocol->sendThreadNum >=1 && m_protocol->sendThreadNum <=3) {
            return m_protocol->sendThreadNum;
        }
        return 1;
    }
    
    int getHttpRetryNum() const {
        if (m_protocol->domainRetryCount >=1 && m_protocol->domainRetryCount<=3) {
            return m_protocol->domainRetryCount;
        }
        return 2;
    }
private:
//    bool getLocalFileLocation ( std::string & );
    int m_curIpIndex;//使用一个循环的下标来达到随机的效果
    Poco::SharedPtr<StrategyProtocol> m_protocol;
};

#endif /* defined(__iosstatsdk__RptStrategy__) */
