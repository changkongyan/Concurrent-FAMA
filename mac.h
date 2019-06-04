/**
 * 
 * Copyright (c) 2019 南京航空航天大学 航空通信网络研究室
 * 
 * @file     d:\code\matlab\Underwater Aloha\mac.h
 * @author   詹长建 (2233930937@qq.com)
 * @date     2019-05
 * @brief    
 * @version  0.0.1
 * 
 * Last Modified:  2019-06-01
 * Modified By:    詹长建 (2233930937@qq.com)
 * 
 */

#ifndef MAC_H
#define MAC_H

#include <stdint.h>

// 定义水声MAC协议属性

#if 0
/**
 * @brief 数据在物理层的格式
 * 
 */
struct FrameFormat
{
    double  physical; ///< 物理层头
    double  mac;      ///< MAC头
    double  payload;  ///< 数据负载
    double  ack;      ///< 应答帧
    FrameFormat():
        physical(208),
        mac(224),
        payload(1500*8),
        ack(physical+112){
        //todo
    }
};
#else
extern double   Physical;///< 物理层头
extern double   Mac;     ///< MAC头
extern double   Payload; ///< 数据负载
extern double   Rts;     ///< RTS帧
extern double   Cts;     ///< CTS帧
extern double   Ack;     ///< 应答帧
#endif

extern double   slot;    ///< 最小时隙单元
extern double   reTx_max;///< 最大重发次数
extern double   cw_min;  ///< 最小退避窗口
extern double   cw_max;  ///< 最大退避窗口


#endif //MAC_H