// WinHttpTest.cpp : 定义控制台应用程序的入口点。
//
#ifndef HttpEventArgs_H
#define HttpEventArgs_H

#include <string>
#include "Poco/EventArgs.h"

class HttpEventArgs : public Poco::EventArgs {
    public:
        std::string response;
        bool success;
};

#endif
