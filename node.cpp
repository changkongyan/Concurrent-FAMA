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
 * Last Modified:  2019-06-04
 * Modified By:    詹长建 (2233930937@qq.com)
 * 
 */
#include "node.h"
#include <iostream>
#include <assert.h>
#include <time.h>


Node::Node()
{ 
    
	address_=0;         // 节点地址
	
    current_time_=0;    // 节点运行时钟
	alarm_time_=0;      // 定时器时钟
	cw_ =cw_min;        // 当前退避窗口
    cw_counter_=0;      // 当前退避计数器

	num=0;               // 当前正发送第n个包
	duration_=0;         // 发包持续时间
    packet_counter_=0;   // 成功发送包的个数
    drop_counter_=0;     // 抛弃的包个数
	send_counter_=0;     // 发送的包个数
	
	send_energy_=0;     // 发送的能量
    receive_energy_=0;  // 接收的能量

    work_state_=IdleState;     // 节点当前所处状态
	next_work_state_=IdleState;//节点下一个工作状态
	timer_=Off;               // 定时器开关状态
	timer_state_=Continue;    // 定时器状态
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
	next_work_state_=IdleState;//节点下一个工作状态
	timer_=Off;                // 定时器开关状态
	timer_state_=Continue;     // 定时器状态
}

Node::~Node()
{
}

std::string Node::GetWorkState(const uint32_t &state){
	std::string tmp;
	switch (state)
	{
		case IdleState:
			tmp="空闲状态";
			break;
		case SendState:
			tmp="发送状态";
			break;
		case ReceiveState:
			tmp="接收状态 ";
			break;
		case BackOffState:
			tmp="退避状态";
			break;
		case CollsionState:
			tmp="冲突状态";
			break;
		case AckTimeout:
			tmp="超时状态";
			break;
		default:
			break;
	}
	return tmp;
}
void Node::InitData(double arrive_time){
	if(tx_packet.retransfer_number < reTx_max && tx_packet.state==Fail){
		tx_packet.retransfer_number++;
	}else{
		tx_packet.id   =(++num)+address_*10000;
		tx_packet.from =address_;
		tx_packet.to   =node_number;
		tx_packet.type =DATA;
		tx_packet.state=Default;
		tx_packet.retransfer_number=0;
		tx_packet.transmission=(Mac+Physical+Payload)/transmission_rate;
		tx_packet.arrive=arrive_time;

		#ifdef MY_DEBUG
			std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"节点"<<tx_packet.from<<"在"<<current_time_
			<<"时刻初始化发送数据，发往节点"<<tx_packet.to<<"，且包id为"<<tx_packet.id<<std::endl;
		#endif
	}
}
void Node::SendRts(double start_time){
	struct Packet pac;
	pac.id   =tx_packet.id;
	pac.from =address_;
	pac.to   =node_number;
	pac.type =RTS;
	pac.state=Default;
	pac.retransfer_number=tx_packet.retransfer_number;
	pac.transmission=(Rts)/transmission_rate;
	pac.arrive=start_time;
	pac.tx_start=start_time;
	pac.tx_end=pac.tx_start+pac.transmission;
	pac.delay=neighbor_node[pac.from][pac.to].neighbor_distance/propagate_speed;
	pac.rx_start=pac.tx_start+pac.delay;
	pac.rx_end=pac.rx_start+pac.transmission;
	pac.energy=propagate_power*pac.transmission;
	
	txpacketVector[pac.from].push_back(pac);//把发送的ack压入发送数据包集合中
	PushPacket(pac);                        //压入接收节点的接收集合中
	
	send_energy_+=pac.transmission;         // 统计所有发包能量消耗,这里统计的是发送的时间

	#ifdef MY_DEBUG
        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"节点"<<pac.from<<"在"<<start_time
		<<"时刻发送RTS，发往节点"<<pac.to<<"且包id为"<<pac.id
		<<"，节点"<<pac.to<<"开始接收时刻为"<<pac.rx_start<<"接收完成时刻为"<<pac.rx_end<<"其持续时间为"<<pac.transmission
		<<"ms"<<std::endl;
    #endif
}
void Node::SendData(double start_time){
	packet_counter_++;                             // 记录成功发送包的个数
	tx_packet.tx_start=start_time;
	tx_packet.tx_end=tx_packet.tx_start+tx_packet.transmission;
	tx_packet.delay=neighbor_node[address_][tx_packet.to].neighbor_distance/propagate_speed;
	tx_packet.rx_start=tx_packet.tx_start+tx_packet.delay;
	tx_packet.rx_end=tx_packet.rx_start+tx_packet.transmission;
	tx_packet.energy=propagate_power*tx_packet.transmission;
	txpacketVector[address_].push_back(tx_packet); // 把发送的数据压入发送数据包集合中

	send_energy_+=tx_packet.transmission;          // 统计所有发包能量消耗,这里统计的是发送的时间
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
				// SetAlarm(current_time_+slot,SendState);
				next_work_state_=SendState;
			}				
		}
		if(work_state_==IdleState && next_work_state_==BackOffState){
			// srand(e()+time(NULL));
			// cw_counter_ = ulrand() % cw_;
			cw_counter_ = rand() % cw_;
			if(cw_counter_  > 0 ){
				SetAlarm(current_time_+slot);
			}else
			{
				// SetAlarm(current_time_+slot,SendState);
				next_work_state_=SendState;
			}
			#ifdef MY_DEBUG
				std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"节点"<<address_
				<<"在"<<current_time_<<"时刻，选取退避值"<<cw_counter_<<std::endl;
			#endif
		}
		if( ( work_state_==CollsionState || work_state_==SendState )  && next_work_state_==IdleState){
			work_state_=next_work_state_;
			InitData(current_time_);
			SetAlarm(current_time_+slot,BackOffState);
			return;
		}
		work_state_=next_work_state_; //节点的工作状态	
		if(work_state_==SendState){
			SendRts(current_time_);   //发送Rts帧
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
					InitData(current_time_);
					SetAlarm(current_time_+slot,BackOffState);
					break;
				case BackOffState:
					break;
				case SendState:
					break;
				case ReceiveState:
					break;
				case CollsionState:
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