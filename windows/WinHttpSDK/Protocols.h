//
//  mbstartup.h
//  aprios
//
//  Created by zhangwusheng on 15/8/22.
//  Copyright (c) 2015年 adw. All rights reserved.
//

#ifndef __aprios__mbstartup__
#define __aprios__mbstartup__

#include <string>

class mbstartup
{
public:
	std::string userid;
	std::string gameid;

	std::string toString();
};

class mbpageview
{
public:
	std::string userid;
	std::string gameid;
	std::string eventid;
	long entertime;
	std::string enterdata;
	long leavetime;
	std::string leavedata;

	std::string toString();
};

class mbclick
{
public:
	std::string userid;
	std::string gameid;
	std::string clickid;

	std::string toString();
};

class pubvideocost
{
public:
	std::string pubspeed;
	std::string pubtime;

	std::string toString();
};

class mcboxlogin
{
public:
	std::string pzid;
	std::string serverid;
	std::string toString();
};

class mcboxpeak
{
public:
	std::string apid;
	std::string serverid;
	std::string interval;
	std::string peaktime;
	std::string peakvalue;

	std::string toString();
};

class mcbsysinf
{
public:
	std::string Os;
	std::string Cpu;
	std::string Disk;
	std::string Mem;
	std::string Video;
	std::string Monitor;

	std::string toString();
};

class mcbInst
{
public:
	std::string evtType;
	std::string toString();
};

#endif /* defined(__aprios__mbstartup__) */
