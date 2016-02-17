#ifndef HttpSdkCallbackIntf_H
#define HttpSdkCallbackIntf_H

#include <string>
#include <map>

class HttpSdkCallbackIntf {
    public:
        virtual ~HttpSdkCallbackIntf() {}
        virtual std::string getLoaderVer() = 0;
        virtual std::string getAddonVer() = 0;
        virtual std::string getAppVer() = 0;
        virtual int getGameId() = 0;
        virtual std::string getUserId() = 0;
        virtual std::string getMachineId() = 0;
};

#endif
