#ifndef WinHttpSDK_H
#define WinHttpSDK_H

#include "Poco/AtomicCounter.h"
#include "Poco/TaskManager.h"
#include "HttpEventArgs.h"
#include "aprptstrategy.pb.h"
#include "Poco/Environment.h"
#include "Poco/PriorityNotificationQueue.h"
#include "Poco/NotificationQueue.h"
#include "Poco/Mutex.h"
#include "HttpSdkCallbackIntf.h"
#include "DefaultSdkCallback.h"
#include "RptStratrgy.h"
#include "mbactions.h"
#include "Protocols.h"

class WinHttpSDK {
    public:
        /**
         *
         */
        WinHttpSDK() {
            m_quitCalled = false;
        }

        ~WinHttpSDK() {
            if ( !m_quitCalled ) {
                this->quit();
            }
        }

        void appStartLaunchWithAppKey (
            const std::string &appkey
            , const std::string &channel
            , HttpSdkCallbackIntf *callback );

        /**
         *
         */
        void startupEvent();

        /**
         *
         */
        void beginLogPageView ( const std::string &pageName, const std::string &enterdata );
        void endLogPageView ( const std::string &pageName, const std::string &enterdata );
        void clickEvent ( const std::string &eventId );
        void beginEvent ( const std::string &pageName, const std::string &enterdata );
        void endEvent ( const std::string &pageName, const std::string &enterdata );
        void quit();
        void pubVideoCost ( const std::string &avgNetSpeed, const std::string &avgTimeCost );
        void mcboxLogin ( const std::string &apid, const std::string &serverid );
        void mcboxPeak ( const std::string &apid
                         , const  std::string &serverid
                         , long peakValue, int interval
                         , const std::string &peakTime  );

        void onStrategyLoaded ( const void *pSender, HttpEventArgs &args );

        Poco::PriorityNotificationQueue &sendQueue() {
            return m_sendQueue;
        }

        Poco::NotificationQueue &failQueue() {
            return m_failedQueue;
        }

        const RptStrategy &rptStrategy() const {
            return m_AipaiRptStrategy;
        }
		RptStrategy &rptStrategy() {
			return m_AipaiRptStrategy;
		}
        void sendKVData ( const std::string &action, const std::string &data );
		void sendKVData(const std::string& action, const std::map<std::string, std::string>& dataItems);
    private:

        void sendData ( const std::string &data, const std::string &actionName );
        Poco::FastMutex m_mutex;
        //std::map < std::string, aipai::log::action::mbpageview *> m_eventPageViewMap;
        //std::map < std::string, aipai::log::action::mbcustevent *> m_eventCustEventMap;
        Poco::PriorityNotificationQueue m_sendQueue;
        Poco::NotificationQueue m_failedQueue;
        Poco::NotificationQueue m_cacheQueue;

        Poco::TaskManager m_taskManager;
        Poco::AtomicCounter m_AtomicCounter;
        RptStrategy m_AipaiRptStrategy;
        HttpSdkCallbackIntf *m_callback;
        std::string m_hdrString;
        std::string m_hdrKVString;
        DefaultSdkCallback m_NullSdkCallback;

        bool m_quitCalled;
		map<string, mbpageview> m_pageview;
};

#endif
