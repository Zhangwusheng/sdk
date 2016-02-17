//
//  StrategyProtocol.h
//  aprios
//
//  Created by zhangwusheng on 15/8/22.
//  Copyright (c) 2015年 adw. All rights reserved.
//

#ifndef aprios_StrategyProtocol_h
#define aprios_StrategyProtocol_h

#include <vector>
#include <map>
#include <string>
#include <sstream>
using namespace std;

class StrategyProtocol
{
public:
    
    StrategyProtocol(){
        setDefaultValue();
    }
    
    std::string debugString(){
        std::ostringstream ostm;
        ostm<<"enabled="<<(this->enabled?"true":"false");
        ostm<<"&domainRetryCount="<<this->domainRetryCount
        <<"&iplist=";
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
        return str;
    }
    
    bool enabled;
    int domainRetryCount;
    std::vector<std::string> iplist;
    std::map<std::string,int> actionSample;
    bool cacheClickEvents;
    int clickEventItems;
    int clickEventInterval;
    std::map<std::string,int> actionPriority;
    int defaultPriority;
    int eventPartition;
    int sendThreadNum;
    
    void setDefaultValue(){
        this->enabled = true;
        this->domainRetryCount = 2;
        
//        this->iplist.push_back("14.29.84.204");
//        this->iplist.push_back("14.29.84.205");
//        this->iplist.push_back("14.29.84.206");
//        this->iplist.push_back("14.29.84.207");
//        this->iplist.push_back("14.29.84.208");
//        this->iplist.push_back("14.29.84.209");
        this->iplist.push_back("219.135.99.168");
        this->iplist.push_back("122.13.76.219");
        this->iplist.push_back("219.135.99.167");
        this->iplist.push_back("122.13.76.218");
        this->iplist.push_back("219.135.99.169");
        this->iplist.push_back("122.13.76.220");
        this->iplist.push_back("219.135.99.166");
        this->iplist.push_back("122.13.76.217");
        this->iplist.push_back("219.135.99.165");
        this->iplist.push_back("122.13.76.216");
        this->iplist.push_back("219.135.99.164");
        this->iplist.push_back("122.13.76.215");
        
        this->actionSample["mbclick"]=100;
        this->actionSample["mbpageview"]=100;
        this->cacheClickEvents = false;
        this->clickEventItems = 5;
        this->clickEventInterval = 10;
        this->actionPriority["mbstartup"]=1000;
        this->actionPriority["pcstartup"]=1000;
        this->actionPriority["mbpageview"]=1000;
        this->defaultPriority = 10;
        this->eventPartition = 0;
        this->sendThreadNum = 2;
    }
    virtual bool deserialize(const std::string& data) = 0;
    virtual std::string serialize() = 0;
};


#endif
