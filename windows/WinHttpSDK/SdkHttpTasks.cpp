#include "SdkHttpTasks.h"
#include "HttpDataNotification.h"
#include "SdkHttpRequests.h"
#include <iostream>
using namespace Poco;
using namespace std;

void HttpSendQueuDataTask::runTask()
{
    while ( this->state() == Poco::Task::TASK_RUNNING ) {
		//cout << "HttpSendQueuDataTask::runTask" << endl;

        Poco::Notification::Ptr ntf = m_sendQueue.waitDequeueNotification ( 1000 );
        if ( !ntf ) {
            continue;
        }

        HttpDataNotification::Ptr dataPtr = ntf.cast<HttpDataNotification>();
        if ( !dataPtr ) {
            continue;
        }

        string data = dataPtr->toString();
        int success = 0;
        for ( int i = 0; i < m_nTrys; i++ ) {
            ActionDataHttpRequest r ( data );
            if ( r.execute() ) {
                success = 1;
                break;
            }
        }

        if ( !success ) {
            m_failQueue.enqueueNotification ( dataPtr );
        }
    }
}

void HttpFailedQueuDataTask::runTask()
{
    while ( this->state() == Poco::Task::TASK_RUNNING ) {
		//cout << "HttpSendQueuDataTask::runTask" << endl;

        Poco::Notification::Ptr ntf = m_failQueue.waitDequeueNotification ( 1000 );
        if ( ! ntf ) {
            continue;
        }

        HttpDataNotification::Ptr dataPtr = ntf.cast<HttpDataNotification>();
        if ( !dataPtr ) {
            continue;
        }

        if ( dataPtr->getFailedCount() >= 2 ) {
            continue;
        }

        FailedActionDataHttpRequest req ( &m_sdk->rptStrategy(), dataPtr->toString() );
        if ( !req.execute() ) {
            dataPtr->incrementFailCount();
            m_failQueue.enqueueNotification ( dataPtr );
        }
    }
}

