// WinHttpTest.cpp : 定义控制台应用程序的入口点。
//
#ifndef HttpDataNotification_H
#define HttpDataNotification_H

#include "Poco/Notification.h"
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Timestamp.h"
#include "Poco/NumberFormatter.h"
#include "Poco/AutoPtr.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timezone.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/String.h"

class HttpDataNotification: public Poco::Notification
/// Base class for TaskManager notifications.
{
    public:
        typedef Poco::AutoPtr<HttpDataNotification> Ptr;
        HttpDataNotification() {
            //Poco::DateTime now;
            //Poco::DateTime d1970 ( 1970, 1, 1 );
            //Poco::Timespan ts = now - d1970;
            Poco::Timestamp ts;
            m_clitime = ts.epochMicroseconds() / 1000;

            Poco::UUIDGenerator &gen = Poco::UUIDGenerator::defaultGenerator();
            Poco::UUID uuid = gen.createOne();
            m_logid = uuid.toString();
            m_logid = Poco::remove ( m_logid, '-' );

            m_nFailedCount = 0;
        }
        /// Creates the TaskNotification.
        void setHdrData ( const std::string &hdrstr ) {
            m_hdrstr = hdrstr;
        }
        std::string getHdrData() const {
            return m_hdrstr;
        }

        void setActionName ( const std::string &data ) {
            m_action = data;
        }
        std::string getActionName() const {
            return m_action;
        }

        std::string getLogid() {
            return m_logid;
        }

        std::string getCliTime() {
            return Poco::NumberFormatter::format ( m_clitime );
        }

        std::string getData() const {
            return m_data;
        }

        void setData ( const std::string &data ) {
            m_data = data;
        }

        void setPriority ( int n ) {
            m_priority = n;
        }

        int getPriority() {
            return m_priority;
        }


        int getFailedCount() {
            return m_nFailedCount;
        }

        void incrementFailCount() {
            m_nFailedCount++;
        }
		
		std::string toString() {
			std::string result;
			result.append("action=");
			result.append(m_action);
			result.append("&");
			result.append(m_hdrstr);
			result.append("&logid=");
			result.append(m_logid);
			result.append("&clitime=");
			result.append(getCliTime());
			result.append("&");
			result.append(m_data);
			return result;
		}

    protected:
        virtual ~HttpDataNotification() {};
        /// Destroys the TaskNotification.

    private:
        int m_priority;
        int m_nFailedCount;
        std::string m_action;
        std::string m_hdrstr;
        //std::string m_appkey;
        Poco::Timestamp::TimeDiff m_clitime;
        std::string m_logid;
        std::string m_data;

        //std::string m_hdrKVstr;
};
#endif
