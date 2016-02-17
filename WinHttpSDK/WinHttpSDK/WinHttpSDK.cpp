
#include "WinHttpSDK.h"
#include "SdkHttpTasks.h"
#include "Poco/Thread.h"
#include "Poco/Base64Decoder.h"
#include "Poco/Base64Encoder.h"
#include "Poco/MD5Engine.h"
#include "HttpDataNotification.h"
#include "Wincrypt.h"
#include "hdr.pb.h"
#include "RptHdrObj.h"
#include "RptUtil.h"
#include "SeqIDGenerator.h"
#include "Protocols.h"
#include "SerDerUtil.h"


#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace Poco;

void WinHttpSDK::onStrategyLoaded ( const void *pSender, HttpEventArgs &args )
{
    bool success = args.success;
    if (  success ) {
        success = m_AipaiRptStrategy.loadHttpStrategy ( args.response ) ;
    }

    if ( !success ) {
        success =  m_AipaiRptStrategy.loadLocalStrategy();
    }


    this->m_AtomicCounter = 1;

    int nTotal = 0;
    Notification::Ptr pNf = m_cacheQueue.dequeueNotification();
    while ( pNf.get() ) {
        HttpDataNotification::Ptr pNf2 = pNf.cast<HttpDataNotification>();
        if ( pNf2 ) {
            nTotal++;
            m_sendQueue.enqueueNotification ( pNf, pNf2->getPriority() );
        }

        pNf  = m_cacheQueue.dequeueNotification();
    }

}

/**
 *  如果应用程序什么都不干，调用了appStartLaunchWithAppKey
 *  立马调用quit,则这时候同一个m_taskManager的两个函数会lock同一个FastMutex，
 *  就会造成死锁。需要睡眠一秒，再调用quit就不会同时在不同线程调用同一个m_taskManager
 *  的两个函数，这样可以避免死锁。当然这种情况只有极端情况下才会发生。
 */
void WinHttpSDK::appStartLaunchWithAppKey (
    const std::string &appkey
    , const std::string &channel
    , HttpSdkCallbackIntf *callback )
{
	RptUtil::ensureBaseDirExists();

    m_callback = callback;
    if ( m_callback == NULL ) {
        m_callback = &m_NullSdkCallback;
    }

	RptHdrObj hdr;// (appkey, channel, m_callback);
	hdr.m_appkey = appkey;
	hdr.m_channel = channel;
	hdr.m_machindId = m_callback->getMachineId();
	hdr.m_osName = Poco::Environment::osName();
	hdr.m_osType = "Windows";
	hdr.m_osVendor = "Microsoft";
	hdr.m_osVersion = Poco::Environment::osVersion();
	hdr.m_SdkCallback = m_callback;
	

	Poco::UUIDGenerator &gen = Poco::UUIDGenerator::defaultGenerator();
	Poco::UUID uuid = gen.createOne();
	string tid = uuid.toString();
	tid = Poco::remove(tid, '-');

	hdr.m_statGuid = RptUtil::getGuidString();
	
    m_hdrString = hdr.toString();

    m_hdrKVString = hdr.toString();

    AutoPtr<HttpStrategyTask> pTT = new HttpStrategyTask ( this );
    this->m_taskManager.start ( pTT.duplicate() );

	//TaskManager::start和TaskManager::joinAll如果同时调用会死锁，所以把这个逻辑从onStrategyLoaded移到启动这里
	for (int i = 0; i < 2; i++) {
		AutoPtr<HttpSendQueuDataTask> task = new HttpSendQueuDataTask(this);
		this->m_taskManager.start(task.duplicate());
	}
	AutoPtr<HttpFailedQueuDataTask> sendFailDataTask = new HttpFailedQueuDataTask(this);
	this->m_taskManager.start(sendFailDataTask.duplicate());
}

void WinHttpSDK::sendData ( const std::string &data, const std::string &actionName )
{
    HttpDataNotification::Ptr pNf = new HttpDataNotification;
    //pNf->setDataFormat ( mode );
    pNf->setActionName ( actionName );
    pNf->setData ( data );
    pNf->setHdrData ( m_hdrString );
    //pNf->setHdrDataKV ( m_hdrKVString );
    pNf->setPriority ( m_AipaiRptStrategy.getActionPriority ( actionName ) );

    if ( m_AtomicCounter == 1 ) {
        if ( m_sendQueue.size() > 1500 ) {
        }
        else {
            m_sendQueue.enqueueNotification ( pNf, pNf->getPriority() );
        }
    }
    else {
        if ( m_cacheQueue.size() > 1500 ) {
        }
        else {
            m_cacheQueue.enqueueNotification ( pNf );
        }
    }
}
void WinHttpSDK::startupEvent()
{
	mbstartup ms;
	ms.gameid = Poco::NumberFormatter::format(m_callback->getGameId());
	ms.userid = m_callback->getUserId();
	string data = ms.toString();

    sendData ( data, "pcstartup" );
}

void WinHttpSDK::beginLogPageView ( const std::string &pageName, const std::string &enterdata )
{
    if ( pageName.length() == 0 ) {
        return;
    }

	mbpageview mp;
	mp.eventid = pageName;
	mp.enterdata = enterdata;
	Poco::Timestamp ts;
	mp.entertime =static_cast<long>( ts.epochMicroseconds() / 1000 );
	mp.userid = m_callback->getUserId();
	mp.gameid = m_callback->getGameId();

    {
        Poco::FastMutex::ScopedLock lock ( this->m_mutex );
		this->m_pageview[pageName] = mp;
    }
}

void WinHttpSDK::endLogPageView ( const std::string &pageName, const std::string &leavedata )
{
	if (this->m_pageview.count(pageName) == 0)
		return;
	mbpageview& mp = this->m_pageview[pageName];
	mp.leavedata = leavedata;
	Poco::Timestamp ts;
	mp.leavetime = static_cast<long>(ts.epochMicroseconds() / 1000);
	string data = mp.toString();
    sendData ( data, "pcpageview" );

    {
        Poco::FastMutex::ScopedLock lock ( this->m_mutex );
       // delete pEvent;
		this->m_pageview.erase(pageName);
    }
}

void WinHttpSDK::clickEvent ( const std::string &eventId )
{
	mbclick mc;
	mc.clickid = eventId;
	mc.gameid = this->m_callback->getGameId();
	mc.userid = this->m_callback->getUserId();
	string data = mc.toString();
    sendData ( data, "pcclick" );
}

void WinHttpSDK::beginEvent ( const std::string &pageName, const std::string &enterdata )
{
	mbpageview mp;
	mp.eventid = pageName;
	mp.enterdata = enterdata;
	Poco::Timestamp ts;
	mp.entertime = static_cast<long>(ts.epochMicroseconds() / 1000);
	mp.userid = m_callback->getUserId();
	mp.gameid = m_callback->getGameId();

	{
		Poco::FastMutex::ScopedLock lock(this->m_mutex);
		this->m_pageview[pageName] = mp;
	}
}

void WinHttpSDK::endEvent ( const std::string &pageName, const std::string &leavedata )
{
	if (this->m_pageview.count(pageName) == 0)
		return;
	mbpageview& mp = this->m_pageview[pageName];
	mp.leavedata = leavedata;
	Poco::Timestamp ts;
	mp.leavetime = static_cast<long>(ts.epochMicroseconds() / 1000);
	string data = mp.toString();
	sendData(data, "pcpageview");

	{
		Poco::FastMutex::ScopedLock lock(this->m_mutex);
		// delete pEvent;
		this->m_pageview.erase(pageName);
	}
}

void WinHttpSDK::pubVideoCost ( const std::string &avgNetSpeed, const std::string &avgTimeCost )
{
	pubvideocost pvs;
	pvs.pubspeed = avgNetSpeed;
	pvs.pubtime = avgTimeCost;
	string data = pvs.toString();
    sendData ( data, "pcpubvideocost" );
}

void WinHttpSDK::quit()
{
    m_quitCalled = true;
    this->m_taskManager.cancelAll();
    this->m_failedQueue.wakeUpAll();
    this->m_sendQueue.wakeUpAll();
    this->m_taskManager.joinAll();
}


void WinHttpSDK::mcboxLogin ( const std::string &apid, const std::string &serverid )
{
	mcboxlogin ml;
	ml.pzid = apid;
	ml.serverid = serverid;
	string data = ml.toString();
	
    sendData ( data, "pcmcboxlogin" );
}

void WinHttpSDK::mcboxPeak ( const std::string &apid
                             , const  std::string &serverid
                             , long peakValue, int interval
                             , const std::string &peakTime  )
{
	mcboxpeak mp;
	mp.apid = apid;
	mp.serverid = serverid;
	mp.interval = Poco::NumberFormatter::format(interval);
	mp.peaktime = peakTime;
	mp.peakvalue = Poco::NumberFormatter::format(peakValue);
	string data = mp.toString();

    sendData ( data, "pcmcboxpeak" );
}
void WinHttpSDK::sendKVData ( const std::string &action, const std::string &data )
{
    string kvdata = data;
    //kvdata.append ( "&v=1" );
    sendData ( kvdata, action );
}

void WinHttpSDK::sendKVData(const std::string& action, const std::map<std::string, std::string>& dataItems)
{
	if (dataItems.size() == 0)
		return;
	ostringstream ostm;
	for (std::map<std::string, std::string>::const_iterator it = dataItems.begin(); it != dataItems.end(); ++it){
		ostm << it->first << "=" << SerDerUtil::toBase64(it->second)<<"&";
	}

	string s = ostm.str();
	s = s.substr(0, s.length() - 1);
	sendData(s,action);
}