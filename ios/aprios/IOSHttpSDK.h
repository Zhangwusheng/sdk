//
//  IOSHttpSDK.m
//  iosstatsdk
//
//  Created by zhangwusheng on 15/8/21.
//  Copyright (c) 2015年 zhangwusheng. All rights reserved.
//

#import <Foundation/Foundation.h>

#pragma mark - 使用说明 -
/**
 * 使用方法：
 * 1.在程序启动时，在AppDelegate的application:didFinishLaunchingWithOptions中调用：
    ......
    //字典必须包含以下key：gameid,userid,appver,loaderver,addonver
    NSDictionary* dict = [ NSDictionary dictionaryWithObjectsAndKeys:@"game1",@"gameid",@"userid"
    ,@"userid",@"appver1",@"appver",@"loaderver1",@"loaderver",@"addonver1",@"addonver",nil];

    [[IOSHttpSDK shareStatSDK] appStartLaunchWithAppKey:@"XXXXXX" withChannel:@"XXXXX" withData:mydict];
    [[IOSHttpSDK shareStatSDK] startupEvent];
    ......
    
    return YES;
    }
 * 2.在程序退出时在AppDelegate的applicationWillTerminate中调用
    ......
    [[IOSHttpSDK shareStatSDK] quit];
    ......
 */
@interface IOSHttpSDK : NSObject

#pragma mark - 单例获取 -
+ (instancetype)shareStatSDK;
#pragma mark - 启动和退出 -
-(void) appStartLaunchWithAppKey:(NSString*)appkey
                     withChannel:(NSString*)channel
                        withData:(NSDictionary*)data;
-(void) quit;
#pragma mark - 数据上报接口 -
-(void) startupEvent;

-(void) clickEvent:(NSString*)evendId;

-(void) beginPageView:(NSString*)pageName withData:(NSString*)begindata;
-(void) endPageView:(NSString*)pageName withData:(NSString*)leaveData;

-(void) beginEvent:(NSString*)eventId withData:(NSString*)begindata;
-(void) endEvent:(NSString*)pageName withData:(NSString*)leaveData;

-(void) sendKVData:(NSString*)data forAction:(NSString*)action;
-(void) sendDictData:(NSDictionary *)data forAction:(NSString *)action;
/**具体参数含义参见Protocols.h mblogin的字段说明*/
-(void) loginEvent:(NSString*)bid forLogName:(NSString*)logName withType:(NSString*)type andDirection:(NSString*)direction;

-(void) pubVideoNetSpeed:(NSString*)speed andPubTimeCost:(NSString*)cost;
#pragma mark - 策略加载(SDK内部使用) -
-(void) onHttpStrategyLoaded:(NSString*)repsonse withSuccessFlag:(BOOL)successLoaded;

-(void) switchAppKey:(NSString*)appkey;


@end