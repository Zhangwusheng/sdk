#include "RptStratrgy.h"
#include "Poco/Base64Decoder.h"
#include "Poco/Base64Encoder.h"
#include "Wincrypt.h"
#include "Poco/URI.h"
#include "Poco/Random.h"
//S#include "PBUtil.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include "RptUtil.h"
using namespace std;
using namespace Poco;


bool RptStrategy::loadLocalStrategy()
{
	string result;

	if (!RptUtil::getLocalStrategyString(result)) {
		return false;
	}

	try {
		m_protocol->deserialize(result);
		return true;
	}
	catch ( ... ) {
		return false;
	}
}

bool RptStrategy::loadHttpStrategy ( const std::string &httpResponse )
{
	try {
		m_protocol->deserialize(httpResponse);
		writeStrategyToLocal();
		return true;
	}
	catch ( ... ) {
		return false;
	}
	return false;
}

bool RptStrategy::writeStrategyToLocal()
{
	string base64 = m_protocol->serialize();
	return RptUtil::writeStrategyStringToLocal(base64);
}

#if 0

bool  RptStrategy::getLocalFileLocation(std::string &location)
{
	char buffer[MAX_PATH];
	DWORD n = GetCurrentDirectoryA(sizeof (buffer), buffer);
	std::string result;
	if (n <= 0 || n >= sizeof (buffer)) {
		return false;
	}

	result.assign(buffer, n);
	if (result[n - 1] != '\\') {
		result.append("\\");
	}

	result.append("Strategy.txt");

	location = result;
	return true;
}

bool RptStrategy::loadLocalStrategy()
{
	string result;
	if (!getLocalFileLocation(result)) {
		return false;
	}
	ifstream ifs(result.c_str());
	if (!ifs) {
		return false;
	}

	char line[2048] = { 0 };
	string base64;

	while (ifs.getline(line, sizeof (line))) {
		base64.append(line);
	}

	return loadHttpStrategy(base64);

}

bool RptStrategy::loadDefaultStrategy()
{
	m_rptstrat.set_enabled(true);
	m_rptstrat.set_domainretrycount(2);
	m_rptstrat.add_iplist("14.29.84.204");
	m_rptstrat.add_iplist("14.29.84.205");
	m_rptstrat.add_iplist("14.29.84.206");
	m_rptstrat.add_iplist("14.29.84.207");

	::aipai::log::strategy::RptKVItemSI *as1 = m_rptstrat.add_actionsample();
	as1->set_key("mbstartup");
	as1->set_value(100);

	::aipai::log::strategy::RptKVItemSI *as2 = m_rptstrat.add_actionsample();
	as1->set_key("mbclick");
	as1->set_value(100);

	m_rptstrat.set_cacheclickevents(false);
	m_rptstrat.set_clickeventinterval(5);
	m_rptstrat.set_clickeventitems(5);

	::aipai::log::strategy::RptKVItemSI *ap1 = m_rptstrat.add_actionpriority();
	as1->set_key("mbstartup");
	as1->set_value(10000);

	::aipai::log::strategy::RptKVItemSI *ap2 = m_rptstrat.add_actionpriority();
	as1->set_key("mbclick");
	as1->set_value(10000);

	m_rptstrat.set_defaultpriority(1);
	m_rptstrat.set_eventpartition(0);
	m_rptstrat.set_sendthreadnum(2);

	return true;
}

bool RptStrategy::writeStrategyToLocal()
{
	string result;
	if (!getLocalFileLocation(result)) {
		return false;
	}

	string base64 = PBUtil::serializeProtobufMessage(this->m_rptstrat);

	ofstream ofs(result.c_str());
	ofs << base64 << endl;
	ofs.close();

	//cout << "RptStrategy::writeStrategyToLocal" << endl;
	return true;
}

bool RptStrategy::loadHttpStrategy(const std::string &httpResponse)
{
	try {
		PBUtil::deserializeProtobufMessage(httpResponse, m_rptstrat);
		writeStrategyToLocal();
		return true;
	}
	catch (Poco::Exception &) {
		return false;
	}
	return false;
}

std::string RptStrategy::getRandomIPStr() const
{
	int n = m_rptstrat.iplist_size();
	if (n == 0) {
		return "mlog.aipai.com";
	}
	Poco::Random rnd;
	n = rnd.next(n);
	return m_rptstrat.iplist().Get(n);
}
#endif // 0
