// MacFetcher.cpp: implementation of the CMacFetcher class.
//
//////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
#include "MacFetcher.h"
//#include "FilePath.h"

#include <tchar.h>
#include <wtypes.h>
#include <winnt.h>

#if _MSC_VER <= 1200
#include "iphlpapi.h"
#else
#include <iphlpapi.h>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <lm.h>
#include <Nb30.h>
#pragma comment (lib, "Netapi32.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMacFetcher::CMacFetcher()
{

}

CMacFetcher::~CMacFetcher()
{

}

bool CMacFetcher::GetMACbyNetBiosRedir(MACAddress& addr)
{
	WKSTA_TRANSPORT_INFO_0 *pwkti = NULL; 
	DWORD dwEntriesRead;
	DWORD dwTotalEntries;
	BYTE *pbBuffer = NULL;
	
	// 通过 NetBIOS的枚举函数获取MAC地址
	NET_API_STATUS dwStatus = NetWkstaTransportEnum(
		NULL,
		0,
		&pbBuffer,
		MAX_PREFERRED_LENGTH,
		&dwEntriesRead,
		&dwTotalEntries,
		NULL);
	if (dwStatus != NERR_Success)
	{
		return false;
	}

	if (pbBuffer == NULL || dwEntriesRead == 0)
	{
		return false;
	}
	
	pwkti = (WKSTA_TRANSPORT_INFO_0 *)pbBuffer; // 指针类型转换
	
	for(DWORD i = 0; i < dwEntriesRead && pwkti; ++i)
	{ 
		short usMac[6] = { 0 };
		// 读取并打印MAC地址
#if _MSC_VER <= 1200
		if (swscanf((wchar_t *)pwkti[i].wkti0_transport_address,
			L"%2hx%2hx%2hx%2hx%2hx%2hx",
			&usMac[0],
			&usMac[1],
			&usMac[2], 
			&usMac[3],
			&usMac[4],
			&usMac[5]) == 6)
#else
		if (swscanf_s((wchar_t *)pwkti[i].wkti0_transport_address,
			L"%2hx%2hx%2hx%2hx%2hx%2hx",
			&usMac[0],
			&usMac[1],
			&usMac[2], 
			&usMac[3],
			&usMac[4],
			&usMac[5]) == 6)
#endif
		{
			for (int j = 0; j < 6; ++j)
			{
				addr.v[j] = (unsigned char)(usMac[j] & 0x00ff);
			}
			if (addr != MACAddress())
			{
				break;
			}
		}
	}

	NetApiBufferFree(pbBuffer);
	return true;
}

bool CMacFetcher::GetMACbyNetBiosStat(MACAddress& addr)
{
#pragma pack(push, 1)
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS adapt;
		NAME_BUFFER    NameBuff [30];
	}ASTAT;
#pragma pack(pop)
	
	ASTAT Adapter;
	NCB Ncb;
	LANA_ENUM lenum;
	
	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);
	if (NRC_GOODRET != Netbios(&Ncb))
	{
		return false;
	}

	for(int i = lenum.length - 1; i >= 0; --i)
	{
		// 重置网卡
		memset( &Ncb, 0, sizeof(Ncb) );
		Ncb.ncb_command = NCBRESET;
		Ncb.ncb_lana_num = lenum.lana[i];
		
		if (NRC_GOODRET != Netbios(&Ncb)) 
		{
			continue;
		}
		
		memset(&Adapter, 0 ,sizeof(Adapter));
 		memset(&Ncb, 0, sizeof(Ncb));
		Ncb.ncb_command = NCBASTAT;
		Ncb.ncb_lana_num = lenum.lana[i];
		
		Ncb.ncb_callname[0] = '*';
		Ncb.ncb_callname[1] = '\0';
		Ncb.ncb_buffer = (UCHAR *) &Adapter;
		Ncb.ncb_length = sizeof(Adapter);

		if (NRC_GOODRET == Netbios(&Ncb))
		{
			addr.v[0] = Adapter.adapt.adapter_address[0];
			addr.v[1] = Adapter.adapt.adapter_address[1];
			addr.v[2] = Adapter.adapt.adapter_address[2];
			addr.v[3] = Adapter.adapt.adapter_address[3];
			addr.v[4] = Adapter.adapt.adapter_address[4];
			addr.v[5] = Adapter.adapt.adapter_address[5];
			if (addr != MACAddress())
			{
				return true;
			}
		}
	}
	return false;
}

bool CMacFetcher::GetMACbyAdapter(MACAddress& addr)
{
    TCHAR szIphipapiDllPath[MAX_PATH] = { 0 };
    GetSystemDirectory(szIphipapiDllPath, MAX_PATH);
    _stprintf_s(szIphipapiDllPath, _countof(szIphipapiDllPath), _T("%s\\iphipapi.dll"), szIphipapiDllPath);
    HINSTANCE hDLLInst = LoadLibrary(szIphipapiDllPath);

	if (NULL == hDLLInst)
	{
		return false;
	}

	bool bRet = false;
	typedef DWORD(CALLBACK * GetAdaptersInfo)(PIP_ADAPTER_INFO, PULONG);
	
	GetAdaptersInfo pGetAdaptersInfo = NULL;
	pGetAdaptersInfo = (GetAdaptersInfo) GetProcAddress(hDLLInst, "GetAdaptersInfo");
	if (pGetAdaptersInfo)
	{
		IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
		// for up to 16 NICs
		DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer
		
		if (NO_ERROR == pGetAdaptersInfo(AdapterInfo, &dwBufLen))
		{
			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
			ADAPTER_TYPE enCurType = AT_UNKNOWN;
			//do 
			while(pAdapterInfo)
			{
				ADAPTER_TYPE enType = GetAdapterType(pAdapterInfo->AdapterName);
				if ((addr == MACAddress())
					|| (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET
					&& enCurType > enType))
				{
					addr.v[0] = pAdapterInfo->Address[0];
					addr.v[1] = pAdapterInfo->Address[1];
					addr.v[2] = pAdapterInfo->Address[2];
					addr.v[3] = pAdapterInfo->Address[3];
					addr.v[4] = pAdapterInfo->Address[4];
					addr.v[5] = pAdapterInfo->Address[5];
					enCurType = enType;
				}

				pAdapterInfo = pAdapterInfo->Next;
			}
		}	
	}

	if (NULL != hDLLInst)
	{
		FreeLibrary(hDLLInst);
		hDLLInst = NULL;
	}
	
	return (addr != MACAddress());
}

 CMacFetcher::ADAPTER_TYPE CMacFetcher::GetAdapterType(char * lpszAdapterName)
{
#define NET_CARD_KEY ("System\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
	ADAPTER_TYPE enType = AT_UNKNOWN;

	char szDataBuf[MAX_ADAPTER_NAME_LENGTH + 4];
	DWORD dwDataLen = MAX_ADAPTER_NAME_LENGTH + 4;
	DWORD dwRegType = REG_SZ;
	HKEY hNetKey = NULL;
	HKEY hLocalNet = NULL;

	if(ERROR_SUCCESS != RegOpenKeyExA(HKEY_LOCAL_MACHINE, NET_CARD_KEY, 0, KEY_READ, &hNetKey))
	{
		goto LABEL_ERROR_3;
	}

#if _MSC_VER <= 1200
	_snprintf(szDataBuf, 
		sizeof(szDataBuf) / sizeof(szDataBuf[0]) - 1,
		"%s\\Connection", lpszAdapterName);
#else
	_snprintf_s(szDataBuf, 
		sizeof(szDataBuf) / sizeof(szDataBuf[0]) - 1,
		"%s\\Connection", lpszAdapterName);
#endif
	if(ERROR_SUCCESS != RegOpenKeyExA(hNetKey ,szDataBuf ,0 ,KEY_READ, &hLocalNet))
	{
		goto LABEL_ERROR_2;
	}

	DWORD dwMediaSubType = 0;
	dwRegType = REG_DWORD;
	dwDataLen = sizeof(dwMediaSubType);
	if (ERROR_SUCCESS != RegQueryValueExA(hLocalNet, "MediaSubType", 0, &dwRegType, (BYTE *)&dwMediaSubType, &dwDataLen))
	{
		goto LABEL_ERROR_1;
	}

	dwRegType = REG_SZ;
	dwDataLen = MAX_ADAPTER_NAME_LENGTH + 4 - 1;
	memset(szDataBuf, 0, sizeof(szDataBuf));
	if (ERROR_SUCCESS != RegQueryValueExA(hLocalNet, "PnpInstanceID", 0, &dwRegType, (BYTE *)szDataBuf, &dwDataLen))
	{
		goto LABEL_ERROR_1;
	}

	// 如果MediaSubType = 1，且PnpInstanceID前面以PCI字样开头则表示是本地实际网卡
	// 如果MediaSubType = 1，且PnpInstanceID前面不以PCI字样开头则表示是本地虚拟网卡
	// 如果MediaSubType = 2，则表示是无线网卡
	if (dwMediaSubType == 1 && strncmp(szDataBuf, "PCI", strlen("PCI")) == 0)
	{
		enType = AT_CABLE;
	}
	else if (dwMediaSubType == 1 && strncmp(szDataBuf, "PCI", strlen("PCI")) != 0)
	{
		enType = AT_VIRTUAL;
	}
	else if (dwMediaSubType == 2)
	{
		enType = AT_WIRENESS;
	}
	else
	{
		enType = AT_UNKNOWN;
	}

LABEL_ERROR_1:
	RegCloseKey(hLocalNet);
LABEL_ERROR_2:
	RegCloseKey(hNetKey);
LABEL_ERROR_3:

	return enType;
}

bool CMacFetcher::GetMAC(MACAddress& addr)
{
	if (!GetMACbyAdapter(addr))
	{
		if (!GetMACbyNetBiosRedir(addr))
		{
			if (!GetMACbyNetBiosStat(addr))
			{
				return false;
			}
		}
	}

	return true;
}
