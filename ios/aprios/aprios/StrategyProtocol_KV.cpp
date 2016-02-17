//
//  RptStrategy.cpp
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/20.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#include "StrategyProtocol_KV.h"
#include "SerDerUtil.h"
#include "Poco/StringTokenizer.h"
#include "Poco/NumberParser.h"
#include <sstream>
#include <iostream>
using namespace std;

bool KVStrategyProtocol::deserialize(const std::string& data)
{
    try {
        string decoded;
        SerDerUtil::decode(data, decoded);
        
        Poco::StringTokenizer token(decoded,"&");
        map<string,string> ms;
        
        for(int i=0;i<token.count();i++){
            string::size_type idx = token[i].find_first_of("=");
            if(idx==string::npos)
                continue;
            
            string key = token[i].substr(0,idx);
            string value = token[i].substr(idx+1);
            
            ms[key]=value;
        }
        
        this->enabled = ( (ms["enabled"] == "true")||(ms["enabled"] == "1"));
        
        this->domainRetryCount = Poco::NumberParser::parse(ms["domainRetryCount"]);
        if( this->domainRetryCount < 2 )
            this->domainRetryCount = 2;
        
        string iplistDecoded =ms["iplist"];
        
        Poco::StringTokenizer ipToken(iplistDecoded,",");
        for(int i=0;i<ipToken.count();i++){
            this->iplist.push_back(ipToken[i]);
        }
        
        
        string samples = ms["actionSample"];
        Poco::StringTokenizer sampleToken(samples,",");
        for(int i=0;i<sampleToken.count();i++){
            string::size_type idx = sampleToken[i].find_first_of(":");
            if(idx==string::npos)
                continue;
            
            string key = sampleToken[i].substr(0,idx);
            string value = sampleToken[i].substr(idx+1);
            
            this->actionSample[key]=Poco::NumberParser::parse(value);
            
        }
        
        this->cacheClickEvents = ( (ms["cacheClickEvents"] == "true")||(ms["cacheClickEvents"] == "1"));
        
        this->clickEventItems =Poco::NumberParser::parse(ms["clickEventItems"]);
        this->clickEventInterval =Poco::NumberParser::parse(ms["clickEventInterval"]);
        
        string priority = ms["actionPriority"];
        //priority = SerDerUtil::fromBase64(priority);
        
        Poco::StringTokenizer priorityToken(priority,",");
        for(int i=0;i<priorityToken.count();i++){
            string::size_type idx = priorityToken[i].find_first_of(":");
            if(idx==string::npos)
                continue;
            
            string key = priorityToken[i].substr(0,idx);
            string value = priorityToken[i].substr(idx+1);
            
            this->actionPriority[key]=Poco::NumberParser::parse(value);
            
        }
        
        this->defaultPriority =Poco::NumberParser::parse(ms["defaultPriority"]);
        
        this->eventPartition =Poco::NumberParser::parse(ms["eventPartition"]);
        
        
        this->sendThreadNum =Poco::NumberParser::parse(ms["sendThreadNum"]);
        if( this->sendThreadNum <= 0 )
            this->sendThreadNum = 2;
        if( this->sendThreadNum > 3 )
            this->sendThreadNum = 3;
        
    } catch (Poco::Exception& e) {
        return false;
    }
    return true;
}

std::string KVStrategyProtocol::serialize(){
    
    ostringstream ostm;
    ostm<<"enabled="<<(this->enabled?"true":"false");
    
    ostm<<"&domainRetryCount="<<this->domainRetryCount<<"&iplist=";
    for(int i=0;i<iplist.size();i++){
        ostm<<iplist[i]<<",";
    }
    
    ostm.seekp((long)ostm.tellp()-1);
    ostm<<"&actionSample=";
    for (map<string,int>::iterator it=actionSample.begin(); it!=actionSample.end(); ++it) {
        ostm<<it->first<<":"<<it->second<<",";
    }
    
    ostm.seekp((long)ostm.tellp()-1);
    ostm<<"&cacheClickEvents="<<(this->enabled?"true":"false");
    ostm<<"&clickEventItems="<<this->clickEventItems;
    ostm<<"&clickEventInterval="<<this->clickEventInterval;
    for (map<string,int>::iterator it=actionPriority.begin(); it!=actionPriority.end(); ++it) {
        ostm<<it->first<<":"<<it->second<<",";
    }
    
    ostm.seekp((long)ostm.tellp()-1);
    ostm<<"&defaultPriority="<<this->defaultPriority;
    ostm<<"&eventPartition="<<this->eventPartition;
    ostm<<"&sendThreadNum="<<this->sendThreadNum;
    
    string str = ostm.str();
    string decoded;
    SerDerUtil::encode(str, "", decoded);
    return decoded;
}

