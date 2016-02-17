// SeqIDGenerator.cpp: implementation of the CSeqIDGenerator class.
//
//////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
#include "SeqIDGenerator.h"

#include "MacFetcher.h"
#include "HddFetcher.h"
//#include "QQCrypt.h"

#include <windows.h>

//
// 其实没必要搞这么复杂，什么鬼网卡、硬盘，还MD5，不过QQ影音等研究院产品已经这么用了，所以还是保持这个算法不变。
// 这样就有可能和他们的产品联合分析数据。
//
// 此函数在我的烂机器上运行，大约耗时50毫秒。不可频繁调用。
// 
// 使用示例：
// GUID szTest = { 0 };
// CSeqIDGenerator::GetSequenceID((unsigned char*)&szTest);
//
// zhangqing 2009.12.16
//   
void CSeqIDGenerator::GetSequenceID(unsigned char pID[GUID_LEN])
{
	MACAddress oAddr;
	std::string strSerialNumber;
	std::string strModelNumber;
	std::string strRevisionNumber;

	memset(pID, 0, GUID_LEN);
	bool bMac = CMacFetcher::GetMAC(oAddr);
	bool bHdd = CHddFetcher::GetPhysicsDriveIdentify(strSerialNumber, strModelNumber, strRevisionNumber);
	if (bMac)
	{
		std::string strInfo = oAddr.ToString();
		if (bHdd)
		{
			/*strInfo += "\0x20" 
				+ strSerialNumber + "\0x20" 
				+ strModelNumber + "\0x20" 
				+ strRevisionNumber;*/		
			strInfo += strSerialNumber + strModelNumber + strRevisionNumber;
		}

		int len = strInfo.size() > GUID_LEN ? GUID_LEN : strInfo.size();
		memcpy(pID, strInfo.c_str(), len);
		//ASSERT_RET(GUID_LEN == MD5_DIGEST_LENGTH, NORET);
		//Md5HashBuffer(pID, (BYTE*)strInfo.c_str(), (int)strInfo.size());
	}
	else if (bHdd){
		std::string strInfo = strSerialNumber + strModelNumber + strRevisionNumber;
		int len = strInfo.size() > GUID_LEN ? GUID_LEN : strInfo.size();
		memcpy(pID, strInfo.c_str(), len);
	}
	else
	{
		::CoInitialize(NULL);

		GUID guid = { 0 };
		::CoCreateGuid(&guid); // 这个可能失败，不判断了，失败了就全用0.
		memcpy(pID, &guid, GUID_LEN);

		::CoUninitialize();
	}
}