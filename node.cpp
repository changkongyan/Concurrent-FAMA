/**
 * 
 * Copyright (c) 2019 南京航空航天大学 航空通信网络研究室
 * 
 * @file     d:\code\matlab\SimWiFi\DCF\CA\node.cpp
 * @author   詹长建 (2233930937@qq.com)
 * @date     2019-05
 * @brief    
 * @version  0.0.1
 * 
 * Last Modified:  2019-06-01
 * Modified By:    詹长建 (2233930937@qq.com)
 * 
 */
#include "node.h"
#include <iostream>
#include <assert.h>
#include <time.h>


Node::Node()
{ 
    
	address_=0;        // 节点地址
	
    current_time_=0;   // 节点运行时钟
	alarm_time_=0;     // 定时器时钟
	cw_ =cw_min;       // 当前退避窗口
    cw_counter_=0;     // 当前退避计数器

	num=0;              // 当前正发送第n个包
	duration_=0;         // 发包持续时间
    packet_counter_=0;  // 成功发送包的个数
    drop_counter_=0;    // 抛弃的包个数
	send_counter_=0;    // 发送的包个数
	
	send_energy_=0;     // 发送的能量
    receive_energy_=0;  // 接收的能量

    work_state_=IdleState;     // 节点当前所处状态
	// channel_state_=Idel;       // 信道处于空闲状态
	next_work_state_=IdleState;//节点下一个工作状态
	timer_=Off;               // 定时器开关状态
	timer_state_=Continue;    // 定时器状态
    time_out_=INT32_MAX;      // ACK超时时间
}
Node::Node(uint32_t id)
{
    address_=id;        // 节点地址
	
    current_time_=0;   // 节点运行时钟
	alarm_time_=0;     // 定时器时钟
	cw_ =cw_min;       // 当前退避窗口
    cw_counter_=0;     // 当前退避计数器


	num=0;              // 当前正发送第n个包
	duration_=0;        // 发包持续时间
    packet_counter_=0;  // 成功发送包的个数
    drop_counter_=0;    // 抛弃的包个数
	send_counter_=0;    // 发送的包个数
	
	send_energy_=0;     // 发送的能量
    receive_energy_=0;  // 接收的能量

    work_state_=IdleState;     // 节点当前所处状态
	// channel_state_=Idel;       // 信道处于空闲状态
	next_work_state_=IdleState;//节点下一个工作状态
	timer_=Off;                // 定时器开关状态
	timer_state_=Continue;     // 定时器状态
    time_out_=INT32_MAX;       // ACK超时时间
}

Node::~Node()
{
}

void Node::InitData(double arrive_time){
	tx_packet.id   =(++num)+address_*10000;
	tx_packet.from =address_;
	tx_packet.to   =node_number;
	#ifdef MY_DEBUG
        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"初始化发送数据，时间为"
					<<current_time_<<" 从节点"<<tx_packet.from<<" 发往节点"<<tx_packet.to<<" 包id为"<<tx_packet.id<<std::endl;
    #endif
	tx_packet.type =DATA;
	tx_packet.state=Default;
	tx_packet.retransfer_number=0;
	tx_packet.transmission=(Mac+Physical+Payload)/transmission_rate;
	tx_packet.arrive=arrive_time;
}
void Node::SendRts(double start_time){
	struct Packet pac;
	pac.id   =tx_packet.id;
	pac.from =address_;
	pac.to   =node_number;
	pac.type =RTS;
	pac.state=Default;
	pac.retransfer_number=pac.retransfer_number;
	pac.transmission=(Rts)/transmission_rate;
	pac.arrive=start_time;
	pac.tx_start=start_time;
	pac.tx_end=pac.tx_start+pac.transmission;
	pac.delay=neighbor_node[pac.from][pac.to].neighbor_distance/propagate_speed;
	pac.rx_start=pac.tx_start+pac.delay;
	pac.rx_end=pac.rx_start+pac.transmission;
	pac.energy=propagate_power*pac.transmission;
	
	txpacketVector[pac.from].push_back(pac);//把ack压入发送数据包集合中
	PushPacket(pac);
	
	send_energy_+=pac.transmission;  // 统计所有发包能量消耗

	#ifdef MY_DEBUG
        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"初始化发送数据，时间为"
					<<start_time<<" 从节点"<<pac.from<<" 发往节点"<<pac.to<<" 包id为"<<pac.id
					<<"rx_start="<<pac.rx_start<<"rx_end="<<pac.rx_end<<"持续时间为"<<pac.transmission<<std::endl;
    #endif
}
void Node::SendData(double start_time){
	packet_counter_++;
	tx_packet.tx_start=start_time;
	tx_packet.tx_end=tx_packet.tx_start+tx_packet.transmission;
	tx_packet.delay=neighbor_node[address_][tx_packet.to].neighbor_distance/propagate_speed;
	tx_packet.rx_start=tx_packet.tx_start+tx_packet.delay;
	tx_packet.rx_end=tx_packet.rx_start+tx_packet.transmission;
	tx_packet.energy=propagate_power*tx_packet.transmission;
	txpacketVector[address_].push_back(tx_packet); //记录发送的数据包

	send_energy_+=tx_packet.transmission;          // 统计所有发包能量消耗
}

void Node::SetAlarm(double time ,uint32_t state){
	timer_ = On;            //开启定时器
	alarm_time_  = time;    //超时的时刻
	next_work_state_=state; //超时后的状态
}
void Node::SetAlarm(double time){
	timer_ = On;            //开启定时器
	alarm_time_  = time;    //超时的时刻
}
// void Node::Timer(){
// 	if(work_state_==BackOffState || work_state_==IdleState){
// 		if(channel_state==Busy){
// 			alarm_time_ = slot + current_time_;
// 			// timer_state_ = Halt;
// 		}
// 	}
// 	if( timer_==On && current_time_ >= alarm_time_){
// 		timer_=Off;                   //关闭定时器
// 		work_state_=next_work_state_; //节点的工作状态
// 		if(work_state_==BackOffState){
// 			cw_counter_--;
// 		}
// 	}
// }
void Node::Timer(){
	if(work_state_==BackOffState || work_state_==IdleState){
		if(channel_state==Busy){
			alarm_time_ = slot + current_time_;
		}
	}
	if( timer_==On && current_time_ >= alarm_time_){
		timer_=Off;  //关闭定时器
        if(work_state_==BackOffState && next_work_state_==BackOffState){
			cw_counter_--;
			if(cw_counter_ > 0 ){
				SetAlarm(current_time_+slot,BackOffState);
			}else{
				SetAlarm(current_time_+slot,SendState);
			}				
		}
		if(work_state_==IdleState && next_work_state_==BackOffState){
			srand(e());
			cw_counter_ = ulrand() % cw_;
			if(cw_counter_  > 0 ){
				SetAlarm(current_time_+slot);
			}else
			{
				SetAlarm(current_time_+slot,SendState);
			}
		}
		work_state_=next_work_state_; //节点的工作状态	
		if(work_state_==SendState){
			SendRts(current_time_); // 发送Rts帧
		}
	}
}
void Node::Run(double clock){
    current_time_ = clock;
	switch(timer_){
		case Off://定时器关闭
		{
			switch(work_state_){
				case IdleState:	
					// cw_counter_ = ulrand() % cw_;
					InitData(current_time_);
					SetAlarm(current_time_+slot,BackOffState);
					#ifdef MY_DEBUG
        			std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"选取退避值，时间为"
					<<current_time_<<" 退避值为"<<cw_counter_<<" 从节点"<<tx_packet.from<<" 发往节点"<<tx_packet.to<<" 包id为"<<tx_packet.id<<std::endl;
    				#endif
					break;
				case BackOffState:
					// if(cw_counter_ <= 0){
					// 	SendRts(current_time_);
					// 	work_state_ = SendState;
					// }else{
					// 	// if(timer_state_ == Halt){
					// 	// 	SetAlarm(current_time_+slot,BackOffState);
					// 	// 	timer_state_ =Continue;
					// 	// }else if(timer_state_ == Continue){
					// 	cw_counter_--;
					// 	SetAlarm(current_time_+slot,BackOffState);
					// 	// }	
					// }	
					break;
				case SendState:
					// SetAlarm(current_time_+tx_packet.transmission,AckTimeout);
					// SendData
					
					break;
				case ReceiveState:
					break;
				case CollsionState:
					// SetAlarm(current_time_+slot,AckTimeout);
					// TRANS = (PHY + MAC + PAYLOAD + ACK) / RATE + SIFS;
					// set_alarm(TRANS);
					break;
				case AckTimeout:
					break;
			}
			break;
		}
		case On: //定时器开启
		{
			this->Timer();
			break;
		}
		default:
			break;
	}

}