#ifndef RptStrategy_H
#define RptStrategy_H

#include "GlobalConfig.h"
#include "Poco/AtomicCounter.h"
#include "Poco/TaskManager.h"
#include "HttpEventArgs.h"
#include "aprptstrategy.pb.h"
#include "Poco/Environment.h"
#include "Poco/PriorityNotificationQueue.h"
#include "Poco/NotificationQueue.h"
#include "DefaultSdkCallback.h"
#include "StrategyProtocol_KV.h"
#include "Poco/SharedPtr.h"

class	RptStrategy {

public:

	RptStrategy(){
		m_curIpIndex = 0;
		m_protocol = new KVStrategyProtocol;
	}

	bool loadHttpStrategy(const std::string &httpResponse);
	bool loadLocalStrategy();
	bool writeStrategyToLocal();

	std::string getRandomIPStr(){
		m_curIpIndex++;
		if (m_curIpIndex > 100000)
			m_curIpIndex = 0;

		size_t n = m_protocol->iplist.size();
		if (n == 0) {
			return "mlog.aipai.com";
		}
		else
			n = (m_curIpIndex % n);

		return m_protocol->iplist[n];
	}

	int getActionPriority(const std::string &action) {
		std::map<std::string, int>::iterator it = m_protocol->actionPriority.find(action);
		if (it != m_protocol->actionPriority.end())
			return it->second;

		return m_protocol->defaultPriority;
	}

	int getSendThreadNum() const {
		if (m_protocol->sendThreadNum >= 1 && m_protocol->sendThreadNum <= 3) {
			return m_protocol->sendThreadNum;
		}
		return 1;
	}

	int getHttpRetryNum() const {
		if (m_protocol->domainRetryCount >= 1 && m_protocol->domainRetryCount <= 3) {
			return m_protocol->domainRetryCount;
		}
		return 2;
	}
private:
	//    bool getLocalFileLocation ( std::string & );
	int m_curIpIndex;//使用一个循环的下标来达到随机的效果
	Poco::SharedPtr<StrategyProtocol> m_protocol;
};

#endif
