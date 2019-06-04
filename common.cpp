/**
 * 
 * Copyright (c) 2019 南京航空航天大学 航空通信网络研究室
 * 
 * @file     d:\code\matlab\Underwater Aloha\common.cpp
 * @author   詹长建 (2233930937@qq.com)
 * @date     2019-05
 * @brief    
 * @version  0.0.1
 * 
 * Last Modified:  2019-06-03
 * Modified By:    詹长建 (2233930937@qq.com)
 * 
 */
#include "common.h"
#include "channel.h"
#include <iostream>
#include <algorithm>
#include <stdlib.h>

uint32_t ulrand(void) {
    return (
         (((uint32_t)rand()<<24)&0xFF000000)
        |(((uint32_t)rand()<<12)&0x00FFF000)
        |(((uint32_t)rand()    )&0x00000FFF)
        );
}
bool AscendingOrder(const struct Packet &a ,const struct Packet &b){
    return a.rx_start < b.rx_start;
}
uint32_t PushPacket(const struct Packet &pac){
    rxpacketVector[pac.to].push_back(pac);
    std::sort(rxpacketVector[pac.to].begin(),rxpacketVector[pac.to].end(),AscendingOrder);
    // #ifdef MY_DEBUG
    //     for(int j=0 ; j < rxpacketVector[pac.to].size() ; j++){
    //         std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
    //         <<"from="<<rxpacketVector[pac.to][j].from<<"\t"
    //         <<"to="<<rxpacketVector[pac.to][j].to<<"\t"
    //         <<"rx_start="<<rxpacketVector[pac.to][j].rx_start<<"\t"
    //         <<"rx_end="<<rxpacketVector[pac.to][j].rx_end<<std::endl;
    //     }
    // #endif
    return EXIT_SUCCESS;
}
std::string GetPacketState(const struct Packet &pac){
    std::string state;
    switch (pac.state)
    {
    case Default:
        state=" ";
        break;
    case Success:
        state="Success";
        break;
    case Fail:
        state="Fail";
        break;
    default:
        break;
    }
    return state;
};
std::string GetPacketType(const struct  Packet &pac){
    std::string type;
    switch (pac.type)
    {
    case RTS:
        type="RTS";
        break;
    case CTS:
        type="CTS";
        break;
    case DATA:
        type="DATA";
        break;
    case ACK:
        type="ACK";
    default:
        break;
    }
    return type;
};

std::default_random_engine e;          ///< 生成种子引擎

std::vector<struct NodeLocation>                 nodes;            // 每个节点的位置
std::vector<std::vector<struct NeighborsNode>>   neighbor_node;    // 邻居节点之间的距离
std::vector<std::vector<struct Packet>>          rxpacketVector;   // 每个节点接收的数据包
std::vector<std::vector<struct Packet>>          txpacketVector;   // 每个节点发送的数据包
std::vector<std::vector<struct Packet>>          cachepacketVector;// 每个节点等待发送的数据包


uint32_t  node_number;      // 仿真的节点个数
double    simulation_time;  // 仿真时间
double    time_unit;        // 仿真运行基本时间单元(以us为单位)

uint32_t  total_packets;     // 总的发包个数
uint32_t  packets;           // 成功的个数
uint32_t  drop_packets;      // 丢包的个数
double    throughput;        // 吞吐量
double    packet_loss_rate;  // 丢包率
double    energy_consumption;// 总能耗
double    average_energy_consumption; // 能耗
double    average_delay;     // 平均时延

