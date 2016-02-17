#include "WinHttpRequest.h"
#include "Poco/Error.h"
#include "Poco/Buffer.h"
#include "Poco/Thread.h"
#include "Poco/UnicodeConverter.h"
using namespace Poco;
using namespace std;

bool WinHttpRequest::execute()
{
    //Poco::Thread::sleep(2000);
    HttpEventArgs args;
    if ( executeInternal() ) {
        args.response = m_response;
        args.success = true;
    }
    else {
        args.response = m_error;
        args.success = false;
    }

    RequestFinished ( args );
    return args.success;
}

bool WinHttpRequest::executeInternal()
{
	//cout << "WinHttpRequest::executeInternal " <<Poco::Thread::currentTid()<< endl;
    string::size_type domain_idx1 = m_url.find ( "//" );
    if ( domain_idx1 == string::npos ) {
        m_error = "Invalid Url:// Not Found";
        return false;
    }
    string::size_type domain_idx =  m_url.find ( '/', domain_idx1 + 2 );

    string domain, path;
    if ( domain_idx == string::npos ) {
        domain =  m_url.substr ( domain_idx1 + 2 );
        path = "/";
    }
    else {
        domain = m_url.substr ( domain_idx1 + 2, domain_idx - domain_idx1 - 2 );
        path = m_url.substr ( domain_idx );
    }

#if APP_DEBUG_MODE
	{
		cout << m_data << endl;
		return true;
	}
#endif

    m_response = "";

    m_hSession = WinHttpOpen ( L"WinAPASDK/1.0",
                               WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                               WINHTTP_NO_PROXY_NAME,
                               WINHTTP_NO_PROXY_BYPASS, 0 );

	if (!m_hSession  ) {
        m_error = Poco::Error::getMessage ( GetLastError ( ) );
        return false;
    }

	if (m_parentTask && (m_parentTask->state() == Poco::Task::TASK_CANCELLING)){
		return false;
	}

	if (!WinHttpSetTimeouts(m_hSession, 5000, 5000, 5000, 5000)) {
        m_error = Poco::Error::getMessage ( GetLastError ( ) );
        return false;
    }

    Poco::UTF16String strDomainUnicode, strPathUnicode ;
    Poco::UnicodeConverter::convert ( domain, strDomainUnicode );
    Poco::UnicodeConverter::convert ( path, strPathUnicode );

    m_hConnect = WinHttpConnect ( m_hSession, strDomainUnicode.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0 );

    if ( !m_hConnect ) {
        m_error = Poco::Error::getMessage ( GetLastError ( ) );
        return false;
    }
	if (m_parentTask && (m_parentTask->state() == Poco::Task::TASK_CANCELLING)){
		return false;
	}

    LPCWSTR method = L"GET";
    if ( m_method == POST ) {
        method = L"POST";
    }

    m_hRequest = WinHttpOpenRequest ( m_hConnect, method, strPathUnicode.c_str(),
                                      NULL, WINHTTP_NO_REFERER,
                                      WINHTTP_DEFAULT_ACCEPT_TYPES,
                                      WINHTTP_FLAG_REFRESH );
    if ( !m_hRequest ) {
        m_error = Poco::Error::getMessage ( GetLastError ( ) );
        return false;
    }
	if (m_parentTask && (m_parentTask->state() == Poco::Task::TASK_CANCELLING)){
		return false;
	}

    const char *pData = m_data.c_str();
    DWORD dwLength = m_data.length();

    BOOL	bResults = WinHttpSendRequest ( m_hRequest,
                                            WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                            ( LPVOID ) pData, dwLength,
                                            dwLength, 0 );

    if ( !bResults ) {
        m_error = Poco::Error::getMessage ( GetLastError ( ) );
        return false;
    }
	if (m_parentTask && (m_parentTask->state() == Poco::Task::TASK_CANCELLING)){
		return false;
	}

    bResults = WinHttpReceiveResponse ( m_hRequest, NULL );
    if ( !bResults ) {
        m_error = Poco::Error::getMessage ( GetLastError ( ) );
        return false;
    }
	if (m_parentTask && (m_parentTask->state() == Poco::Task::TASK_CANCELLING)){
		return false;
	}

    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    do {
        // Check for available data.
        dwSize = 0;
        if ( !WinHttpQueryDataAvailable ( m_hRequest, &dwSize ) ) {
            m_error = Poco::Error::getMessage ( GetLastError ( ) );
            return false;
        }

        if ( dwSize == 0 ) {
            break;
        }
		
		if (m_parentTask && (m_parentTask->state() == Poco::Task::TASK_CANCELLING)){
			return false;
		}

        // Allocate space for the buffer.
        Poco::Buffer<char> buffer ( dwSize + 1 );
        char *p = buffer.begin();
        DWORD dwToRead = dwSize;
        // Read the data.
        ZeroMemory ( buffer.begin(), buffer.capacityBytes() );

        while ( dwToRead > 0 ) {
            if ( !WinHttpReadData ( m_hRequest, ( LPVOID ) p, dwToRead, &dwDownloaded ) ) {
                m_error = Poco::Error::getMessage ( GetLastError ( ) );
                return false;
            }
			if (m_parentTask && (m_parentTask->state() == Poco::Task::TASK_CANCELLING)){
				return false;
			}
            dwToRead -= dwDownloaded;
            p += dwDownloaded;
        }

        m_response.append ( buffer.begin() );
    }
    while ( dwSize > 0 );

	//cout << "WinHttpRequest::executeInternal finished" << endl;
    return true;
}
