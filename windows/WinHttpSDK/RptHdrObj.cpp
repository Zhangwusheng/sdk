#include "Poco/Base64Decoder.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Buffer.h"
#include "Poco/URI.h"
#include "Poco/Environment.h"
#include "Wincrypt.h"
#include "hdr.pb.h"
#include "RptHdrObj.h"
//S#include "PBUtil.h"
#include "SerDerUtil.h"
#include <sstream>

using namespace std;
using namespace Poco;


std::string RptHdrObj::toString()
{
	ostringstream ostm;
	ostm << "appkey=" << m_appkey
		<< "&channel=" << SerDerUtil::toBase64(m_channel)
		<< "&machineid=" << SerDerUtil::toBase64(m_machindId)
		<< "&loaderver=" << SerDerUtil::toBase64(m_SdkCallback->getLoaderVer())
		<< "&addonver=" << SerDerUtil::toBase64(m_SdkCallback->getAddonVer())
		<< "&ostype=" << SerDerUtil::toBase64(m_osType)
		<< "&osver=" << SerDerUtil::toBase64(m_osVersion)
		<< "vendor=" << SerDerUtil::toBase64(m_osVendor)
		<< "&appver=" << SerDerUtil::toBase64(m_SdkCallback->getAppVer())
		<< "&networktype=" << SerDerUtil::toBase64("notyet")
		<< "&phone=" << SerDerUtil::toBase64("notyet")
		<< "&guid=" << m_statGuid
		<< "&v=1";

	return ostm.str();
}