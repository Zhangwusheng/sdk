// SeqIDGenerator.h: interface for the CSeqIDGenerator class.
//
//
// 从QQ影音拿过来的代码，做了些修改
//     zhangqing 2009.12.16
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQIDGENERATOR_H__A0A2A8F0_23C1_4B3A_B78F_0BC50564BE5A__INCLUDED_)
#define AFX_SEQIDGENERATOR_H__A0A2A8F0_23C1_4B3A_B78F_0BC50564BE5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER == 1200
#pragma warning( disable : 4251)
#endif

#include <string>

class  CSeqIDGenerator  
{
public:
	enum {GUID_LEN = 16};

	static void GetSequenceID(unsigned char pID[GUID_LEN]);
};

#endif // !defined(AFX_SEQIDGENERATOR_H__A0A2A8F0_23C1_4B3A_B78F_0BC50564BE5A__INCLUDED_)
