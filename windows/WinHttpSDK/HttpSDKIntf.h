#ifndef HttpSDKIntf_H
#define HttpSDKIntf_H

#include "HttpSdkCallbackIntf.h"

#pragma comment(lib, "Winhttp")
#pragma comment(lib, "iphlpapi")
#pragma comment(lib, "Crypt32")

namespace Poco {
    template <class S>
    class SingletonHolder;
}


namespace AipaiRptSDK {

    class HttpSDKIntf {

        public:

            /**
             * Singleton.The only way to get a HttpSDKIntf instance
             */
            static HttpSDKIntf &getInstance();
            /**
             * SDK Initialization。必须在应用程序启动Application初始化那里进行调用.
             * @param appkey APP标识。 向数据平台部申请下发。
             * @param channel 渠道标识。向数据平台部申请下发。
             * @param callback 回调接口。
             */
            void appStartLaunchWithAppKey (
                const std::string &appkey
                , const std::string &channel
                , HttpSdkCallbackIntf *callback );
            /**
             * 程序启动事件。应用程序启动初始化成功后调用，一般用来计算程序的日活
             */
            void startupEvent();
            /**
             * 页面时长统计。beginLogPageView和endLogPageView必须配对调用，否则数据不会上
             * pageName必须全局唯一，否则数据会失效
             */
            void beginLogPageView ( const std::string &pageName, const std::string &enterdata );
            void endLogPageView ( const std::string &pageName, const std::string &leavedata );
            /**
             * 点击事件上报。clickId由数据平台部分配
             */
            void clickEvent ( const std::string &clickId );
            /**
             * 自定义事件上报，eventid由数据平台部分配
             */
            void beginEvent ( const std::string &eventid, const std::string &enterdata );
            void endEvent ( const std::string &eventid, const std::string &leavedata );

            /**
             * 程序退出时调用
             */
            void quit();

            /**
             * PC发布时长和平均网速上报
             */
            void pubVideoCost ( const std::string &avgNetSpeed, const std::string &avgTimeCost );

            /**
             * MC盒子登陆上报。登陆成功后上报
             * @serverid	服务器ID
             *			>0 登陆服务器成功
             *			=0 登陆盒子客户端成功
             * @apid
             *			 拍子号
             */
            void mcboxLogin ( const std::string &apid, const std::string &serverid );
            /**
             * MC盒子上报。每隔interval上报一次。
             * @serverid	服务器ID
             * @apid		拍子号
             * @peakValue	上报时刻的峰值
             * @interval	上报时间间隔，单位为分钟
             * @peakTime	峰值时间，格式为2015082111XX,XX代表分钟，最后两位必须为时间间隔的整数倍
             *
             */
            void mcboxPeak ( const std::string &apid
                             , const  std::string &serverid
                             , long peakValue, int interval
                             , const std::string &peakTime  );

            void sendKVData ( const std::string &action, const std::string &data );
			void sendKVData(const std::string& action, const std::map<std::string, std::string>& dataItems);
        private:
            HttpSDKIntf();
            ~HttpSDKIntf();
            HttpSDKIntf ( const HttpSDKIntf & );
            friend class Poco::SingletonHolder<HttpSDKIntf>;
    };
}
#endif
