
// MfcTestDlg.h : ͷ�ļ�
//

#pragma once

#include "../WinHttpSDK/HttpSDKIntf.h"
#include "../WinHttpSDK/DefaultSdkCallback.h"

// CMfcTestDlg �Ի���
class CMfcTestDlg : public CDialogEx {
        // ����
    public:
        CMfcTestDlg ( CWnd *pParent = NULL );	// ��׼���캯��

        // �Ի�������
        enum { IDD = IDD_MFCTEST_DIALOG };

    protected:
        virtual void DoDataExchange ( CDataExchange *pDX );	// DDX/DDV ֧��


        // ʵ��
    protected:
        HICON m_hIcon;

        // ���ɵ���Ϣӳ�亯��
        virtual BOOL OnInitDialog();
        afx_msg void OnSysCommand ( UINT nID, LPARAM lParam );
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        DECLARE_MESSAGE_MAP()
    public:
        afx_msg void OnBnClickedOk();
        afx_msg void OnBnClickedOk2();


    private:
        AipaiRptSDK::HttpSDKIntf &m_sdk;
    public:
        afx_msg void OnBnClickedOk3();
        afx_msg void OnBnClickedCancel();
        afx_msg void OnBnClickedOk4();
};
