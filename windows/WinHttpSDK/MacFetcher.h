// MacFetcher.h: interface for the CMacFetcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MACFETCHER_H__D52BC07C_96B8_45EA_8B53_5F678891758A__INCLUDED_)
#define AFX_MACFETCHER_H__D52BC07C_96B8_45EA_8B53_5F678891758A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#if _MSC_VER == 1200
#pragma warning( disable : 4251)
#endif
#endif

#include <ostream>
#include <sstream>
#include <iomanip>
#include <tchar.h>

#pragma pack(push, 1)
struct MACAddress
{
	inline MACAddress() {
		memset(&v, 0, sizeof(v));
	}
	
	inline MACAddress(const MACAddress& val) {
		for (int i = 0; i < sizeof(v) / sizeof(v[0]); ++i) {
			v[i] = val.v[i];
		}
	}

	inline MACAddress& operator = (const MACAddress& rhs) {
		for (int i = 0; i < sizeof(v) / sizeof(v[0]); ++i) {
			v[i] = rhs.v[i];
		}
	}

	inline bool operator == (const MACAddress& rhs) const {
		for (int i = 0; i < sizeof(v) / sizeof(v[0]); ++i) {
			if (v[i] != rhs.v[i]) return false;
		}
		return true;
	}

	inline bool operator != (const MACAddress& rhs) const {
		return !(operator==(rhs));
	}
	
	friend std::ostream& operator << (std::ostream& os, const MACAddress& v) {
		for (int i = 0; i < sizeof(v.v); ++i) {
			os << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)v.v[i] << " ";
		}
		return os;
	}

	friend std::wostream& operator << (std::wostream& os, const MACAddress& v) {
		for (int i = 0; i < sizeof(v.v); ++i) {
			os << std::hex << std::setw(2) << std::setfill(L'0') << (unsigned int)v.v[i] << _T(" ");
		}
		return os;
	}
	
	std::string ToString() const
	{
		std::ostringstream oss ;
		for (int i = 0; i < sizeof(v); ++i) {
			oss << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)v[i] ;
		}
		return oss.str();
	}

	std::wstring ToWstring() const
	{
		std::wostringstream oss ;
		for (int i = 0; i < sizeof(v); ++i) {
			oss << std::hex << std::setw(2) << std::setfill(L'0') << (unsigned int)v[i] ;
		}
		return oss.str();
	}

	unsigned char v[6];
};
#pragma pack(pop)

class CMacFetcher  
{
public:
	CMacFetcher();
	virtual ~CMacFetcher();

	static bool GetMAC(MACAddress& addr);

#ifdef _TEST
public:
#else
protected:
#endif
	static bool GetMACbyNetBiosRedir(MACAddress& addr);
	static bool GetMACbyNetBiosStat(MACAddress& addr);
	static bool GetMACbyAdapter(MACAddress& addr);

private:
	enum ADAPTER_TYPE
	{
		AT_CABLE = 0
		, AT_WIRENESS
		, AT_VIRTUAL
		, AT_UNKNOWN
	};
	static ADAPTER_TYPE GetAdapterType(char * lpszAdapterName);
};

#endif // !defined(AFX_MACFETCHER_H__D52BC07C_96B8_45EA_8B53_5F678891758A__INCLUDED_)
