/**
 * 
 * Copyright (c) 2019 南京航空航天大学 航空通信网络研究室
 * 
 * @file     d:\code\matlab\Underwater Aloha\run.cpp
 * @author   詹长建 (2233930937@qq.com)
 * @date     2019-05
 * @brief    
 * @version  0.0.1
 * 
 * Last Modified:  2019-06-04
 * Modified By:    詹长建 (2233930937@qq.com)
 * 
 */

#include "run.h"
#include <time.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <iomanip> //std::setw()
#include <fstream>

int CreateNodes(){
    char s[100];sprintf(s,"%d nodes of simulator.txt",node_number);
    std::ofstream out;
    out.setf(std::ios::fixed | std::ios::left);
    out.open(s,std::ios::out | std::ios::trunc);

    /*
     * tips
     * 中文与英文对齐，则设其令中文字节宽度为a，英文字节宽度为b
     * 则a,b大致满足b=2a/3+1
     * 因为utf-8编码，一个中文为3个字节，英文为一个字节
     * windows系统，英文字符的长度约为中文字符的一半
     */
    out<<std::setw(24)<<"节点仿真个数"
    <<std::setw(24)<<"节点传播范围"
    <<std::setw(24)<<"节点传输速率"
    <<std::setw(24)<<"节点传播速度"
    <<std::setw(24)<<"节点传播功率"
    <<std::setw(24)<<"节点接收功率"
    <<std::setw(24)<<"节点传播误码率"<<std::endl;
    out<<std::setw(17)<<node_number
    <<std::setw(17)<<propagate_range
    <<std::setw(17)<<transmission_rate
    <<std::setw(17)<<propagate_speed
    <<std::setw(17)<<propagate_power
    <<std::setw(17)<<receive_power
    <<std::setw(17)<<propagate_error<<std::endl;

    out<<std::endl<<std::endl<<"节点传输负载大小"<<std::endl;
    out<<Payload<<std::endl;
    
    
    
    // 随机数生成
    e.seed(time(NULL));
    e();
    srand(time(NULL)+e());
    std::uniform_int_distribution<> location(0,(uint32_t)propagate_range);
    double x,y;
    struct NodeLocation node_location;
    out<<std::endl<<std::endl<<"在半径为"<<propagate_range/2<<"m的范围内，节点的位置坐标"<<std::endl
    <<std::setw(19)<<"节点ID"
    <<std::setw(23)<<"节点X轴坐标"
    <<std::setw(24)<<"节点Y轴坐标"<<std::endl;
    for(uint32_t i=0; i < node_number; i++){
        for (; ;)
        { 
            x=location(e);
            y=location(e);
            if(
                (x-propagate_range/2)*(x-propagate_range/2)
               +(y-propagate_range/2)*(y-propagate_range/2)
               < propagate_range*propagate_range/4
                ){
                break;
            }       
        }
       
        node_location.address=i;
        node_location.x_axis =x;
        node_location.y_axis =y;
        nodes.push_back(node_location);
        out<<std::setw(17)<<i<<std::setw(17)<<x<<std::setw(17)<<y<<std::endl;
    }

    //add
    node_location.address=node_number;
    node_location.x_axis =propagate_range/2;
    node_location.y_axis =propagate_range/2;
    nodes.push_back(node_location);
    out<<std::setw(17)<<node_number<<std::setw(17)<<propagate_range/2<<std::setw(17)<<propagate_range/2<<std::endl;

    std::vector<struct NodeLocation> (nodes).swap(nodes); // 释放多余空间
    #ifdef MY_DEBUG
        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"所有节点坐标占用的空间 "<<nodes.capacity()<<std::endl;
    #endif
    
    struct NeighborsNode temp;
    //add
    for(uint32_t i=0;i<nodes.size();i++){ 
        std::vector<struct NeighborsNode> neighbor;
        for(uint32_t j=0;j<nodes.size();j++){
            temp.local_address=i;
            temp.neighbor_address=j;
            temp.neighbor_distance=sqrt(
                (nodes.at(i).x_axis-nodes.at(j).x_axis)*(nodes.at(i).x_axis-nodes.at(j).x_axis)
               +(nodes.at(i).y_axis-nodes.at(j).y_axis)*(nodes.at(i).y_axis-nodes.at(j).y_axis));
            neighbor.push_back(temp);
        }
        std::vector<struct NeighborsNode> (neighbor).swap(neighbor); //释放多余的空间
        #ifdef MY_DEBUG
            std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"单个节点与邻居节点信息占用的空间 "<<neighbor.capacity()<<std::endl;
        #endif
        neighbor_node.push_back(neighbor);
    }
    std::vector<std::vector<struct NeighborsNode>> (neighbor_node).swap(neighbor_node); //释放多余的空间
    #ifdef MY_DEBUG
        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"所有节点与邻居节点信息占用的空间 "<<neighbor_node.capacity()<<std::endl;
    #endif
    // 打印距离信息
    out<<std::endl<<std::endl<<"节点之间的距离"<<std::endl<<std::setw(4)<<" ";
    for(uint32_t j=0; j<nodes.size(); j++){
        out<<std::setw(12)<<j;
    }
    out<<std::endl;
    for(uint32_t i=0;i<nodes.size();i++){ 
        out<<std::setw(4)<<i;
        for(uint32_t j=0;j<nodes.size();j++){
            out<<std::setw(12)<<neighbor_node[i][j].neighbor_distance;
        }
        out<<std::endl;
    }
    
    out<<std::endl<<std::endl;
    out.close();
    return EXIT_SUCCESS;
}

double SimulatorRun(double simulation_time){
    Node node[node_number+1];
    #ifdef MY_DEBUG
        std::cout<<"节点的坐标"<<std::endl;
    #endif
    std::vector<struct Packet> packet; //单个节点收到的数据包集合
    for(uint32_t i=0; i <= node_number ; i++){
        node[i].address_=nodes.at(i).address; //节点地址
        #ifdef MY_DEBUG
            std::cout<<nodes.at(i).address<<"\t"<<nodes.at(i).x_axis<<"\t"<<nodes.at(i).y_axis<<std::endl;
    	#endif
        rxpacketVector.push_back(packet);   // 每个节点接收的数据包
        txpacketVector.push_back(packet);   // 每个节点发送的数据包
        cachepacketVector.push_back(packet);// 每个节点等待发送的数据包
    }
    std::vector<std::vector<struct Packet>> (rxpacketVector).swap(rxpacketVector);
    std::vector<std::vector<struct Packet>> (txpacketVector).swap(txpacketVector);
    std::vector<std::vector<struct Packet>> (cachepacketVector).swap(cachepacketVector);
    #ifdef MY_DEBUG
        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"所有节点接收的数据包集合占用的空间大小 "<<rxpacketVector.capacity()<<std::endl;
        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"所有节点发送的数据包集合占用的空间大小 "<<txpacketVector.capacity()<<std::endl;
        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"所有节点缓存的数据包集合占用的空间大小 "<<cachepacketVector.capacity()<<std::endl;
    #endif

    uint32_t send_num=0;         //发送rts的节点个数
    std::vector<int> send_index; //哪些节点在发送数据
	std::vector<int>::iterator iterVector;
    for(double bus_clock=0; bus_clock < simulation_time ; bus_clock += time_unit){
        send_num = 0;           //主时间轴进行判断
        for(uint32_t i=0; i < node_number; i++){
            node[i].Run(bus_clock);
            if(channel_state==Idel)//信道是空闲状态
				if(node[i].work_state_==SendState)
					send_num++;
        }

        if(channel_state==Idel &&  rxpacketVector[node_number].size() > 0){//信道是空闲状态
            #ifdef MY_DEBUG
                std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                <<(send_num==rxpacketVector[node_number].size()?
                "send_num等于rxpacketVector[node_number].size()":"send_num不等于rxpacketVector[node_number].size()")
                <<"\t其中send_num="<<send_num<<std::endl;
            #endif
            for(uint32_t i=0; i <  rxpacketVector[node_number].size() ; i++){
                if( i+1 < rxpacketVector[node_number].size() ){
                    if(rxpacketVector[node_number][i].rx_end > rxpacketVector[node_number][i+1].rx_start){
                        send_num=0;
                        break;
                    }
                }
            }
            if(send_num==0){
                for(uint32_t j=0; j <  rxpacketVector[node_number].size() ; j++){
                    
                    node[rxpacketVector[node_number][j].from].work_state_=CollsionState;
                    
                    send_index.push_back(rxpacketVector[node_number][j].from);
                    node[rxpacketVector[node_number][j].from].SetAlarm(bus_clock+slot,IdleState);
                    // node[rxpacketVector[node_number][j].from].SetAlarm(bus_clock+2*slot,IdleState);
                    
                    int size=txpacketVector[rxpacketVector[node_number][j].from].size()-1;
                    if(size>=0){
                        txpacketVector[rxpacketVector[node_number][j].from][size].description="RTS冲突";
                    }
                    
                    //todo 
                    // 窗口问题
                    // if( node[rxpacketVector[node_number][j].from].cw_ < cw_max){
					// 	node[rxpacketVector[node_number][j].from].cw_= node[rxpacketVector[node_number][j].from].cw_*2;
                    // }else
                    // {
					// 	node[rxpacketVector[node_number][j].from].cw_ = cw_min;
                    // }

                    //数据问题 
                    node[rxpacketVector[node_number][j].from].tx_packet.state=Fail;
                    #ifdef MY_DEBUG
                        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                        <<"RTS冲突"
                        <<"id="<<node[rxpacketVector[node_number][j].from].tx_packet.id
                        <<" retransfer_number="<< node[rxpacketVector[node_number][j].from].tx_packet.retransfer_number
                        <<std::endl;
                    #endif
                    if(node[rxpacketVector[node_number][j].from].tx_packet.retransfer_number >= reTx_max){ 
                        #ifdef MY_DEBUG
                            std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                            <<"抛弃的包的id"<<node[rxpacketVector[node_number][j].from].tx_packet.id
                            <<std::endl;
                        #endif
                        node[rxpacketVector[node_number][j].from].drop_counter_++; //记录节点抛弃包的个数
                    }
                    
                }
            }else
            {
                double duration= std::ceil(rxpacketVector[node_number].size()*((Mac+Physical+Payload)/transmission_rate)/slot)*slot;
                duration+=3*slot; //debug
                #ifdef MY_DEBUG
                    std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"Payload="<<Payload<<" duration/slot="<<duration/slot<<std::endl;
                #endif
                for(uint32_t j=0; j <  rxpacketVector[node_number].size() ; j++){
                    #ifdef MY_DEBUG
                        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                        <<"节点"<<rxpacketVector[node_number][j].from<<"第"<<j+1<<"个发送数据"<<std::endl;
                    #endif
                    send_index.push_back(rxpacketVector[node_number][j].from);
                    node[rxpacketVector[node_number][j].from].SetAlarm(bus_clock+duration,IdleState);
                    node[rxpacketVector[node_number][j].from].tx_packet.state=Success;//发送成功后，方便重新初始化数据   
                    if(j==0){
                        node[rxpacketVector[node_number][j].from].SendData(bus_clock+2*slot);
                    }else
                    {
                        node[rxpacketVector[node_number][j].from].SendData(node[rxpacketVector[node_number][j-1].from].tx_packet.rx_end-
                                node[rxpacketVector[node_number][j].from].tx_packet.delay);
                    }
                }
            }

            channel_state=Busy;
        }
                
        if(!send_index.empty() && channel_state==Busy){
			for(iterVector=send_index.begin();iterVector!=send_index.end();){
				if(node[*iterVector].work_state_==IdleState){
					iterVector = send_index.erase(iterVector);
					if(send_index.empty()){
						channel_state = Idel;
                        rxpacketVector[node_number].clear();
						break;
					}
				}
				else{
					iterVector++;
				}
			}
		}

        #ifdef MY_DEBUG       
            // for(uint32_t i=0;i<node_number;i++){
            //     printf("id=%d\ttimer_=%d\tchannel_state=%d\twork_state_=%d\tbus_clock=%f\tcw_counter_=%d\tcw_=%d\n",i,
            //         node[i].timer_,channel_state,node[i].work_state_,node[i].current_time_,node[i].cw_counter_,node[i].cw_);
            // }
            for(uint32_t i=0;i<node_number;i++){
                std::cout<<"节点id="<<i <<"  运行时刻："<<bus_clock<<"ms"
                            <<"  定时器："<<(node[i].timer_==Off?"关闭":"开启")
                            <<"  信道："<<(channel_state==Busy?"忙碌":"空闲")
                            <<"  节点状态："<<Node::GetWorkState(node[i].work_state_)
                            <<"  节点计数器："<<node[i].cw_counter_<<"  退避窗口："<<node[i].cw_<<std::endl;
            }
        #else
            printf("\rcurrent progress:%.2lf%%",bus_clock/simulation_time*100);//显示执行进度
        #endif
    }
    #ifndef MY_DEBUG  
        printf("\rcurrent progress:%.2lf%%",simulation_time/simulation_time*100);//显示执行进度
    #endif
    
    // 统计数据
    for(uint32_t i=0; i< node_number; i++){
        packets+=node[i].packet_counter_;
        drop_packets+=node[i].drop_counter_;
        energy_consumption+=node[i].send_energy_;
    }
    total_packets+=packets+drop_packets;
    //这里我们设置接收功率和空闲功率都为80mw，发送功率为10w
    energy_consumption=energy_consumption*propagate_power+(simulation_time-energy_consumption)*receive_power;
    
    // 输出日志
    // extern double     average_delay;    // 平均时延
    char s[100];sprintf(s,"%d nodes of simulator.txt",node_number);
    std::ofstream out;
    out.setf(std::ios::fixed | std::ios::left);
    out.open(s, std::ios::app);
    if(out.is_open()){
        for(uint32_t i=0;i<node_number;i++){
            out<<std::endl<<"*******************************************************************************************************"
            <<std::endl<<"节点"<<i<<"收到的数据包"<<std::endl;
            out<<std::setw(20)<<"数据包ID"
            <<std::setw(24)<<"  源节点"
            <<std::setw(22)<<"目的节点"
            <<std::setw(22)<<"数据类型"
            <<std::setw(22)<<"接收状态"
            <<std::setw(24)<<"重传次数"
            <<std::setw(22)<<"传输时延"
            <<std::setw(24)<<"数据到达"
            <<std::setw(22)<<"传播开始"
            <<std::setw(24)<<"传播结束"
            <<std::setw(22)<<"接收开始"
            <<std::setw(22)<<"接收结束"
            <<std::setw(24)<<"传播时延"
            <<std::setw(20)<<"传播能量"<<"数据包状态描述"<<std::endl;
            for(uint32_t j=0;j<rxpacketVector[i].size();j++){
                out<<std::setw(18)<<rxpacketVector[i][j].id
                <<std::setw(18)<<rxpacketVector[i][j].from
                <<std::setw(18)<<rxpacketVector[i][j].to
                <<std::setw(18)<<GetPacketType(rxpacketVector[i][j])
                <<std::setw(18)<<GetPacketState(rxpacketVector[i][j])
                <<std::setw(18)<<rxpacketVector[i][j].retransfer_number
                <<std::setw(18)<<rxpacketVector[i][j].transmission
                <<std::setw(18)<<rxpacketVector[i][j].arrive
                <<std::setw(18)<<rxpacketVector[i][j].tx_start
                <<std::setw(18)<<rxpacketVector[i][j].tx_end
                <<std::setw(18)<<rxpacketVector[i][j].rx_start
                <<std::setw(18)<<rxpacketVector[i][j].rx_end
                <<std::setw(18)<<rxpacketVector[i][j].delay
                <<std::setw(18)<<rxpacketVector[i][j].energy
                <<rxpacketVector[i][j].description<<std::endl;
            }
            out<<std::endl<<"节点"<<i<<"发送的数据包"<<std::endl;
            out<<std::setw(20)<<"数据包ID"
            <<std::setw(24)<<"  源节点"
            <<std::setw(22)<<"目的节点"
            <<std::setw(22)<<"数据类型"
            <<std::setw(22)<<"发送状态"
            <<std::setw(24)<<"重传次数"
            <<std::setw(22)<<"传输时延"
            <<std::setw(24)<<"数据到达"
            <<std::setw(22)<<"传播开始"
            <<std::setw(24)<<"传播结束"
            <<std::setw(22)<<"接收开始"
            <<std::setw(22)<<"接收结束"
            <<std::setw(24)<<"传播时延"
            <<std::setw(20)<<"传播能量"<<"数据包状态描述"<<std::endl;
            for(uint32_t j=0;j<txpacketVector[i].size();j++){
                out<<std::setw(18)<<txpacketVector[i][j].id
                <<std::setw(18)<<txpacketVector[i][j].from
                <<std::setw(18)<<txpacketVector[i][j].to
                <<std::setw(18)<<GetPacketType(txpacketVector[i][j])
                <<std::setw(18)<<GetPacketState(txpacketVector[i][j])
                <<std::setw(18)<<txpacketVector[i][j].retransfer_number
                <<std::setw(18)<<txpacketVector[i][j].transmission
                <<std::setw(18)<<txpacketVector[i][j].arrive
                <<std::setw(18)<<txpacketVector[i][j].tx_start
                <<std::setw(18)<<txpacketVector[i][j].tx_end
                <<std::setw(18)<<txpacketVector[i][j].rx_start
                <<std::setw(18)<<txpacketVector[i][j].rx_end
                <<std::setw(18)<<txpacketVector[i][j].delay
                <<std::setw(18)<<txpacketVector[i][j].energy
                <<txpacketVector[i][j].description<<std::endl;
            }
            out.flush();
        }  
        out.close();
    }
    return EXIT_SUCCESS;
}