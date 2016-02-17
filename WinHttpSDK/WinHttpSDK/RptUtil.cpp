//
//  RptUtil.h
//  aprios
//
//  Created by zhangwusheng on 15/10/29.
//  Copyright © 2015年 adw. All rights reserved.
//


#include <string>
#include <fstream>
#include "RptUtil.h"
#include <Shlobj.h>
#include "Poco/File.h"
#include "Poco/String.h"
#include "Poco/Exception.h"
#include "Poco/UUIDGenerator.h"/**
* 创建上报的基础目录/Documents/aprsdk
*/
bool RptUtil::ensureBaseDirExists(){
	std::string dir = getRptBaseDir();
	DWORD attr = GetFileAttributes(dir.c_str());
	if (attr != INVALID_FILE_ATTRIBUTES)
	{
		if ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			return true;
		}
	}

	if (CreateDirectoryA(dir.c_str(), 0) == 0)
	{
		return false;
	}

	return true;
}
/**
* 获取上报的基础目录/Documents/aprsdk
*/
std::string RptUtil::getRptBaseDir(){
	char  buffer[MAX_PATH] = { 0 };
	if (SUCCEEDED(SHGetFolderPath(NULL,
		CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
		NULL,
		0,
		buffer)))
	{
		return std::string(buffer)+"\\aprsdk";
	}


	return ".\\aprsdk";
}
/**
* 获取代表本机的GUID，第一次生成guid写入文件，重新安装会重新生成文件
*/
std::string RptUtil::getGuidString()
{
	std::string myguid;
	std::string dir = getRptBaseDir();
	dir.append("\\aprsdk.guid");
	DWORD attr = GetFileAttributes(dir.c_str());
	if (attr != INVALID_FILE_ATTRIBUTES)
	{
		std::ifstream ifs(dir.c_str());
		char line[100] = { 0 };
		ifs.getline(line,sizeof(line));
		myguid =  std::string(line);
	}
	else
	{
		Poco::UUIDGenerator &gen = Poco::UUIDGenerator::defaultGenerator();
		Poco::UUID uuid = gen.createOne();
		std::string tid = uuid.toString();
		tid = Poco::remove(tid, '-');

		std::ofstream ofs(dir.c_str());
		ofs << tid<<std::endl;
		myguid = tid;
	}
	return myguid;
}
/**
* 程序退出时把失败队列的数据刷到此文件
*/
std::string RptUtil::getFailQueueFileName()
{
	std::string fname = getRptBaseDir();
	fname.append("\\fqueue.dat");
	return fname;
}
/**
* 程序退出时把正常队列的数据刷到此文件
*/
std::string RptUtil::getSendQueueFileName()
{
	std::string fname = getRptBaseDir();
	fname.append("\\squeue.dat");
	return fname;
}
/**
* 加载本地策略字符串
*/
bool RptUtil::getLocalStrategyString(std::string& startegy)
{
	std::string fname = getRptBaseDir();
	fname.append("\\Strategy.txt");
	DWORD attr = GetFileAttributes(fname.c_str());
	if (attr != INVALID_FILE_ATTRIBUTES)
	{
		std::ifstream ifs(fname.c_str());
		char line[1000] = { 0 };
		ifs.getline(line, sizeof(line));
		startegy = std::string(line);
		return true;
	}
	return false;
}
/**
* 把策略内容写入本地目录
*/
bool RptUtil::writeStrategyStringToLocal(const std::string& strategy)
{
	std::string fname = getRptBaseDir();
	fname.append("\\Strategy.txt");
	std::ofstream ofs(fname.c_str());

	if (ofs){
		ofs << strategy << std::endl;
		return true;
	}
	else
		return false;

}



