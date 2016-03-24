//
//  mbstartup.cpp
//  aprios
//
//  Created by zhangwusheng on 15/8/22.
//  Copyright (c) 2015年 adw. All rights reserved.
//

#include "Protocols.h"
#include "SerDerUtil.h"
#if DATA_SERDER_CUR == DATA_SERDER_KV
#include "Poco/NumberFormatter.h"

using namespace std;

std::string mbstartup::toString(){
    string result;
    result.append("gameid=");
    result.append(SerDerUtil::toBase64( gameid ));
    result.append("&userid=");
    result.append(SerDerUtil::toBase64( userid ));
    
    return result;
}

std::string mbpageview::toString(){
    string result;
    result.append("gameid=");
    result.append(SerDerUtil::toBase64(gameid));
    result.append("&userid=");
    result.append(SerDerUtil::toBase64(userid));
    result.append("&eventid=");
    result.append(SerDerUtil::toBase64(eventid));
    result.append("&entertime=");
    result.append(SerDerUtil::toBase64(Poco::NumberFormatter::format(entertime)));
    result.append("&enterdata=");
    result.append(SerDerUtil::toBase64(enterdata));
    result.append("&leavetime=");
    result.append(SerDerUtil::toBase64(Poco::NumberFormatter::format(leavetime)));
    result.append("&leavedata=");
    result.append(SerDerUtil::toBase64(leavedata));
    
    
    return result;
}

std::string mbclick::toString(){
    string result;
    result.append("gameid=");
    result.append(SerDerUtil::toBase64(gameid));
    result.append("&userid=");
    result.append(SerDerUtil::toBase64(userid));
    result.append("&clickitems=");
    result.append( SerDerUtil::toBase64( clickid+":1") );
//    result.append(":1");
    
    return result;
}

std::string pubvideocost::toString(){
    string result;
    result.append("netspeed=");
    result.append(SerDerUtil::toBase64(pubspeed));
    result.append("&pubtime=");
    result.append(SerDerUtil::toBase64(pubtime));
    
    return result;
}

std::string mblogin::toString(){
    string result;
    result.append("bid=");
    result.append(SerDerUtil::toBase64(this->bid));
    result.append("&loginid=");
    result.append(SerDerUtil::toBase64(this->loginid));
    result.append("&type=");
    result.append(SerDerUtil::toBase64(this->type));
    result.append("&direction=");
    result.append(SerDerUtil::toBase64(this->direction));
    
    return result;
}

std::string mbvideoplayduration::toString(){

    string result;
    result.append("playid=");
    result.append(SerDerUtil::toBase64(this->playid));
    result.append("&duration=");
    result.append(SerDerUtil::toBase64(Poco::NumberFormatter::format(this->duration)));
    result.append("&headtime=");
    result.append(SerDerUtil::toBase64(Poco::NumberFormatter::format(this->headtime)));
    result.append("&endflag=");
    result.append(SerDerUtil::toBase64(Poco::NumberFormatter::format(this->endflag)));
    result.append("&src=");
    result.append(SerDerUtil::toBase64(Poco::NumberFormatter::format(this->source)));
    result.append("&srccheck=");
    result.append(SerDerUtil::toBase64(Poco::NumberFormatter::format(this->sourcecheck)));
    result.append("&playerlocation=");
    result.append(SerDerUtil::toBase64(Poco::NumberFormatter::format(this->type)));
    result.append("&videoid=");
    result.append(SerDerUtil::toBase64(this->videoid));
    result.append("&userid=");
    result.append(SerDerUtil::toBase64(this->uid));
    
    return result;
}
#else

//#include "mbstartup.pb.h"
//#include "mbstartup.pb.cc"
//#include "mbpageview.pb.h"
//#include "mbpageview.pb.cc"
//#include "mbclick.pb.h"
//#include "mbclick.pb.cc"
//#include "PBUtil.h"
//
//std::string mbstartup::toString(){
//    
//    ::aipai::log::action::mbstartup startupProto;
//    startupProto.set_gameid ( gameid );
//    startupProto.set_userid ( userid );
//    
//    string data = PBUtil::serializeProtobufMessage ( startupProto );
//    return data;
//}
//
//std::string mbstartup::toString(){
//    
//    ::aipai::log::action::mbstartup startupProto;
//    startupProto.set_gameid ( gameid );
//    startupProto.set_userid ( userid );
//    
//    string data = PBUtil::serializeProtobufMessage ( startupProto );
//    return data;
//}
//
//string mbclick::toString(){
//    ::aipai::log::action::mbclick objclick;
//    
//    ::aipai::log::action::clickitem *item = objclick.add_clickitems();;
//    item->set_key ( clickid );
//    item->set_value ( 1 );
//    
//    objclick.set_userid ( userid);
//    objclick.set_gameid ( gameid );
//    
//    
//    string data = PBUtil::serializeProtobufMessage ( objclick );
//    return data;
//    
//}
//
//string pubvideocost::toString(){
//#error implemente me
//}
#endif
