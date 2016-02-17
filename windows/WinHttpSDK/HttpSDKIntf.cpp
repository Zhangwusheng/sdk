#include "HttpSDKIntf.h"
#include "WinHttpSDK.h"
#include "Poco/SingletonHolder.h"
#include "Poco/MD5Engine.h"
#include "SeqIDGenerator.h"

using namespace Poco;
using namespace AipaiRptSDK;

namespace {
    static SingletonHolder<HttpSDKIntf> sHttpSDKIntf;
    static WinHttpSDK *sWinHttpSDK;
}

HttpSDKIntf::HttpSDKIntf()
{
    sWinHttpSDK = new WinHttpSDK;
}


HttpSDKIntf::~HttpSDKIntf()
{
    if ( sWinHttpSDK != NULL ) {
        delete sWinHttpSDK;
        sWinHttpSDK = NULL;
    }
}


HttpSDKIntf &HttpSDKIntf::getInstance()
{
    return *sHttpSDKIntf.get();
}

void HttpSDKIntf::appStartLaunchWithAppKey (
    const std::string &appkey
    , const std::string &channel
    , HttpSdkCallbackIntf *callback )
{
    try {
        return sWinHttpSDK->appStartLaunchWithAppKey ( appkey, channel, callback );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::startupEvent()
{
    try {
        return sWinHttpSDK->startupEvent();
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::quit()
{
    try {
		::Sleep(20);
        return sWinHttpSDK->quit();
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::beginLogPageView ( const std::string &pageName, const std::string &enterdata )
{
    try {
        return sWinHttpSDK->beginLogPageView ( pageName, enterdata );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::endLogPageView ( const std::string &pageName, const std::string &leavedata )
{
    try {
        return sWinHttpSDK->endLogPageView ( pageName, leavedata );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::clickEvent ( const std::string &eventId )
{
    try {
        return sWinHttpSDK->clickEvent ( eventId );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::beginEvent ( const std::string &pageName, const std::string &enterdata )
{
    try {
        return sWinHttpSDK->beginEvent ( pageName, enterdata );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::endEvent ( const std::string &pageName, const std::string &leavedata )
{
    try {
        return sWinHttpSDK->endEvent ( pageName, leavedata );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::pubVideoCost ( const std::string &avgNetSpeed, const std::string &avgTimeCost )
{
    try {
        return sWinHttpSDK->pubVideoCost ( avgNetSpeed, avgTimeCost );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::mcboxLogin ( const std::string &apid, const std::string &serverid )
{
    try {
        return sWinHttpSDK->mcboxLogin ( apid, serverid );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::mcboxPeak ( const std::string &apid
                              , const  std::string &serverid
                              , long peakValue, int interval
                              , const std::string &peakTime  )
{
    try {
        return sWinHttpSDK->mcboxPeak ( apid, serverid, peakValue, interval, peakTime );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::sendKVData ( const std::string &action, const std::string &data )
{
    try {
        return sWinHttpSDK->sendKVData ( action, data );
    }
    catch ( Poco::Exception & ) {
        //std::cout << e.displayText() << std::endl;
    }
    catch ( ... ) {
    }
}

void HttpSDKIntf::sendKVData(const std::string& action, const std::map<std::string, std::string>& dataItems)
{
	try {
		return sWinHttpSDK->sendKVData(action, dataItems);
	}
	catch (Poco::Exception &) {
		//std::cout << e.displayText() << std::endl;
	}
	catch (...) {
	}
}
std::string DefaultSdkCallback::getMachineId()
{
	string machineId = Poco::Environment::nodeId();
	if (machineId.find("00:00") != string::npos)
		return machineId;

	unsigned char mid[16];
	MD5Engine engine;
	CSeqIDGenerator::GetSequenceID(mid);
	engine.update(mid, 16);
	machineId = DigestEngine::digestToHex(engine.digest());
	return machineId;
}