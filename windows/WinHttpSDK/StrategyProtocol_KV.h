//
//  RptStrategy.h
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/20.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#ifndef WINSDK___StrategyProtocol_KV__
#define WINSDK___StrategyProtocol_KV__

#include "StrategyProtocol.h"

class KVStrategyProtocol : public StrategyProtocol
{
public:
    virtual bool deserialize(const std::string& data) ;
    virtual std::string serialize() ;
};

#endif /* defined(__iosstatsdk__StrategyProtocol_KV__) */
