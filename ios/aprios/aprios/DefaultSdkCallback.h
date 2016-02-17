//
//  DefaultSdkCallback.h
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/20.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#ifndef iosstatsdk_DefaultSdkCallback_h
#define iosstatsdk_DefaultSdkCallback_h

#include "HttpSdkCallbackIntf.h"
#include "Poco/Environment.h"
#include <string>

class DefaultSdkCallback : public HttpSdkCallbackIntf {
public:
    virtual ~DefaultSdkCallback() {}
    
    void setLoaderVer(const std::string& d){
        this->m_loaderVer = d;
    }
    virtual std::string getLoaderVer() {
        return m_loaderVer;
    }
    
    void setAddonVer(const std::string& d){
        this->m_addonVer = d;
    }
    virtual std::string getAddonVer() {
        return m_addonVer;
    }
    
    void setAppVer(const std::string& d){
        m_appVer = d;
    }
    virtual std::string getAppVer() {
        return m_appVer;
    }
    
    void setGameId(const std::string& d){
        m_gameId =d;
    }
    virtual std::string getGameId() {
        return m_gameId;
    }
    
    void setUserId(const std::string& d){
        m_userId=d;
    }
    virtual std::string getUserId() {
        return m_userId;
    }
    
    void setMachineId(const std::string& d){
        m_machineId = d;
    }
    virtual std::string getMachineId() {
        return m_machineId;
    }
    
private:
    std::string m_addonVer;
    std::string m_loaderVer;
    std::string m_gameId;
    std::string m_userId;
    std::string m_machineId;
    std::string m_appVer;
};

#endif
