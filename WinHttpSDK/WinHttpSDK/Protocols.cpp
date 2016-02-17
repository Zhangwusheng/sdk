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

std::string mcboxlogin::toString(){
	string result;
	result.append("pzid=");
	result.append(SerDerUtil::toBase64(this->pzid));
	result.append("&serverid=");
	result.append(SerDerUtil::toBase64(this->serverid));

	return result;
}

std::string mcboxpeak::toString(){

	string result;
	result.append("apid=");
	result.append(SerDerUtil::toBase64(this->apid));
	result.append("&serverid=");
	result.append(SerDerUtil::toBase64(this->serverid));
	result.append("&interval=");
	result.append(SerDerUtil::toBase64(this->interval));
	result.append("&peaktime=");
	result.append(SerDerUtil::toBase64(this->peaktime));
	result.append("&peakvalue=");
	result.append(SerDerUtil::toBase64(this->peakvalue));

	return result;
}

std::string mcbsysinf::toString(){

	string result;
	result.append("Os=");
	result.append(SerDerUtil::toBase64(this->Os));
	result.append("&Cpu=");
	result.append(SerDerUtil::toBase64(this->Cpu));
	result.append("&Disk=");
	result.append(SerDerUtil::toBase64(this->Disk));
	result.append("&Mem=");
	result.append(SerDerUtil::toBase64(this->Mem));
	result.append("&Video=");
	result.append(SerDerUtil::toBase64(this->Video));
	result.append("&Monitor=");
	result.append(SerDerUtil::toBase64(this->Monitor));

	return result;
}

std::string mcbInst::toString(){
	string result;
	result.append("evtType=");
	result.append(SerDerUtil::toBase64(this->evtType));
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
