#ifndef DefaultSdkCallback_H
#define DefaultSdkCallback_H

#include "HttpSdkCallbackIntf.h"
#include <string>

class DefaultSdkCallback : public HttpSdkCallbackIntf {
    public:
        virtual ~DefaultSdkCallback() {}
        virtual std::string getLoaderVer() {
            return "pcdefault";
        }
        virtual std::string getAddonVer() {
            return "pcdefault";
        }
        virtual std::string getAppVer() {
            return "pcdefault";
        }

        virtual int getGameId() {
            return 0;
        }
        virtual std::string getUserId() {
            return "pcdefault";
        }

        virtual std::string getMachineId();
};

#endif
