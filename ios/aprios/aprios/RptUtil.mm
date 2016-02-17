//
//  RptUtil.mm
//  aprios
//
//  Created by zhangwusheng on 15/10/29.
//  Copyright © 2015年 adw. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FCUUID.h"
#include "RptUtil.h"
#include <fstream>

@interface NSRptUtil : NSObject
+(NSString*) getRptBaseDir;
@end

@implementation NSRptUtil

+(NSString*) getRptBaseDir{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSUserDomainMask, YES);
    NSString* documentsDir = [ paths objectAtIndex:0];
    
    NSString* aprDirName = [documentsDir stringByAppendingPathComponent:@"aprsdk"];
    
    return aprDirName;
}
@end

bool RptUtil::ensureBaseDirExists(){
    NSString* aprDirName = [NSRptUtil getRptBaseDir];
//    NSLog(@"document dir is :%@",aprDirName);
    
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    BOOL isDir = FALSE;
    BOOL dirExists = [fileManager  fileExistsAtPath:aprDirName isDirectory:&isDir];
    
    if(!(dirExists && isDir) ){
        BOOL created = [fileManager createDirectoryAtPath:aprDirName withIntermediateDirectories:YES attributes:nil error:nil];
        if(!created){
//            NSLog(@"create %@ failed...",aprDirName);
            return false;
        }
    }
    return true;
}

std::string RptUtil::getRptBaseDir(){
    return std::string([[NSRptUtil getRptBaseDir] UTF8String]);
}

std::string RptUtil::getFailQueueFileName(){
    NSString* baseDir = [NSRptUtil getRptBaseDir];
    NSString* fileName = [baseDir stringByAppendingPathComponent:@"fqueue.dat"];
    return std::string([ fileName UTF8String]);
}

std::string RptUtil::getSendQueueFileName(){
    NSString* baseDir = [NSRptUtil getRptBaseDir];
    NSString* fileName = [baseDir stringByAppendingPathComponent:@"squeue.dat"];
    return std::string([ fileName UTF8String]);
}

std::string RptUtil::getGuidString()
{
    std::string myguid;
    
    NSString* aprDirName = [NSRptUtil getRptBaseDir];
    
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* fileName = [aprDirName stringByAppendingPathComponent:@"aprsdk.guid"];
    
    const char* pFileName = [fileName UTF8String];
    BOOL fileExists = FALSE;
    
    fileExists = [ fileManager fileExistsAtPath:fileName];
    if( fileExists){
        std::ifstream ifs(pFileName);
        char line[100]={0};
        ifs.getline(line, sizeof(line));
        myguid = line;
        
    }
    else{
        myguid = [[ FCUUID uuid] UTF8String];
        std::ofstream ofs(pFileName);
        if(ofs){
            ofs<<myguid<<std::endl;
        }
    }
    
    return myguid;
}

bool RptUtil::getLocalStrategyString(std::string& startegy){
    NSString* aprDirName = [NSRptUtil getRptBaseDir];
    
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* fileName = [aprDirName stringByAppendingPathComponent:@"Strategy.txt"];
    const char* pFileName = [fileName UTF8String];
    BOOL fileExists = FALSE;
    
    fileExists = [ fileManager fileExistsAtPath:fileName];
    if( fileExists){
        std::ifstream ifs(pFileName);
        char line[1000]={0};
        ifs.getline(line, sizeof(line));
        startegy = line;
        return true;
    }
    else{
        return false;
    }
}

bool RptUtil::writeStrategyStringToLocal(const std::string &strategy){
    
    NSString* aprDirName = [NSRptUtil getRptBaseDir];
    
    NSString* fileName = [aprDirName stringByAppendingPathComponent:@"Strategy.txt"];
    const char* pFileName = [fileName UTF8String];
    
    std::ofstream ofs(pFileName);
    if(ofs){
        ofs<<strategy<<std::endl;
        return true;
    }
    else{
        return false;
    }
}


