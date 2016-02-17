#include "DefaultSdkCallback.h"
#include "HttpSDKIntf.h"
#include <iostream>

using namespace std;


class DataReportUtils
{
    class McboxSdkCallback :public DefaultSdkCallback
    {
    public:
        McboxSdkCallback(const std::string& strAppVer)
        {
            m_strAppVersion = strAppVer;
        }

        std::string getAppVer()
        {
            return m_strAppVersion;
        }

    private:
        std::string m_strAppVersion;
    };

public:
    DataReportUtils(const std::string& strAppVer, const std::string& strChannel = "aipai")
    {
        m_pMcboxSdkCallBack = new McboxSdkCallback(strAppVer);

        //上报器初始化
        AipaiRptSDK::HttpSDKIntf::getInstance().appStartLaunchWithAppKey(
            "01bd059ff9284e3f9abed72ec5bc35e4", strChannel, m_pMcboxSdkCallBack);
    }



    virtual ~DataReportUtils()
    {
        //关闭上报器
        quitSDK();

        if (m_pMcboxSdkCallBack)
        {
            delete m_pMcboxSdkCallBack;
            m_pMcboxSdkCallBack = NULL;
        }
    }

    void quitSDK()
    {
        AipaiRptSDK::HttpSDKIntf::getInstance().quit();
    }

    /**
    *  @brief 上报kv格式数据
    *  @param action 事件类型项
    *  @param data 数据项
    */
    void ReportKVData(const std::string& action, const std::string& data)
    {
        AipaiRptSDK::HttpSDKIntf::getInstance().sendKVData(action, data);
    }

private:
    McboxSdkCallback* m_pMcboxSdkCallBack;
};
//DataReportUtils *gDataReportUtils;//数据上报辅助类全局对象
#include <Shlobj.h>
#include "RptUtil.h"
#include <map>
using namespace std;

int main()
{
	AipaiRptSDK::HttpSDKIntf & sdk = AipaiRptSDK::HttpSDKIntf::getInstance();
	sdk.appStartLaunchWithAppKey("testkey", "tchannel", NULL);
	//sdk.startupEvent();
	cout << " ---" << endl;
	//sdk.clickEvent("clickid");
	//sdk.mcboxLogin("iiii", "233");
	//sdk.mcboxPeak("1111", "2", 1000, 99, "201501234");
	//sdk.beginLogPageView("ppage", "data1");
	//sdk.endLogPageView("ppage", "leaveda1");

	map<string, string> ms;
	ms["k1"] = "v1";
	ms["k2"] = "v2";
	ms["k3"] = "v3";
	sdk.sendKVData("kvkey", ms);

	::Sleep(100000);
	//sdk.quit();
	//RptUtil::ensureBaseDirExists();

/*	char  buffer[MAX_PATH] = { 0 };
	BOOL result = SHGetSpecialFolderPath(NULL
		, buffer
		, CSIDL_LOCAL_APPDATA
		, false);

	cout << buffer << endl;
    DataReportUtils dataReportUtils("1.1.1.0");

	cout << "\n===================\n";
	if (SUCCEEDED(SHGetFolderPath(NULL,
		CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
		NULL,
		0,
		buffer)))
	{
		cout << buffer << endl;
	}

	::_sleep(10);
    dataReportUtils.quitSDK();

    cout << "quit sdk finisehd...";
*/

    return 0;
}