/**
 * 
 * Copyright (c) 2019 南京航空航天大学 航空通信网络研究室
 * 
 * @file     d:\code\matlab\Underwater Aloha\mac.cpp
 * @author   詹长建 (2233930937@qq.com)
 * @date     2019-05
 * @brief    
 * @version  0.0.1
 * 
 * Last Modified:  2019-06-01
 * Modified By:    詹长建 (2233930937@qq.com)
 * 
 */
#include "mac.h"

double   Physical;  // 物理层头
double   Mac;       // MAC头
double   Payload;   // 数据负载
double   Rts;       // RTS帧
double   Cts;       // CTS帧
double   Ack;       // 应答帧

double   slot;    // 最小时隙单元
double   reTx_max;// 最大重发次数
double   cw_min;  // 最小退避窗口
double   cw_max;  // 最大退避窗口 