/**
 * Copyright (c) 2014-2015, RongCloud.
 * All rights reserved.
 *
 * All the contents are the copyright of RongCloud Network Technology Co.Ltd.
 * Unless otherwise credited. http://rongcloud.cn
 *
 */

//  RCUtilities.h
//  Created by Heq.Shinoda on 14-5-15.

#ifndef __RCUtilities
#define __RCUtilities

#import <UIKit/UIKit.h>
#import "RCMessage.h"

/*!
 工具类
 */
@interface RCUtilities : NSObject

/*!
 将base64编码的字符串解码并转换为NSData数据

 @param string      base64编码的字符串
 @return            解码后的NSData数据

 @discussion 此方法主要用于iOS6解码base64。
 */
+ (NSData *)dataWithBase64EncodedString:(NSString *)string;

/*!
 将NSData数据转化并编码为base64的字符串

 @param data    未编码的NSData数据
 @return        编码后的base64字符串

 @discussion 此方法主要用于iOS6编码base64。
 */
+ (NSString *)base64EncodedStringFrom:(NSData *)data;

/*!
 scaleImage

 @param image           image
 @param scaleSize       scaleSize

 @return                scaled image
 */
+ (UIImage *)scaleImage:(UIImage *)image toScale:(float)scaleSize;

/*!
 imageByScalingAndCropSize

 @param image           image
 @param targetSize      targetSize

 @return                image
 */
+ (UIImage *)imageByScalingAndCropSize:(UIImage *)image
                            targetSize:(CGSize)targetSize;

/*!
 generate thumbnail from image
 
 @param image           image
 @param targetSize      targetSize
 
 @return                image
 */
+ (UIImage *)generateThumbnail:(UIImage *)image
                    targetSize:(CGSize)targetSize;
/*!
 compressedImageWithMaxDataLength

 @param image               image
 @param maxDataLength       maxDataLength

 @return                    nsdate
 */
+ (NSData *)compressedImageWithMaxDataLength:(UIImage *)image
                               maxDataLength:(CGFloat)maxDataLength;

/*!
 compressedImageAndScalingSize

 @param image           image
 @param targetSize      targetSize
 @param maxDataLen      maxDataLen

 @return                image nsdata
 */
+ (NSData *)compressedImageAndScalingSize:(UIImage *)image
                               targetSize:(CGSize)targetSize
                               maxDataLen:(CGFloat)maxDataLen;

/*!
 compressedImageAndScalingSize

 @param image           image
 @param targetSize      targetSize
 @param percent         percent

 @return                image nsdata
 */
+ (NSData *)compressedImageAndScalingSize:(UIImage *)image
                               targetSize:(CGSize)targetSize
                                  percent:(CGFloat)percent;
/*!
 compressedImage

 @param image           image
 @param percent         percent

 @return                image nsdata
 */
+ (NSData *)compressedImage:(UIImage *)image percent:(CGFloat)percent;

/*!
 获取文字显示的尺寸
 
 @param text 文字
 @param font 字体
 @param constrainedSize 文字显示的容器大小
 
 @return 文字显示的尺寸
 
 @discussion 该方法在计算iOS 7以下系统显示的时候默认使用NSLineBreakByTruncatingTail模式。
 */
+ (CGSize)getTextDrawingSize:(NSString *)text font:(UIFont *)font constrainedSize:(CGSize)constrainedSize;

/*!
 判断是否是本地路径
 
 @param path 路径
 
 @return 是否是本地路径
 */
+ (BOOL)isLocalPath:(NSString *)path;

/*!
 判断是否是网络地址
 
 @param url 地址
 
 @return 是否是网络地址
 */
+ (BOOL)isRemoteUrl:(NSString *)url;

/*!
 获取沙盒修正后的文件路径
 
 @param localPath 本地路径
 
 @return 修正后的文件路径
 */
+ (NSString *)getCorrectedFilePath:(NSString *)localPath;

/*!
 * 获取文件存储路径
 */
+ (NSString *)getFileStoragePath;

/*!
 excludeBackupKeyForURL

 @param storageURL      storageURL

 @return                BOOL
 */
+ (BOOL)excludeBackupKeyForURL:(NSURL *)storageURL;

/*!
 获取App的文件存放路径

 @return    App的文件存放路径
 */
+ (NSString *)applicationDocumentsDirectory;

/*!
 获取融云SDK的文件存放路径

 @return    融云SDK的文件存放路径
 */
+ (NSString *)rongDocumentsDirectory;

/*!
 获取融云SDK的缓存路径

 @return    融云SDK的缓存路径
 */
+ (NSString *)rongImageCacheDirectory;

/*!
 获取当前系统时间

 @return    当前系统时间
 */
+ (NSString *)currentSystemTime;

/*!
 获取当前运营商名称

 @return    当前运营商名称
 */
+ (NSString *)currentCarrier;

/*!
 获取当前网络类型

 @return    当前网络类型
 */
+ (NSString *)currentNetWork;

/*!
 获取当前网络类型
 
 @return    当前网络类型
 */
+ (NSString *)currentNetworkType;

/*!
 获取系统版本

 @return    系统版本
 */
+ (NSString *)currentSystemVersion;

/*!
 获取设备型号

 @return    设备型号
 */
+ (NSString *)currentDeviceModel;

/*!
 获取非换行的字符串
 
 @param originalString 原始的字符串
 
 @return 非换行的字符串
 
 @discussion 所有换行符将被替换成单个空格
 */
+ (NSString *)getNowrapString:(NSString *)originalString;

/**
 获取消息类型对应的描述

 @param mediaType 消息类型
 @return 描述
 */
+ (NSString *)getMediaTypeString:(RCMediaType)mediaType;

/**
 获取消息内容对应的媒体类型

 @param content 消息内容
 @return 媒体类型，如果是不支持的媒体类型或者消息，将返回-1
 */
+ (RCMediaType)getMediaType:(RCMessageContent *)content;

@end
#endif
