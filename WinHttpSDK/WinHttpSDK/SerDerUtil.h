//
//  SerDerUtil.h
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/20.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#ifndef iosstatsdk_PBUtil_h
#define iosstatsdk_PBUtil_h

#include "GlobalConfig.h"
#include <string>
#include <sstream>
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"
#include "Poco/NumberFormatter.h"
#include "Poco/Exception.h"
#include "Poco/URI.h"

namespace SerDerUtil
{
    inline void decode(const std::string& str, std::string& decodedStr, bool plusAsSpace);
    inline void encode(const std::string& str, const std::string& reserved, std::string& encodedStr);
    
    inline std::string urlencode(const std::string& src){
        std::string dest;
        encode(src, "", dest);
        return dest;
    }
    
    inline std::string toBase64(const std::string& src){
		std::string base64str;
        std::ostringstream str;

        Poco::Base64Encoder encoder(str);
        std::istringstream istr(src);
        copy(std::istreambuf_iterator<char>(istr),
             std::istreambuf_iterator<char>(),
             std::ostreambuf_iterator<char>(encoder));
        encoder.close();

		base64str = str.str();
		std::string encoded;
		Poco::URI::encode(base64str, "", encoded);
		return encoded;
        return str.str() ;

    }
    
    inline std::string fromBase64(const std::string& src){
        std::istringstream istr(src);
        Poco::Base64Decoder decoder(istr);
        std::string decodedResponse;
        Poco::Base64Decoder b64in(istr);
        std::ostringstream ostr;
        std::copy(std::istreambuf_iterator<char>(b64in),
                  std::istreambuf_iterator<char>(),
                  std::ostreambuf_iterator<char>(ostr));
        decodedResponse = ostr.str();
        
        return decodedResponse;
    }
    
    inline std::string urldecode(const std::string& src){
        std::string dest;
        decode(src, dest, false);
        return dest;
    }
    
    const std::string RESERVED_PATH     = "?#";
    const std::string RESERVED_QUERY    = "#";
    const std::string RESERVED_FRAGMENT = "";
    const std::string ILLEGAL           = "%<>{}|\\\"^`";
    
    
    inline void encode(const std::string& str, const std::string& reserved, std::string& encodedStr)
    {
        for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
        {
            char c = *it;
            if ((c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                c == '-' || c == '_' ||
                c == '.' || c == '~')
            {
                encodedStr += c;
            }
            else if (c <= 0x20 || c >= 0x7F || ILLEGAL.find(c) != std::string::npos || reserved.find(c) != std::string::npos)
            {
                encodedStr += '%';
                encodedStr += Poco::NumberFormatter::formatHex((unsigned) (unsigned char) c, 2);
            }
            else encodedStr += c;
        }
    }
    
    inline void decode(const std::string& str, std::string& decodedStr, bool plusAsSpace=false)
    {
        bool inQuery = false;
        std::string::const_iterator it  = str.begin();
        std::string::const_iterator end = str.end();
        while (it != end)
        {
            char c = *it++;
            if (c == '?') inQuery = true;
            // spaces may be encoded as plus signs in the query
            if (inQuery && plusAsSpace && c == '+') c = ' ';
            else if (c == '%')
            {
                if (it == end) throw Poco::SyntaxException("URI encoding: no hex digit following percent sign", str);
                char hi = *it++;
                if (it == end) throw Poco::SyntaxException("URI encoding: two hex digits must follow percent sign", str);
                char lo = *it++;
                if (hi >= '0' && hi <= '9')
                    c = hi - '0';
                else if (hi >= 'A' && hi <= 'F')
                    c = hi - 'A' + 10;
                else if (hi >= 'a' && hi <= 'f')
                    c = hi - 'a' + 10;
                else throw Poco::SyntaxException("URI encoding: not a hex digit");
                c *= 16;
                if (lo >= '0' && lo <= '9')
                    c += lo - '0';
                else if (lo >= 'A' && lo <= 'F')
                    c += lo - 'A' + 10;
                else if (lo >= 'a' && lo <= 'f')
                    c += lo - 'a' + 10;
                else throw Poco::SyntaxException("URI encoding: not a hex digit");
            }
            decodedStr += c;
        }
    }
    
    
    //#if DATA_SERDER_CUR == DATA_SERDER_PB
    //    template < class T>
    //    std::string serializeProtobufMessage(const T& msg)
    //    {
    //        std::string objBinStr = msg.SerializeAsString();
    //        std::string base64str;
    //
    //        std::ostringstream str;
    //        Poco::Base64Encoder encoder(str);
    //        std::istringstream istr(objBinStr);
    //        copy(std::istreambuf_iterator<char>(istr),
    //             std::istreambuf_iterator<char>(),
    //             std::ostreambuf_iterator<char>(encoder));
    //        encoder.close();
    //        base64str = str.str() ;
    //
    //        return base64str;
    //        std::string encoded;
    //        encode(base64str,"",encoded);
    //        return encoded;
    //    }
    //
    //
    //    template < class T>
    //    bool deserializeProtobufMessage(const std::string& base64,T& msg)
    //    {
    //        std::istringstream istr(base64);
    //        Poco::Base64Decoder decoder(istr);
    //        std::string decodedResponse;
    //        Poco::Base64Decoder b64in(istr);
    //        std::ostringstream ostr;
    //        std::copy(std::istreambuf_iterator<char>(b64in),
    //             std::istreambuf_iterator<char>(),
    //             std::ostreambuf_iterator<char>(ostr));
    //        decodedResponse = ostr.str();
    //
    //        return msg.ParseFromString(decodedResponse);
    //    }
    //#endif
}

#endif
