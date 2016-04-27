//
//  HttpDataNotification.h
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/20.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#include "HttpDataNotification.h"
#include "Poco/StringTokenizer.h"
#include "Poco/NumberParser.h"

std::string HttpDataNotification::serialize() {
    std::string result;
//    result.append ( "a" );
    result.append ( m_action );
    result.append ( "|" );
    result.append ( m_hdrstr );
    result.append ( "|" );
    result.append ( m_logid );
    result.append ( "|" );
    result.append ( getCliTime() );
    result.append ( "|" );
    result.append ( m_data );
    return result;
}

bool HttpDataNotification::deserialize(const std::string& data){
    Poco::StringTokenizer token(data,"|");
    if( token.count() != 5 )
        return false;
    
    try {
        this->setActionName(token[0]);
        this->setHdrData(token[1]);
        this->setLogId(token[2]);
        this->m_clitime = Poco::NumberParser::parse64(token[3]);
        this->setData(token[4]);
        return true;
        
    } catch (...) {
        return false;
    }
}

