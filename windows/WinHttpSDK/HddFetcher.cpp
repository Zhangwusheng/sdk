// HddFetcher.cpp: implementation of the CHddFetcher class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _WIN32_WINNT 
#define _WIN32_WINNT 0x0500
#endif
//#include "stdafx.h"
#include "HddFetcher.h"

#include <windows.h>
#include <winioctl.h>

#include <tchar.h>
#include <windef.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define  DFP_GET_VERSION          0x00074080
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition

#pragma warning(disable:4267)
#pragma warning(disable:4312)

// vc2008的头文件winioctrl.h已经定义了如下结构体[2/23/2010 ysq]
#if _MSC_VER <=1400 // VC2005以下才自己定义. 

#pragma pack(1)

typedef struct _IDENTIFY_DATA {
    USHORT GeneralConfiguration;            // 00 00
    USHORT NumberOfCylinders;               // 02  1
    USHORT Reserved1;                       // 04  2
    USHORT NumberOfHeads;                   // 06  3
    USHORT UnformattedBytesPerTrack;        // 08  4
    USHORT UnformattedBytesPerSector;       // 0A  5
    USHORT SectorsPerTrack;                 // 0C  6
    USHORT VendorUnique1[3];                // 0E  7-9
    USHORT SerialNumber[10];                // 14  10-19
    USHORT BufferType;                      // 28  20
    USHORT BufferSectorSize;                // 2A  21
    USHORT NumberOfEccBytes;                // 2C  22
    USHORT FirmwareRevision[4];             // 2E  23-26
    USHORT ModelNumber[20];                 // 36  27-46
    UCHAR  MaximumBlockTransfer;            // 5E  47
    UCHAR  VendorUnique2;                   // 5F
    USHORT DoubleWordIo;                    // 60  48
    USHORT Capabilities;                    // 62  49
    USHORT Reserved2;                       // 64  50
    UCHAR  VendorUnique3;                   // 66  51
    UCHAR  PioCycleTimingMode;              // 67
    UCHAR  VendorUnique4;                   // 68  52
    UCHAR  DmaCycleTimingMode;              // 69
    USHORT TranslationFieldsValid:1;        // 6A  53
    USHORT Reserved3:15;
    USHORT NumberOfCurrentCylinders;        // 6C  54
    USHORT NumberOfCurrentHeads;            // 6E  55
    USHORT CurrentSectorsPerTrack;          // 70  56
    ULONG  CurrentSectorCapacity;           // 72  57-58
    USHORT CurrentMultiSectorSetting;       //     59
    ULONG  UserAddressableSectors;          //     60-61
    USHORT SingleWordDMASupport : 8;        //     62
    USHORT SingleWordDMAActive : 8;
    USHORT MultiWordDMASupport : 8;         //     63
    USHORT MultiWordDMAActive : 8;
    USHORT AdvancedPIOModes : 8;            //     64
    USHORT Reserved4 : 8;
    USHORT MinimumMWXferCycleTime;          //     65
    USHORT RecommendedMWXferCycleTime;      //     66
    USHORT MinimumPIOCycleTime;             //     67
    USHORT MinimumPIOCycleTimeIORDY;        //     68
    USHORT Reserved5[2];                    //     69-70
    USHORT ReleaseTimeOverlapped;           //     71
    USHORT ReleaseTimeServiceCommand;       //     72
    USHORT MajorRevision;                   //     73
    USHORT MinorRevision;                   //     74
    USHORT Reserved6[50];                   //     75-126
    USHORT SpecialFunctionsEnabled;         //     127
    USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;

#pragma pack(4)

typedef struct _STORAGE_DEVICE_DESCRIPTOR {
	
    //
    // Sizeof(STORAGE_DEVICE_DESCRIPTOR)
    //
	
    ULONG Version;
	
    //
    // Total size of the descriptor, including the space for additional
    // data and id strings
    //
	
    ULONG Size;
	
    //
    // The SCSI-2 device type
    //
	
    UCHAR DeviceType;
	
    //
    // The SCSI-2 device type modifier (if any) - this may be zero
    //
	
    UCHAR DeviceTypeModifier;
	
    //
    // Flag indicating whether the device's media (if any) is removable.  This
    // field should be ignored for media-less devices
    //
	
    BOOLEAN RemovableMedia;
	
    //
    // Flag indicating whether the device can support mulitple outstanding
    // commands.  The actual synchronization in this case is the responsibility
    // of the port driver.
    //
	
    BOOLEAN CommandQueueing;
	
    //
    // Byte offset to the zero-terminated ascii string containing the device's
    // vendor id string.  For devices with no such ID this will be zero
    //
	
    ULONG VendorIdOffset;
	
    //
    // Byte offset to the zero-terminated ascii string containing the device's
    // product id string.  For devices with no such ID this will be zero
    //
	
    ULONG ProductIdOffset;
	
    //
    // Byte offset to the zero-terminated ascii string containing the device's
    // product revision string.  For devices with no such string this will be
    // zero
    //
	
    ULONG ProductRevisionOffset;
	
    //
    // Byte offset to the zero-terminated ascii string containing the device's
    // serial number.  For devices with no serial number this will be zero
    //
	
    ULONG SerialNumberOffset;
	
    //
    // Contains the bus type (as defined above) of the device.  It should be
    // used to interpret the raw device properties at the end of this structure
    // (if any)
    //
	
    /*STORAGE_BUS_TYPE*/int BusType;
	
    //
    // The number of bytes of bus-specific data which have been appended to
    // this descriptor
    //
	
    ULONG RawPropertiesLength;
	
    //
    // Place holder for the first byte of the bus specific property data
    //
	
    UCHAR RawDeviceProperties[1];
	
} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;


//  Required to ensure correct PhysicalDrive IOCTL structure setup
#pragma pack(4)


//
// IOCTL_STORAGE_QUERY_PROPERTY
//
// Input Buffer:
//      a STORAGE_PROPERTY_QUERY structure which describes what type of query
//      is being done, what property is being queried for, and any additional
//      parameters which a particular property query requires.
//
//  Output Buffer:
//      Contains a buffer to place the results of the query into.  Since all
//      property descriptors can be cast into a STORAGE_DESCRIPTOR_HEADER,
//      the IOCTL can be called once with a small buffer then again using
//      a buffer as large as the header reports is necessary.
//


//
// Types of queries
//

typedef enum _STORAGE_QUERY_TYPE {
    PropertyStandardQuery = 0,          // Retrieves the descriptor
		PropertyExistsQuery,                // Used to test whether the descriptor is supported
		PropertyMaskQuery,                  // Used to retrieve a mask of writeable fields in the descriptor
		PropertyQueryMaxDefined     // use to validate the value
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

//
// define some initial property id's
//

typedef enum _STORAGE_PROPERTY_ID {
    StorageDeviceProperty = 0,
		StorageAdapterProperty
} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

//
// Query structure - additional parameters for specific queries can follow
// the header
//

typedef struct _STORAGE_PROPERTY_QUERY {
	
    //
    // ID of the property being retrieved
    //
	
    STORAGE_PROPERTY_ID PropertyId;
	
    //
    // Flags indicating the type of query being performed
    //
	
    STORAGE_QUERY_TYPE QueryType;
	
    //
    // Space for additional parameters if necessary
    //
	
    UCHAR AdditionalParameters[1];
	
} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif   // #if _MSC_VER <=1400  

// The following struct defines the interesting part of the IDENTIFY
// buffer:
typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;



typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;



#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE

#pragma pack(1)

struct GDT_DESCRIPTOR
{
	WORD Limit_0_15;
	WORD Base_0_15;
	BYTE Base_16_23;
	BYTE Type         : 4;
	BYTE System       : 1;
	BYTE DPL          : 2;
	BYTE Present      : 1;
	BYTE Limit_16_19  : 4;
	BYTE Available    : 1;
	BYTE Reserved     : 1;
	BYTE D_B          : 1;
	BYTE Granularity  : 1;
	BYTE Base_24_31;
};

struct CALLGATE_DESCRIPTOR
{
	WORD Offset_0_15;
	WORD Selector;
	WORD ParamCount   : 5;
	WORD Unused       : 3;
	WORD Type         : 4;
	WORD System       : 1;
	WORD DPL          : 2;
	WORD Present      : 1;
	WORD Offset_16_31;
};



struct GDTR
{
	WORD wGDTLimit;
	DWORD dwGDTBase;
};

namespace _XXX__
{
	enum { MAX_IDE_DRIVES = 16};

	bool Encode(const std::string& in, std::wstring& out);

	bool GetPortVal(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
	bool SetPortVal(WORD wPortAddr, DWORD dwPortVal, BYTE bSize);
	bool CallRing0(PVOID pvRing0FuncAddr, 
		WORD wPortAddr, 
		PDWORD pdwPortVal, 
		BYTE bSize);
	
	bool DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
		PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
		PDWORD lpcbBytesReturned);

	std::string ConvertToString(USHORT* pBuf, 
		int firstIndex,
		int lastIndex);
	char * flipAndCodeBytes (const char * str, int pos, int flip, char * buf);

	bool Encode(const std::string& in, std::wstring& out)
	{
		int iRetBytes = MultiByteToWideChar(CP_ACP, 
			0, 
			in.c_str(), 
			in.length(), 
			0, 
			0);
		if (0 == iRetBytes)
		{
			return false;
		}
		out.resize(iRetBytes);
		return (0 != MultiByteToWideChar(CP_ACP, 
			0, 
			in.c_str(), 
			in.length(), 
			&out[0], 
			iRetBytes));
	}
	__declspec(naked) void Ring0GetPortVal()
	{
		_asm
		{
			Cmp CL, 1
				Je ByteVal
				Cmp CL, 2
				Je WordVal
				Cmp CL, 4
				Je DWordVal
				
ByteVal:
			
			In AL, DX
				Mov [EBX], AL
				Retf
				
WordVal:
			
			In AX, DX
				Mov [EBX], AX
				Retf
				
DWordVal:
			
			In EAX, DX
				Mov [EBX], EAX
				Retf
		}
	}
	
	__declspec(naked) void Ring0SetPortVal()
	{
		_asm
		{
			Cmp CL, 1
				Je ByteVal
				Cmp CL, 2
				Je WordVal
				Cmp CL, 4
				Je DWordVal
				
ByteVal:
			
			Mov AL, [EBX]
				Out DX, AL
				Retf
				
WordVal:
			
			Mov AX, [EBX]
				Out DX, AX
				Retf
				
DWordVal:
			
			Mov EAX, [EBX]
				Out DX, EAX
				Retf
		}
	}

	bool DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
		PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
		PDWORD lpcbBytesReturned)
	{
		if (INVALID_HANDLE_VALUE == hPhysicalDriveIOCTL
			|| NULL == pSCIP 
			|| NULL == pSCOP 
			|| NULL == lpcbBytesReturned)
		{
			return false;
		}
		
		// Set up data structures for IDENTIFY command.
		pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
		pSCIP -> irDriveRegs.bFeaturesReg = 0;
		pSCIP -> irDriveRegs.bSectorCountReg = 1;
		//pSCIP -> irDriveRegs.bSectorNumberReg = 1;
		pSCIP -> irDriveRegs.bCylLowReg = 0;
		pSCIP -> irDriveRegs.bCylHighReg = 0;
		
		// Compute the drive number.
		pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);
		
		// The command can either be IDE identify or ATAPI identify.
		pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
		pSCIP -> bDriveNumber = bDriveNum;
		pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
		
		return bool(TRUE == DeviceIoControl(hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
			(LPVOID) pSCIP,
			sizeof(SENDCMDINPARAMS) - 1,
			(LPVOID) pSCOP,
			sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
			lpcbBytesReturned, NULL));
	}
		
	std::string ConvertToString(USHORT* pBuf, 
		int firstIndex,
		int lastIndex)
	{
		std::string strRet;
		
		//  each integer has two characters stored in it backwards
		for (int index = firstIndex; index <= lastIndex; ++index)
		{
			//  get high byte for 1st character
			strRet += (char) (pBuf [index] / 256);
			
			//  get low byte for 2nd character
			strRet += (char) (pBuf [index] % 256);
		}
		
		while (!strRet.empty() && strRet[strRet.length() - 1] == '\x20')
			strRet.erase(strRet.length() - 1, 1);
		
		return strRet;
	}
		
	char * flipAndCodeBytes (const char * str,
		int pos,
		int flip,
		char * buf)
	{
		int i;
		int j = 0;
		int k = 0;
		
		buf [0] = '\0';
		if (pos <= 0)
			return buf;
		
		if ( ! j)
		{
			char p = 0;
			
			// First try to gather all characters representing hex digits only.
			j = 1;
			k = 0;
			buf[k] = 0;
			for (i = pos; j && str[i] != '\0'; ++i)
			{
				char c = tolower(str[i]);
				
				if (isspace(c))
					c = '0';
				
				++p;
				buf[k] <<= 4;
				
				if (c >= '0' && c <= '9')
					buf[k] |= (unsigned char) (c - '0');
				else if (c >= 'a' && c <= 'f')
					buf[k] |= (unsigned char) (c - 'a' + 10);
				else
				{
					j = 0;
					break;
				}
				
				if (p == 2)
				{
					if (buf[k] != '\0' && ! isprint((int)((unsigned char)buf[k])))
					{
						j = 0;
						break;
					}
					++k;
					p = 0;
					buf[k] = 0;
				}
				
			}
		}
		
		if ( ! j)
		{
			// There are non-digit characters, gather them as is.
			j = 1;
			k = 0;
			for (i = pos; j && str[i] != '\0'; ++i)
			{
				char c = str[i];
				
				if ( ! isprint(c))
				{
					j = 0;
					break;
				}
				
				buf[k++] = c;
			}
		}
		
		if ( ! j)
		{
			// The characters are not there or are not printable.
			k = 0;
		}
		
		buf[k] = '\0';
		
		if (flip)
		{
			// Flip adjacent characters
			for (j = 0; j < k; j += 2)
			{
				char t = buf[j];
				buf[j] = buf[j + 1];
				buf[j + 1] = t;
			}
		}
		
		// Trim any beginning and end space
		i = j = -1;
		for (k = 0; buf[k] != '\0'; ++k)
		{
			if (! isspace(buf[k]))
			{
				if (i < 0)
					i = k;
				j = k;
			}
		}
		
		if ((i >= 0) && (j >= 0))
		{
			for (k = i; (k <= j) && (buf[k] != '\0'); ++k)
				buf[k - i] = buf[k];
			buf[k - i] = '\0';
		}
		
		return buf;
	}

	bool GetPortVal(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize)
	{
		return CallRing0((PVOID)Ring0GetPortVal, wPortAddr, pdwPortVal, bSize);
	}

	bool SetPortVal(WORD wPortAddr, DWORD dwPortVal, BYTE bSize)
	{
		return CallRing0((PVOID)Ring0SetPortVal, wPortAddr, &dwPortVal, bSize);
	}

	bool CallRing0(PVOID pvRing0FuncAddr, 
								WORD wPortAddr, 
								PDWORD pdwPortVal, 
								BYTE bSize)
	{
		struct GDT_DESCRIPTOR *pGDTDescriptor;
		struct GDTR gdtr;
		WORD CallgateAddr[3];
		WORD wGDTIndex = 1;
		
		gdtr.dwGDTBase = 0;
		gdtr.wGDTLimit = 0;
		
		_asm Sgdt [gdtr]
			
			// Skip the null descriptor
			pGDTDescriptor = (struct GDT_DESCRIPTOR *)(gdtr.dwGDTBase + 8);
		
		// Search for a free GDT descriptor
		for (wGDTIndex = 1; wGDTIndex < (gdtr.wGDTLimit / 8); wGDTIndex++)
		{
			if (pGDTDescriptor->Type == 0     &&
				pGDTDescriptor->System == 0   &&
				pGDTDescriptor->DPL == 0      &&
				pGDTDescriptor->Present == 0)
			{
				// Found one !
				// Now we need to transform this descriptor into a callgate.
				// Note that we're using selector 0x28 since it corresponds
				// to a ring 0 segment which spans the entire linear address
				// space of the processor (0-4GB).
				
				struct CALLGATE_DESCRIPTOR *pCallgate;
				
				pCallgate =	(struct CALLGATE_DESCRIPTOR *) pGDTDescriptor;
				pCallgate->Offset_0_15 = LOWORD(pvRing0FuncAddr);
				pCallgate->Selector = 0x28;
				pCallgate->ParamCount =	0;
				pCallgate->Unused = 0;
				pCallgate->Type = 0xc;
				pCallgate->System = 0;
				pCallgate->DPL = 3;
				pCallgate->Present = 1;
				pCallgate->Offset_16_31 = HIWORD(pvRing0FuncAddr);
				
				// Prepare the far call parameters
				CallgateAddr[0] = 0x0;
				CallgateAddr[1] = 0x0;
				CallgateAddr[2] = (wGDTIndex << 3) | 3;
				
				// Please fasten your seat belts!
				// We're about to make a hyperspace jump into RING 0.
				_asm Mov DX, [wPortAddr]
					_asm Mov EBX, [pdwPortVal]
					_asm Mov CL, [bSize]
					_asm Call FWORD PTR [CallgateAddr]
					
					// We have made it !
					// Now free the GDT descriptor
					memset(pGDTDescriptor, 0, 8);
				
				// Our journey was successful. Seeya.
				return true;
			}
			
			// Advance to the next GDT descriptor
			pGDTDescriptor++;
		}
		
		// Whoops, the GDT is full
		return false;
	}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHddFetcher::CHddFetcher()
{

}

CHddFetcher::~CHddFetcher()
{

}

bool CHddFetcher::ReadPhysicalDriveInNTWithAdminRights(std::string& serialNumber,
													   std::string& modelNumber,
													   std::string& revisionNumber)
{

	bool done = false;
	
	TCHAR szDriveName[256];
	for (int drive = 0; drive < _XXX__::MAX_IDE_DRIVES && !done; ++drive)
	{
		HANDLE hPhysicalDriveIOCTL = NULL;
		
		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		memset(szDriveName, 0, sizeof(szDriveName));
#if _MSC_VER <= 1200
		_sntprintf(szDriveName, 
			sizeof(szDriveName) / sizeof(szDriveName[0]) - 1,
			_T("\\\\.\\PhysicalDrive%d"), drive);
#else
		_sntprintf_s(szDriveName, 
			sizeof(szDriveName) / sizeof(szDriveName[0]) - 1,
			_T("\\\\.\\PhysicalDrive%d"), drive);
#endif
		
		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFile(szDriveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		
		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			GETVERSIONINPARAMS VersionParams;
			DWORD cbBytesReturned = 0;
			
            // Get the version, etc of PhysicalDrive IOCTL
			memset ((void*)&VersionParams, 0, sizeof(VersionParams));
			
			if (!DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL, 
				0,
				&VersionParams,
				sizeof(VersionParams),
				&cbBytesReturned, NULL))
			{         
			}
			
            // If there is a IDE device at number "i" issue commands
            // to the device
			if (VersionParams.bIDEDeviceMap <= 0)
			{
			}
			else
			{
				BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
				SENDCMDINPARAMS  scip;
				
				// Now, get the ID sector for all IDE devices in the system.
				// If the device is ATAPI use the ATAPI_IDENTIFY command,
				// otherwise use the IDE_ATA_IDENTIFY command
				bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? \
						ATAPI_ID_CMD : ID_CMD;
				
				BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
				memset(&scip, 0, sizeof(scip));
				memset(IdOutCmd, 0, sizeof(IdOutCmd));
				
				if (_XXX__::DoIDENTIFY(hPhysicalDriveIOCTL, 
					&scip, 
					(PSENDCMDOUTPARAMS)&IdOutCmd, 
					(BYTE) bIDCmd,
					(BYTE) drive,
					&cbBytesReturned))
				{
					USHORT *pIdSector = (USHORT *)
						((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;
					DWORD dwSize = ((PSENDCMDOUTPARAMS) IdOutCmd) ->cBufferSize;
					
					if (pIdSector && dwSize > 46)
					{
						serialNumber = _XXX__::ConvertToString (pIdSector, 10, 19);
						modelNumber = _XXX__::ConvertToString (pIdSector, 27, 46);
						revisionNumber = _XXX__::ConvertToString (pIdSector, 23, 26);
						
						done = true;
					}
				}
			}
			
			CloseHandle (hPhysicalDriveIOCTL);
		}
	}
	
	return done;
}

bool CHddFetcher::ReadPhysicalDriveInNTUsingSmart(std::string& serialNumber,
												  std::string& modelNumber,
												  std::string& revisionNumber)
{
	bool done = false;
	
	TCHAR szDriveName[256];
	for (int drive = 0; drive < _XXX__::MAX_IDE_DRIVES && !done; ++drive)
	{
		HANDLE hPhysicalDriveIOCTL = 0;
		
		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		memset(szDriveName, 0, sizeof(szDriveName));
#if _MSC_VER <= 1200
		_sntprintf(szDriveName, 
			sizeof(szDriveName) / sizeof(szDriveName[0]) - 1,
			_T("\\\\.\\PhysicalDrive%d"), drive);
#else
		_sntprintf_s(szDriveName, 
			sizeof(szDriveName) / sizeof(szDriveName[0]) - 1,
			_T("\\\\.\\PhysicalDrive%d"), drive);
#endif
		
		//  Windows NT, Windows 2000, Windows Server 2003, Vista
		hPhysicalDriveIOCTL = CreateFile (szDriveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 
			NULL, OPEN_EXISTING, 0, NULL);
		
		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{
		}
		else
		{
			GETVERSIONINPARAMS GetVersionParams;
			DWORD cbBytesReturned = 0;
			
            // Get the version, etc of PhysicalDrive IOCTL
			memset ((void*) & GetVersionParams, 0, sizeof(GetVersionParams));
			
			if ( ! DeviceIoControl (hPhysicalDriveIOCTL, SMART_GET_VERSION,
				NULL, 
				0,
				&GetVersionParams, sizeof (GETVERSIONINPARAMS),
				&cbBytesReturned, NULL) )
			{         
			}
			else
			{
				// Allocate the command buffer
				ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
				PSENDCMDINPARAMS Command =(PSENDCMDINPARAMS) malloc (CommandSize);
				// Retrieve the IDENTIFY data
				// Prepare the command
				if (Command)
				{
					Command -> irDriveRegs.bCommandReg = ID_CMD;
					DWORD BytesReturned = 0;
					if ( ! DeviceIoControl (hPhysicalDriveIOCTL, 
						SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS),
						Command, CommandSize,
						&BytesReturned, NULL) )
					{
					} 
					else
					{
						USHORT *pIdSector = (USHORT *)
							((PSENDCMDOUTPARAMS) Command) -> bBuffer;
						DWORD dwSize = ((PSENDCMDOUTPARAMS) Command) ->cBufferSize;
						if (pIdSector && dwSize > 46)
						{
							serialNumber = _XXX__::ConvertToString (pIdSector, 10, 19);
							modelNumber = _XXX__::ConvertToString (pIdSector, 27, 46);
							revisionNumber = _XXX__::ConvertToString (pIdSector, 23, 26);
							
							done = true;
						}
					}
					// Done
					free(Command);
				}
			}
			CloseHandle (hPhysicalDriveIOCTL);
		}
	}
	
	return done;
}

bool CHddFetcher::ReadPhysicalDriveInNTWithZeroRights(std::string& serialNumber,
													  std::string& modelNumber,
													  std::string& revisionNumber)
{
	bool done = false;
	
	TCHAR szDriveName[256];
	for (int drive = 0; drive < _XXX__::MAX_IDE_DRIVES && !done; ++drive)
	{
		HANDLE hPhysicalDriveIOCTL = 0;
		
		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		memset(szDriveName, 0, sizeof(szDriveName));
#if _MSC_VER <= 1200
		_sntprintf(szDriveName, 
			sizeof(szDriveName) / sizeof(szDriveName[0]) - 1,
			_T("\\\\.\\PhysicalDrive%d"), drive);
#else
		_sntprintf_s(szDriveName, 
			sizeof(szDriveName) / sizeof(szDriveName[0]) - 1,
			_T("\\\\.\\PhysicalDrive%d"), drive);
#endif
		
		//  Windows NT, Windows 2000, Windows XP - admin rights not required
		hPhysicalDriveIOCTL = CreateFile (szDriveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{
		}
		else
		{
			STORAGE_PROPERTY_QUERY query;
			DWORD cbBytesReturned = 0;
			char buffer [2048];
			
			memset ((void *) & query, 0, sizeof (query));
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;
			
			memset (buffer, 0, sizeof (buffer));
			
			if ( DeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
				& query,
				sizeof (query),
				& buffer,
				sizeof (buffer),
				& cbBytesReturned, NULL) )
			{         
				STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;
				if (descrip)
				{
					char productSerialNumber [256] = {0};
					char productModelNumber [256] = {0};
					char productRevision [256] = {0};
					
					_XXX__::flipAndCodeBytes (buffer,
						descrip -> ProductIdOffset,
						0, productModelNumber );
					_XXX__::flipAndCodeBytes (buffer,
						descrip -> ProductRevisionOffset,
						0, productRevision );
					_XXX__::flipAndCodeBytes (buffer,
						descrip -> SerialNumberOffset,
						1, productSerialNumber );
					
					//  serial number must be alphanumeric
					//  (but there can be leading spaces on IBM drives)
					if ((isalnum (productSerialNumber [0]) || isalnum (productSerialNumber [19])))
					{
						serialNumber = std::string(productSerialNumber);
						modelNumber = std::string(productModelNumber);
						revisionNumber = std::string(productRevision);
						done = true;
					}

				}
// 				// Get the disk drive geometry.
// 				memset (buffer, 0, sizeof(buffer));
// 				if ( ! DeviceIoControl (hPhysicalDriveIOCTL,
// 					IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
// 					NULL,
// 					0,
// 					&buffer,
// 					sizeof(buffer),
// 					&cbBytesReturned,
// 					NULL))
// 				{
// 				}
// 				else
// 				{         
// 					DISK_GEOMETRY_EX* geom = (DISK_GEOMETRY_EX*) &buffer;
// 					int fixed = (geom->Geometry.MediaType == FixedMedia);
// 					__int64 size = geom->DiskSize.QuadPart;
// 					
// 				}
			}
			else
			{
				GetLastError ();
			}
			
			CloseHandle (hPhysicalDriveIOCTL);
		}
	}
	
	return done;
}

bool CHddFetcher::ReadDrivePortsInWin9X(std::string& serialNumber,
										std::string& modelNumber,
										std::string& revisionNumber)
{
	bool bDone = false;
	
	//  Get IDE Drive info from the hardware ports
	//  loop thru all possible drives
	for (int drive = 0; drive < _XXX__::MAX_IDE_DRIVES && !bDone; ++drive)
	{
		DWORD diskdata [256];
		WORD  baseAddress = 0;   //  Base address of drive controller
		DWORD portValue = 0;
		int waitLoop = 0;
		int index = 0;
		
		switch (drive / 2)
		{
		case 0: 
			baseAddress = 0x1f0; 
			break;
		case 1: 
			baseAddress = 0x170; 
			break;
		case 2: 
			baseAddress = 0x1e8; 
			break;
		case 3: 
			baseAddress = 0x168; 
			break;
		}
		
		//  Wait for controller not busy
		waitLoop = 100000;
		while (--waitLoop > 0)
		{
			_XXX__::GetPortVal ((WORD) (baseAddress + 7), &portValue, (BYTE) 1);
            //  drive is ready
			if ((portValue & 0x40) == 0x40) 
			{
				break;
			}
			
            //  previous drive command ended in error
			if ((portValue & 0x01) == 0x01)
			{
				break;
			}
		}
		
		if (waitLoop < 1)
		{
			continue;
		}
		
		//  Set Master or Slave drive
		if ((drive % 2) == 0)
		{
			_XXX__::SetPortVal ((WORD) (baseAddress + 6), 0xA0, 1);
		}
		else
		{
			_XXX__::SetPortVal ((WORD) (baseAddress + 6), 0xB0, 1);
		}
		
		//  Get drive info data
		_XXX__::SetPortVal ((WORD) (baseAddress + 7), 0xEC, 1);
		
		// Wait for data ready
		waitLoop = 100000;
		while (--waitLoop > 0)
		{
			_XXX__::GetPortVal ((WORD) (baseAddress + 7), &portValue, 1);
            //  see if the drive is ready and has it's info ready for us
			if ((portValue & 0x48) == 0x48)
			{
				break;
			}

            //  see if there is a drive error
			if ((portValue & 0x01) == 0x01)
			{
				break;
			}
		}
		
		//  check for time out or other error
		if (waitLoop < 1 || portValue & 0x01)
		{
			continue;
		}
		
		//  read drive id information
		for (index = 0; index < 256; index++)
		{
			diskdata [index] = 0;   //  init the space
			_XXX__::GetPortVal (baseAddress, &(diskdata [index]), 2);
		}
		
		serialNumber = _XXX__::ConvertToString ((USHORT *)diskdata, 10, 19);
		modelNumber = _XXX__::ConvertToString ((USHORT *)diskdata, 27, 46);
		revisionNumber = _XXX__::ConvertToString ((USHORT *)diskdata, 23, 26);
		bDone = true;
	}
	
	return bDone;
}

bool CHddFetcher::ReadIdeDriveAsScsiDriveInNT(std::string& serialNumber,
											  std::string& modelNumber,
											  std::string& revisionNumber)
{
	bool done = FALSE;
	TCHAR szDriveName[256];
	for (int controller = 0; controller < 16 && !done; controller++)
	{
		HANDLE hScsiDriveIOCTL = 0;
		
		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		memset(szDriveName, 0, sizeof(szDriveName));
#if _MSC_VER <= 1200
		_sntprintf(szDriveName, 
			sizeof(szDriveName) / sizeof(szDriveName[0]) - 1,
			_T("\\\\.\\Scsi%d:"), controller);
#else
		_sntprintf_s(szDriveName, 
			sizeof(szDriveName) / sizeof(szDriveName[0]) - 1,
			_T("\\\\.\\Scsi%d:"), controller);
#endif
		//  Windows NT, Windows 2000, any rights should do
		hScsiDriveIOCTL = CreateFile (szDriveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);	
		
		if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			for (int drive = 0; drive < 2; drive++)
			{
				char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
				SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
				SENDCMDINPARAMS *pin =
					(SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
				DWORD dummy;
				
				memset (buffer, 0, sizeof (buffer));
				p -> HeaderLength = sizeof (SRB_IO_CONTROL);
				p -> Timeout = 10000;
				p -> Length = SENDIDLENGTH;
				p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
#if _MSC_VER <= 1200
				strncpy ((char *) p -> Signature, "SCSIDISK", 8);
#else
				//strncpy_s ((char *) p -> Signature, sizeof(p -> Signature), "SCSIDISK", 8);
				memcpy((char *) p -> Signature, "SCSIDISK", 8);
#endif
				pin -> irDriveRegs.bCommandReg = ID_CMD;
				pin -> bDriveNumber = drive;
				
				if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
					buffer,
					sizeof (SRB_IO_CONTROL) +
					sizeof (SENDCMDINPARAMS) - 1,
					buffer,
					sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
					&dummy, NULL))
				{
					SENDCMDOUTPARAMS *pOut =
						(SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
					IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
					if (pId -> sModelNumber [0])
					{
						USHORT *pIdSector = (USHORT *) pId;
						
						serialNumber = _XXX__::ConvertToString (pIdSector, 10, 19);
						modelNumber = _XXX__::ConvertToString (pIdSector, 27, 46);
						revisionNumber = _XXX__::ConvertToString (pIdSector, 23, 26);
	
						done = true;
					}
				}
			}
			CloseHandle (hScsiDriveIOCTL);
		}
	}
	
	return done;
}

bool CHddFetcher::ReadPhysicalDriveByRegister(std::string& serialNumber,
											  std::string& modelNumber,
											  std::string& revisionNumber)
{
	bool bReturn = false;
	HKEY hKeyDisks;
	std::string strClass;
	std::string strInstance;
	
	if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
		_T("SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum"), 
		0, KEY_READ, &hKeyDisks))
	{
		unsigned char szValue[1024] = { 0 };
		DWORD dwSize = sizeof(szValue);
		if (ERROR_SUCCESS == RegQueryValueExA(hKeyDisks, "0", NULL, NULL, szValue, &dwSize))
		{
			std::string strValue((char *)szValue);
			std::string::size_type nPos = strValue.find_first_of('\\', 0);
			if (nPos != std::string::npos)
			{
				
			}
		}
		
		RegCloseKey(hKeyDisks);
	}   
	
	return bReturn;
}


bool CHddFetcher::GetPhysicsDriveIdentify(std::string& serialNumber,
										  std::string& modelNumber,
										  std::string& revisionNumber)
{
	bool bDone = false;
	OSVERSIONINFO version;
	memset (&version, 0, sizeof (version));
	version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	//都没有9X了，这段代码也该屏蔽掉了
	//GetVersionEx(&version);
	
	//if (version.dwPlatformId == VER_PLATFORM_WIN32_NT)
	if ( TRUE )
	{
		bDone = ReadPhysicalDriveInNTWithAdminRights(serialNumber, 
			modelNumber, 
			revisionNumber);
		if (!bDone)
		{
			bDone = ReadIdeDriveAsScsiDriveInNT(serialNumber, 
				modelNumber, 
				revisionNumber);
		}

		if (!bDone)
		{
			bDone = ReadPhysicalDriveInNTWithZeroRights(serialNumber, 
				modelNumber, 
				revisionNumber);
		}

		if (!bDone)
		{
			bDone = ReadPhysicalDriveInNTUsingSmart(serialNumber, 
				modelNumber, 
				revisionNumber);
		}
	}
	else
	{
		bDone = ReadDrivePortsInWin9X(serialNumber, 
			modelNumber, 
			revisionNumber);
	}
	return bDone;
}

bool CHddFetcher::GetPhysicsDriveIdentify(std::wstring& serialNumber,
										  std::wstring& modelNumber,
										  std::wstring& revisionNumber)
{
	std::string strSerialNumber;
	std::string strModelNumber;
	std::string strRevisionNumber;
	if (CHddFetcher::GetPhysicsDriveIdentify(strSerialNumber, 
		strModelNumber, 
		strRevisionNumber))
	{
		bool bSn = _XXX__::Encode(strSerialNumber, serialNumber);
		bool bMn = _XXX__::Encode(strModelNumber, modelNumber);
		bool bRn = _XXX__::Encode(strRevisionNumber, revisionNumber);
		return (bSn && bMn && bRn);
	}
	
	return false;
}