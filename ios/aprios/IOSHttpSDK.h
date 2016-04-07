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
/**
 *应用程序启动上报。用来统计日活
 */
-(void) startupEvent;
/**
 *点击上报。
 */
-(void) clickEvent:(NSString*)evendId;
/**
 *页面时长上报。
 */
-(void) beginPageView:(NSString*)pageName withData:(NSString*)begindata;
-(void) endPageView:(NSString*)pageName withData:(NSString*)leaveData;
/**
 *自定义事件上报。
 */
-(void) beginEvent:(NSString*)eventId withData:(NSString*)begindata;
-(void) endEvent:(NSString*)pageName withData:(NSString*)leaveData;
/**
 *自定义KV数据上报。
 */
-(void) sendKVData:(NSString*)data forAction:(NSString*)action;
-(void) sendDictData:(NSDictionary *)data forAction:(NSString *)action;

/**具体参数含义参见Protocols.h mblogin的字段说明*/
-(void) loginEvent:(NSString*)bid forLogName:(NSString*)logName withType:(NSString*)type andDirection:(NSString*)direction loginStatus:(NSString*)status;

-(void) pubVideoNetSpeed:(NSString*)speed andPubTimeCost:(NSString*)cost;
#pragma mark - 策略加载(SDK内部使用) -
-(void) onHttpStrategyLoaded:(NSString*)repsonse withSuccessFlag:(BOOL)successLoaded;
/**
 * 拍仔逆战有两种模式：工具模式和爱拍模式。专门为此准备。无模式时，startup只报一遍。
 * 有模式时，所有的数据都报两遍，除startup。
 */
-(void) switchAppKey:(NSString*)appkey;

/**
 * 视频时长上报。用来统计APP端视频VV数据和视频总观看时长。
 * 上报逻辑：每隔20秒调用一次 reportVideoPlay，注意：每次开始一次新的视频播放的时候，isNewPlay一定要设置成true，这时候会生成一个新的playid，标识一次单独的播放，后续的时间段的播放都使用这个playid。后续针对这个视频的上报都把这个参数设置成false，这时候会使用以前的playid。这样就可以根据playid去重知道被播放了多少次。根据playdi取最大的duration就知道播放了多少时间。
    
    @param videoId 视频ID
    @param isNew 每次开始一次新的播放时，设置为TRUE，如果是连续播放时上报，则设置为FALSE
    @param headTime 当前视频播放到了多少秒；
    @param duration 视频总时长
    @param source 视频CDN源。0：爱拍，1：虎牙
    @param check
        1：源没Copy，用爱拍源
        2：源已copy，比例在20%内，用爱拍源
        3：源已Copy，比例在80%内，用虎牙源
        4：在第三部分内，但是虎牙源删掉了，回源，用爱拍源
    @type IOS播放器，固定为20
    @endFlag 是否到了视频末尾
 
 */
-(void)reportVideoPlay:(NSString*)videoId
             isNewPlay:(BOOL)isNew
          withHeadTime:(int)headTime
          withDuration:(long)duration
       fromVideoSource:(int)source
        andSourceCheck:(int)check
         forPlayerType:(int)type
            isVideoEnd:(int)endFlag;

/**
 * appStartLaunchWithAppKey初始化的时候提供了withData，SDK会把这些变量保存到自己的变量中。这其实是应用的所有的状态与SDK的交互接口。由于业务状态可能在任何地方变化，因此这些也必须反应到SDK当中，以便在其中应用。所以提供了了这个接口。任何变化都会合并到初始化时保存的那个Dict中。比如，开始时用户没有登录，获取UID会为空。但是登陆后UID就不为空了。此时可以将"userid"=>bid，作为newData，合并到SDK里面去。
 */
-(void)mergeData:(NSDictionary*) newData;
@end