// WinHttpTest.cpp : 定义控制台应用程序的入口点。
//
#ifndef WINHTTP_TEST_H
#define WINHTTP_TEST_H

#include "stdafx.h"
#include <windows.h>
#include <Winhttp.h>
#include <string>
#include <iostream>

#include "Poco/Error.h"
#include "Poco/Task.h"
#include "Poco/TaskManager.h"
#include "Poco/Buffer.h"
#include "Poco/BasicEvent.h"
#include "Poco/EventArgs.h"
#include "Poco/Environment.h"
#include "Poco/SharedPtr.h"
#include "Poco/Delegate.h"
#include "Poco/TaskNotification.h"

#include "WinHttpRequest.h"
#include "WinHttpSDK.h"
using namespace Poco;

using namespace std;



class HttpEventArgs : public Poco::EventArgs
{
public:
	string response;
	bool success;
};

class WinHttpRequest 
{
public:
	enum RequestType
	{
		GET = 0,
		POST = 1
	};

	Poco::BasicEvent<HttpEventArgs> RequestFinished;

	WinHttpRequest(int method,const string& url,const string& data="")
	{
		m_method = m_method;
		m_url = url;
		m_data = data;
		m_hSession = NULL;
		m_hConnect = NULL;
		m_hRequest = NULL;
	}

	virtual ~WinHttpRequest()
	{
		if( m_hRequest != NULL )
		{
			WinHttpCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}
		if( m_hConnect != NULL )
		{
			WinHttpCloseHandle(m_hConnect);
			m_hConnect = NULL;
		}
		if( m_hSession != NULL )
		{
			WinHttpCloseHandle(m_hSession);
			m_hSession = NULL;
		}
	}

	void execute();
	string getLastError(){
		return m_error;
	};
private:
	bool executeInternal();
	HINTERNET  m_hSession;
	HINTERNET  m_hConnect;
	HINTERNET  m_hRequest;
	int m_method;
	string m_url;
	string m_data;
	string m_error;
	string m_response;
};

class StrategyHttpRequest:public WinHttpRequest
{
public:
	StrategyHttpRequest():
	  WinHttpRequest(WinHttpRequest::GET,"http://trans.mlog.aipai.com"){}
};

class ActionDataHttpRequest:public WinHttpRequest
{
public:
	ActionDataHttpRequest(const string& data):
	  WinHttpRequest(WinHttpRequest::POST,"http://mlog.aipai.com/i.gif"){}
};

class WinHttpSDK
{
public:
	void onStrategyLoaded(const void* pSender,HttpEventArgs& args)
	{
		cout << "Http Load "<<(args.success?"Success":"Failed")
			<<",response="<<args.response << endl;
	}

	void initSDK()
	{

	}

private:
	TaskManager m_taskManager;
};

class HttpStrategyTask : public Poco::Task
{
public:
	HttpStrategyTask(WinHttpSDK* sdk) : Poco::Task("HttpStrategyTask")
	{
		m_StrategyHttpRequest = new StrategyHttpRequest;
		m_StrategyHttpRequest->RequestFinished += delegate(sdk,&WinHttpSDK::onStrategyLoaded);
	}

	void runTask()
	{
		m_StrategyHttpRequest->execute();
	}
private:
	Poco::SharedPtr<StrategyHttpRequest> m_StrategyHttpRequest;
};

#endif
