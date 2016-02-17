#ifndef RptHdrObj_H
#define RptHdrObj_H

#include "HttpSdkCallbackIntf.h"

class RptHdrObj {
public:
	std::string toString();

	std::string m_appkey;
	std::string m_channel;
	std::string m_osVersion;
	std::string m_osName;
	std::string m_osType;
	std::string m_machindId;
	std::string m_osVendor;

	std::string m_statGuid;
	HttpSdkCallbackIntf* m_SdkCallback;
};

#endif
