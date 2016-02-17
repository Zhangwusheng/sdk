// WinHttpTest.cpp : 定义控制台应用程序的入口点。
//
//
#include "stdafx.h"
#include "WinHttpSDK.h"
#include "Poco/Thread.h"
#include "Poco/Timestamp.h"
#include "WinHttpRequest.h"
#include "RptHdrObj.h"
#include "HttpSDKIntf.h"
#include "Poco/LocalDateTime.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timezone.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"

using namespace Poco;
using namespace std;
using namespace AipaiRptSDK;

int _tmain ( int argc, _TCHAR *argv[] )
{
    HttpSDKIntf &sdk = HttpSDKIntf::getInstance();
    sdk.appStartLaunchWithAppKey ( "pcsdkkey", "aipai", NULL );
    sdk.startupEvent();
    Poco::Thread::sleep ( 2000 );

    for ( int i = 0; i < 1; i++ ) {
        sdk.startupEvent();
    }


    sdk.beginEvent ( "testevent", "testenter" );
    sdk.endEvent ( "testevent", "leave" );

    sdk.beginLogPageView ( "testpage", "testenter" );
    sdk.endLogPageView ( "testpage", "leaveit" );

    sdk.clickEvent ( "clickit" );

    Poco::Thread::sleep ( 5000 );

    sdk.quit();
    return 0;
}
