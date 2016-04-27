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
#include <vector>

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

/*
 * 用户登录成功后发送数据包
 */
class mblogin
{
public:
    std::string bid;//用户ID
    std::string loginid;//qq号，手机号，cookieid等用来登录的用户名
    //拍子号:pzh,微信:wx,QQ:qq,微博：wb,手机：sj，邮箱：yx
    std::string type;
    std::string direction;   //0:自动登录，1：手动登录，2：登出
    std::string login_status;//登录成功还是失败
    std::string toString();
};


class mbvideoplayduration
{
public:
    std::string videoid;
    std::string playid;
    long headtime;
    long duration;
    int source;
    int sourcecheck;
    int type;
    int endflag;
    std::string uid;
    
    std::string toString();
};

class mberrorreport
{
public:
    int signal;
    std::string name;
    std::string reason;
    std::vector<std::string> items;
    int itemcount;
    std::string toString();

};

class mbheartbeat
{
public:
    std::string sessionid;
    std::string starttime;
    long span;
    std::string toString();
};
#endif /* defined(__aprios__mbstartup__) */
