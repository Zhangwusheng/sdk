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
// ��ʵû��Ҫ����ô���ӣ�ʲô��������Ӳ�̣���MD5������QQӰ�����о�Ժ��Ʒ�Ѿ���ô���ˣ����Ի��Ǳ�������㷨���䡣
// �������п��ܺ����ǵĲ�Ʒ���Ϸ������ݡ�
//
// �˺������ҵ��û��������У���Լ��ʱ50���롣����Ƶ�����á�
// 
// ʹ��ʾ����
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
		::CoCreateGuid(&guid); // �������ʧ�ܣ����ж��ˣ�ʧ���˾�ȫ��0.
		memcpy(pID, &guid, GUID_LEN);

		::CoUninitialize();
	}
}