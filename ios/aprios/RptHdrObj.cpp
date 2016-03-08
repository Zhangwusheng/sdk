//
//  RptHdrObj.cpp
//  aprios
//
//  Created by zhangwusheng on 15/8/22.
//  Copyright (c) 2015年 adw. All rights reserved.
//

#include "RptHdrObj.h"
#include <fstream>
#if DATA_SERDER_CUR == DATA_SERDER_PB
#include "hdr.pb.h"
#else
#include <sstream>
#endif
#include "SerDerUtil.h"
#include <iostream>
using namespace std;

std::string RptHdrObj::getAppKey(int index)
{
    if( index == 1 )
        return m_appkey;
    else
        return m_secondAppKey;
}

bool RptHdrObj::hasNextKey(){
    if (m_secondAppKey.length()>0) {
        if (m_secondAppKey != m_appkey) {
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

std::string RptHdrObj::toString(int keyIndex)
{
#if DATA_SERDER_CUR == DATA_SERDER_KV
    
    ostringstream ostm;
    
    ostm<<"appkey="<<getAppKey(keyIndex)
        <<"&channel="<<SerDerUtil::toBase64( m_channel)
        <<"&machineid="<<SerDerUtil::toBase64(m_machindId)
        <<"&loaderver="<<SerDerUtil::toBase64(m_SdkCallback->getLoaderVer())
        <<"&addonver="<<SerDerUtil::toBase64(m_SdkCallback->getAddonVer())
        <<"&ostype="<<SerDerUtil::toBase64(m_osType)
        <<"&osver="<<SerDerUtil::toBase64(m_osVersion)
        <<"vendor="<<SerDerUtil::toBase64(m_osVendor)
        <<"&appver="<<SerDerUtil::toBase64(m_SdkCallback->getAppVer())
        <<"&networktype="<<SerDerUtil::toBase64("notyet")
        <<"&phone="<<SerDerUtil::toBase64("notyet")
        <<"&guid="<<m_statGuid
        <<"&v=1";
    
    return ostm.str();
    
#else
//    ::aipai::log::action::hdr pbHdrObj;
//    pbHdrObj.set_addonver ( m_SdkCallback->getAddonVer());
//    pbHdrObj.set_appkey ( m_appkey );
//    pbHdrObj.set_channel ( m_channel );
//    pbHdrObj.set_appver ( m_SdkCallback->getAppVer() );
//    pbHdrObj.set_loaderver ( m_SdkCallback->getLoaderVer() );
//    pbHdrObj.set_machineid ( m_machindId );
//    pbHdrObj.set_ostype ( m_osName );
//    pbHdrObj.set_osver ( m_osVersion );
//    pbHdrObj.set_vendor ( m_osVendor );
//    
//    
//    std::string base64str = SerDerUtil::serializeProtobufMessage ( pbHdrObj );
//    
//    return base64str;
#endif
    
}

