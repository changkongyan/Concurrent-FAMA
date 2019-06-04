
/**
 * 
 * Copyright (c) 2019 南京航空航天大学 航空通信网络研究室
 * 
 * @file     d:\code\matlab\SimWiFi\DCF\CA\node.h
 * @author   詹长建 (2233930937@qq.com)
 * @date     2019-05
 * @brief    
 * @version  0.0.1
 * 
 * Last Modified:  2019-06-03
 * Modified By:    詹长建 (2233930937@qq.com)
 * 
 */
#ifndef  NODE_H
#define  NODE_H

#include "common.h"
#include "channel.h"
#include "mac.h"
#include <stdint.h>
#include <string>


// 定义水声节点属性

/**
 * @brief 定时器状态
 * 
 */
enum TimerState{
    Halt,    ///< 挂起定时器
    Continue ///< 继续退避
};
/**
 * @brief 定时器开关
 * 
 */
enum Timer{
    Off,  ///< 关闭定时器
    On    ///< 开启定时器
};
/**
 * @brief 节点所处状态
 * 
 */
enum NodeState{
    IdleState,      ///< 空闲状态
    SendState,      ///< 发送状态 
    ReceiveState,   ///< 接收状态
    BackOffState,   ///< 退避状态
    CollsionState,  ///< 冲突状态
    AckTimeout      ///< 应答超时状态
};


/**
 * @brief 采用面向对象思想，定一个节点的所有属性和动作，简化仿真设计
 * 
 */
class Node
{
private:
    /* data */
public:
    uint32_t   address_;          ///< 节点地址
    
    double     current_time_;     ///< 节点运行时钟
	double     alarm_time_;       ///< 定时器时钟

    uint32_t   cw_;               ///< 当前退避窗口
    uint32_t   cw_counter_;       ///< 当前退避计数器
    uint32_t   packet_counter_;   ///< 成功发送包的个数
    uint32_t   drop_counter_;     ///< 抛弃的包个数
    uint32_t   send_counter_;     ///< 发送的包个数

    uint32_t   num;               ///< 当前正发送第n个包
    double     duration_;         ///< 发包持续时间
    struct Packet tx_packet;      ///< 当前发送的数据包 
    double     send_energy_;      ///< 发送的能量
    double     receive_energy_;   ///< 接收的能量


    uint32_t   work_state_;      ///< 节点当前所处状态
    uint32_t   next_work_state_; ///< 节点下一个工作状态
    uint32_t   timer_;           ///< 定时器开关状态
    uint32_t   timer_state_;     ///< 定时器状态
public:
    /**
     * @brief Construct a new Node object
     * 
     * @param id 节点地址
     */
    Node(uint32_t id);
    Node();
    virtual ~Node();


    /**
     * @brief 返回节点状态描述
     * 
     * @param state  节点工作状态
     * @return std::string 节点状态描述
     */
    static std::string GetWorkState(const uint32_t &state);
    /**
     * @brief 初始化发送的数据包
     * 
     */
    void InitData(double arrive_time);
    /**
     * @brief  发送数据包
     * 
     */
    void SendData(double start_time);
    /**
     * @brief  rts请求
     * 
     */
    void SendRts(double start_time);
    /**
     * @brief 仿真过程当前时刻的状态计算
     * 
     * @param clock 当前时刻
     */
	void Run(double clock);
    /**
     * @brief 定时器超时处理函数
     * 
     */
	void Timer();
    /**
     * @brief  开启定时器
     * 
     * @param time  定时器的超时时刻
     * @param state 超时后的节点工作状态
     */
	void SetAlarm(double time,uint32_t state);
    /**
     * @brief 开启定时器
     * 
     * @param time 定时器的超时时刻
     */
    void SetAlarm(double time);
};

#endif //NODE_H
