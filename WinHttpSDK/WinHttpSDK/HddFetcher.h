// HddFetcher.h: interface for the CHddFetcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HDDFETCHER_H__ADBC514B_6450_4716_927B_78425F13EF12__INCLUDED_)
#define AFX_HDDFETCHER_H__ADBC514B_6450_4716_927B_78425F13EF12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#if _MSC_VER == 1200
#pragma warning( disable : 4251)
#endif
#endif

#include <string>

class CHddFetcher  
{
public:
	CHddFetcher();
	virtual ~CHddFetcher();

	static bool GetPhysicsDriveIdentify(std::string& serialNumber,
		std::string& modelNumber,
		std::string& revisionNumber);

	static bool GetPhysicsDriveIdentify(std::wstring& serialNumber,
		std::wstring& modelNumber,
		std::wstring& revisionNumber);

#ifdef _TEST
public:
#else
protected:
#endif
	static bool ReadPhysicalDriveInNTWithAdminRights(std::string& serialNumber,
		std::string& modelNumber,
		std::string& revisionNumber);
	static bool ReadPhysicalDriveInNTUsingSmart(std::string& serialNumber,
		std::string& modelNumber,
		std::string& revisionNumber);
	static bool ReadPhysicalDriveInNTWithZeroRights(std::string& serialNumber,
		std::string& modelNumber,
		std::string& revisionNumber);
	static bool ReadDrivePortsInWin9X(std::string& serialNumber,
		std::string& modelNumber,
		std::string& revisionNumber);
	static bool ReadIdeDriveAsScsiDriveInNT(std::string& serialNumber,
		std::string& modelNumber,
		std::string& revisionNumber);
	static bool ReadPhysicalDriveByRegister(std::string& serialNumber,
		std::string& modelNumber,
		std::string& revisionNumber);
};

#endif // !defined(AFX_HDDFETCHER_H__ADBC514B_6450_4716_927B_78425F13EF12__INCLUDED_)
