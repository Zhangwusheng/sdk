
// MfcTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MfcTest.h"
#include "MfcTestDlg.h"
#include "afxdialogex.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/UUID.h"
#include "Poco/NumberFormatter.h"
#include "Poco/String.h"
#include <string>
#include <vector>
using namespace std;
using namespace Poco;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx {
    public:
        CAboutDlg();

        // 对话框数据
        enum { IDD = IDD_ABOUTBOX };

    protected:
        virtual void DoDataExchange ( CDataExchange *pDX ); // DDX/DDV 支持

        // 实现
    protected:
        DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx ( CAboutDlg::IDD )
{
}

void CAboutDlg::DoDataExchange ( CDataExchange *pDX )
{
    CDialogEx::DoDataExchange ( pDX );
}

BEGIN_MESSAGE_MAP ( CAboutDlg, CDialogEx )
END_MESSAGE_MAP()


// CMfcTestDlg 对话框

class TestCallback : public DefaultSdkCallback {
    public:
        std::string getAppVer() {
            return "MfcVer1.2";
        }

        std::string getMachineId() {
            UUIDGenerator &gen = UUIDGenerator::defaultGenerator();
            Poco::UUID id = gen.createOne();
            string s = id.toString();
            Poco::removeInPlace ( s, '-' );
            return s;
        }

} gTestCallback;

vector <Poco::UUID> gUUIDs;


class PaidashiTestCallback : public DefaultSdkCallback {
    public:
        PaidashiTestCallback() {
            m_index = 0;
            m_vecIndex = 0;
        }

        std::string getAppVer() {
            return "paidatashiTestVersion";
        }
        std::string getMachineId() {
            string ret;
            m_index = 0;
            for ( int i = 0; i <= m_index; i++ ) {
                //UUIDGenerator &gen = UUIDGenerator::defaultGenerator();
                //Poco::UUID id = gen.createOne();
                Poco::UUID &id = gUUIDs[m_vecIndex + i];
                string s = id.toString();
                Poco::removeInPlace ( s, '-' );
                ret.append ( s );
            }

            m_index++;
            m_vecIndex++;
            if ( m_index >= 3 ) {
                m_index = 0;
            }

            if ( m_vecIndex >= gUUIDs.size() - 4 ) {
                m_vecIndex = 0;
            }

            ::OutputDebugStringA ( ( ret + "\n" ).c_str() );
            // ::MessageBoxA ( NULL, ret.c_str(), "Machineid", MB_OK );
            return ret;
        }

        int m_index;
        int m_vecIndex;
} gPDSTestCallback;

CMfcTestDlg::CMfcTestDlg ( CWnd *pParent /*=NULL*/ )
    : CDialogEx ( CMfcTestDlg::IDD, pParent )
    , m_sdk ( AipaiRptSDK::HttpSDKIntf::getInstance() )
{
    m_hIcon = AfxGetApp()->LoadIcon ( IDR_MAINFRAME );
}

void CMfcTestDlg::DoDataExchange ( CDataExchange *pDX )
{
    CDialogEx::DoDataExchange ( pDX );
}

BEGIN_MESSAGE_MAP ( CMfcTestDlg, CDialogEx )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED ( IDOK, &CMfcTestDlg::OnBnClickedOk )
    ON_BN_CLICKED ( IDOK2, &CMfcTestDlg::OnBnClickedOk2 )
    ON_BN_CLICKED ( IDOK3, &CMfcTestDlg::OnBnClickedOk3 )
    ON_BN_CLICKED ( IDCANCEL, &CMfcTestDlg::OnBnClickedCancel )
    ON_BN_CLICKED ( IDOK4, &CMfcTestDlg::OnBnClickedOk4 )
END_MESSAGE_MAP()


// CMfcTestDlg 消息处理程序

BOOL CMfcTestDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    UUIDGenerator &gen = UUIDGenerator::defaultGenerator();

    for ( int i = 0; i < 30; i++ ) {
        gUUIDs.push_back ( gen.createOne() );
        Sleep ( 10 );
    }

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT ( ( IDM_ABOUTBOX & 0xFFF0 ) == IDM_ABOUTBOX );
    ASSERT ( IDM_ABOUTBOX < 0xF000 );

    CMenu *pSysMenu = GetSystemMenu ( FALSE );
    if ( pSysMenu != NULL ) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString ( IDS_ABOUTBOX );
        ASSERT ( bNameValid );
        if ( !strAboutMenu.IsEmpty() ) {
            pSysMenu->AppendMenu ( MF_SEPARATOR );
            pSysMenu->AppendMenu ( MF_STRING, IDM_ABOUTBOX, strAboutMenu );
        }
    }

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon ( m_hIcon, TRUE );			// 设置大图标
    SetIcon ( m_hIcon, FALSE );		// 设置小图标

    // TODO: 在此添加额外的初始化代码



    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMfcTestDlg::OnSysCommand ( UINT nID, LPARAM lParam )
{
    if ( ( nID & 0xFFF0 ) == IDM_ABOUTBOX ) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else {
        CDialogEx::OnSysCommand ( nID, lParam );
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMfcTestDlg::OnPaint()
{
    if ( IsIconic() ) {
        CPaintDC dc ( this ); // 用于绘制的设备上下文

        SendMessage ( WM_ICONERASEBKGND, reinterpret_cast<WPARAM> ( dc.GetSafeHdc() ), 0 );

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics ( SM_CXICON );
        int cyIcon = GetSystemMetrics ( SM_CYICON );
        CRect rect;
        GetClientRect ( &rect );
        int x = ( rect.Width() - cxIcon + 1 ) / 2;
        int y = ( rect.Height() - cyIcon + 1 ) / 2;

        // 绘制图标
        dc.DrawIcon ( x, y, m_hIcon );
    }
    else {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMfcTestDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR> ( m_hIcon );
}



void CMfcTestDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    m_sdk.quit();
    CDialogEx::OnOK();
}


void CMfcTestDlg::OnBnClickedOk2()
{
    m_sdk.appStartLaunchWithAppKey ( "403e805c0c0a488bba47a4d7b7fd7ad0", "aipai", &gPDSTestCallback );
    // TODO: 在此添加控件通知处理程序代码
    for ( int i = 0; i < 10; i++ ) {
        m_sdk.startupEvent();
        //Sleep ( 1000 );
        //m_sdk.clickEvent ( "OnBnClickedOk2" );
        m_sdk.pubVideoCost ( Poco::NumberFormatter::format ( i * 2 )
                             , Poco::NumberFormatter::format ( i + 9 ) );

        m_sdk.sendKVData ( "testkv", "k1=v1&k2=v2" );
        Sleep ( 100 );
        // m_sdk.mcboxPeak ( "paizi2", "2", 1234, 10, "2015082510" );
        //m_sdk.pubVideoCost ( "12.34", "345.7" );
    }

    for ( int i = 0; i < 63; i++ ) {
        long long id = 10010000000 + i;
        string idstr = Poco::NumberFormatter::format ( id );
        m_sdk.clickEvent ( idstr );

    }


    Sleep ( 10000 );
    m_sdk.quit();

    MessageBox ( "Finished" );
}


void CMfcTestDlg::OnBnClickedOk3()
{
    m_sdk.appStartLaunchWithAppKey ( "01bd059ff9284e3f9abed72ec5bc35e4", "aipai", &gTestCallback );
    // TODO: 在此添加控件通知处理程序代码
    for ( int i = 0; i < 10; i++ ) {
        m_sdk.startupEvent();
        //Sleep ( 1000 );
        //m_sdk.clickEvent ( "OnBnClickedOk2" );
        m_sdk.mcboxLogin ( "paizihao" + Poco::NumberFormatter::format ( i ), Poco::NumberFormatter::format ( i * 3 ) );
        m_sdk.mcboxPeak ( "paizi2", "2", i * i, 10, "2015082510" );
        //m_sdk.pubVideoCost ( "12.34", "345.7" );
        Sleep ( 100 );
    }
    //Sleep ( 10000 );
    m_sdk.quit();
    MessageBox ( "Finished" );
    // TODO: 在此添加控件通知处理程序代码
}


void CMfcTestDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    m_sdk.quit();
    CDialogEx::OnCancel();
}


void CMfcTestDlg::OnBnClickedOk4()
{
    for ( int j = 0; j < 100; j++ ) {
        string strj = Poco::NumberFormatter::format ( j );
        ::OutputDebugStringA ( ( strj + "\n" ).c_str() );
        // TODO: 在此添加控件通知处理程序代码
        m_sdk.appStartLaunchWithAppKey ( "403e805c0c0a488bba47a4d7b7fd7ad0", "aipai", &gPDSTestCallback );

        vector<int> vi;
        vi.push_back ( 7 );
        vi.push_back ( 9 );
        vi.push_back ( 42 );
        vi.push_back ( 43 );
        vi.push_back ( 45 );
        vi.push_back ( 46 );
        vi.push_back ( 48 );
        vi.push_back ( 53 );
        vi.push_back ( 63 );
        vi.push_back ( 29 );



        for ( int i = 0; i < vi.size(); i++ ) {
            long long id = 10010000000 + vi[i];
            string idstr = Poco::NumberFormatter::format ( id );
            m_sdk.clickEvent ( idstr );

        }

        Sleep ( 5000 );
        // DebugO
        m_sdk.quit();
    }

    MessageBox ( "Finished" );
}
