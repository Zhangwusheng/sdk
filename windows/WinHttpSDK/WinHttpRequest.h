// WinHttpTest.cpp : 定义控制台应用程序的入口点。
//
#ifndef WinHttpRequest_H
#define WinHttpRequest_H

#include <windows.h>
#include <Winhttp.h>
#include <string>
#include "GlobalConfig.h"
#include "Poco/BasicEvent.h"
#include "HttpEventArgs.h"
#include "Poco/Task.h"

class WinHttpRequest {
    public:
        enum RequestType {
            GET = 0,
            POST = 1
        };

        Poco::BasicEvent<HttpEventArgs> RequestFinished;

		WinHttpRequest(int method, const std::string &url, const std::string &data = "", Poco::Task* pTask = NULL) {
            m_method = method;
            m_url = url;
            m_data = data;
            m_hSession = NULL;
            m_hConnect = NULL;
            m_hRequest = NULL;
			m_parentTask = pTask;
        }

        virtual ~WinHttpRequest() {
            if ( m_hRequest != NULL ) {
                WinHttpCloseHandle ( m_hRequest );
                m_hRequest = NULL;
            }
            if ( m_hConnect != NULL ) {
                WinHttpCloseHandle ( m_hConnect );
                m_hConnect = NULL;
            }
            if ( m_hSession != NULL ) {
                WinHttpCloseHandle ( m_hSession );
                m_hSession = NULL;
            }
        }

        bool execute();

        std::string getLastError() {
            return m_error;
        };
    protected:
        bool executeInternal();
        HINTERNET  m_hSession;
        HINTERNET  m_hConnect;
        HINTERNET  m_hRequest;
        int m_method;
        std::string m_url;
        std::string m_data;
        std::string m_error;
        std::string m_response;
		Poco::Task* m_parentTask;
};

#endif
