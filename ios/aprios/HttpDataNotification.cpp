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
    result.append ( "action=" );
    result.append ( m_action );
    result.append ( "|hdr=" );
    result.append ( m_hdrstr );
    result.append ( "|logid=" );
    result.append ( m_logid );
    result.append ( "|clitime=" );
    result.append ( getCliTime() );
    result.append ( "|data=" );
    result.append ( m_data );
    return result;
}

bool HttpDataNotification::deserialize(const std::string& data){
    Poco::StringTokenizer token(data,"|");
    if( token.count() != 4 )
        return false;
    
    try {
        this->setActionName(token[0]);
        this->setHdrData(token[1]);
        this->setLogId(token[2]);
        this->m_clitime = Poco::NumberParser::parse64(token[3]);
        
        return true;
        
    } catch (...) {
        return false;
    }
}

