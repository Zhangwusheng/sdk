//
//  RptUtil.h
//  aprios
//
//  Created by zhangwusheng on 15/10/29.
//  Copyright © 2015年 adw. All rights reserved.
//
#ifndef __aprios__RptUtil__
#define __aprios__RptUtil__

#include <string>

class RptUtil
{
public:
    /**
     * 创建上报的基础目录/Documents/aprsdk
     */
    static bool ensureBaseDirExists();
    /**
     * 获取上报的基础目录/Documents/aprsdk
     */
    static std::string getRptBaseDir();
    /**
     * 获取代表本机的GUID，第一次生成guid写入文件，重新安装会重新生成文件
     */
    static std::string getGuidString();
    /**
     * 程序退出时把失败队列的数据刷到此文件
     */
    static std::string getFailQueueFileName();
    /**
     * 程序退出时把正常队列的数据刷到此文件
     */
    static std::string getSendQueueFileName();
    /**
     * 加载本地策略字符串
     */
    static bool getLocalStrategyString(std::string& startegy);
    /**
     * 把策略内容写入本地目录
     */
    static bool writeStrategyStringToLocal(const std::string& strategy);
};

#endif
