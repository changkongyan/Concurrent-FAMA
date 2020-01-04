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
 * Last Modified:  2019-12-31
 * Modified By:    詹长健 (2233930937@qq.com)
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

    out<<std::endl<<std::endl<<"节点传输负载大小(bit)"<<std::endl;
    out<<Payload<<std::endl;
    
    
    
    // 随机数生成
    // e.seed(time(NULL));
    // e();
    // srand(time(NULL)+e());
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

int SimulatorRun(double simulation_time){
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
    std::vector<int> send_rts_index;  //哪些节点在发送rts数据
    std::vector<int> send_data_index; //哪些节点在发送data数据
	std::vector<int>::iterator iterVector;
    for(double bus_clock=0; bus_clock < simulation_time ; bus_clock += time_unit){
        send_num = 0;           //主时间轴进行判断
        for(uint32_t i=0; i < node_number; i++){
            node[i].Run(bus_clock);
            if(channel_state==Idel)//信道是空闲状态
				if(node[i].work_state_==SendState){
					send_num++;
                }
        }

        if(channel_state==Idel &&  rxpacketVector[node_number].size() > 0){//信道是空闲状态
            
            #ifdef MY_DEBUG
                std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                <<(send_num==rxpacketVector[node_number].size()?
                "send_num等于rxpacketVector[node_number].size()":"send_num不等于rxpacketVector[node_number].size()")
                <<"\t其中send_num="<<send_num<<std::endl;
            #endif
            
            for(uint32_t i=0,temp=0; i < rxpacketVector[node_number].size() ; i++){
                temp=0;
                if(i==0 || i== rxpacketVector[node_number].size()-1){
                    temp++;
                }

                #ifdef MY_DEBUG
                std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                <<"temp="<<temp<<std::endl;
                #endif

                if(i && rxpacketVector[node_number][i-1].rx_end < rxpacketVector[node_number][i].rx_start){
                    temp++;
                }
                if (i+1 < rxpacketVector[node_number].size() && rxpacketVector[node_number][i].rx_end < rxpacketVector[node_number][i+1].rx_start ){
                    temp++;
                }
                
                #ifdef MY_DEBUG
                std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                <<"temp="<<temp<<std::endl;
                #endif
                
                if(rxpacketVector[node_number].size()==1 && temp==1){
                    temp=2;
                }
                
                if(temp!=2 ){//判断是否发生冲突，不等于2表示发生冲突
                    send_num--;
                    
                    node[rxpacketVector[node_number][i].from].work_state_=CollsionState;
                    
                    send_rts_index.push_back(rxpacketVector[node_number][i].from);

                    // node[rxpacketVector[node_number][j].from].SetAlarm(bus_clock+slot,IdleState);
                    node[rxpacketVector[node_number][i].from].SetAlarm(bus_clock+2*slot,IdleState); //进行握手
                    
                    int size=txpacketVector[rxpacketVector[node_number][i].from].size()-1;
                    if(size>=0){
                        txpacketVector[rxpacketVector[node_number][i].from][size].description="RTS冲突";
                    }
                    
                    // todo 
                    // 窗口问题
                    if( node[rxpacketVector[node_number][i].from].cw_ < cw_max){
                        #ifdef MY_DEBUG
                            std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                            <<"退避窗口大小"<<node[rxpacketVector[node_number][i].from].cw_
                            <<" 最大退避窗口"<<cw_max<<std::endl;
                        #endif
						node[rxpacketVector[node_number][i].from].cw_= node[rxpacketVector[node_number][i].from].cw_*2;
                        #ifdef MY_DEBUG
                            std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                            <<"退避窗口大小"<<node[rxpacketVector[node_number][i].from].cw_
                            <<" 最大退避窗口"<<cw_max<<std::endl;
                        #endif
                    }else
                    {
                        #ifdef MY_DEBUG
                            std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                            <<"退避窗口大小"<<node[rxpacketVector[node_number][i].from].cw_
                            <<" 最大退避窗口"<<cw_max<<std::endl;
                        #endif
                        node[rxpacketVector[node_number][i].from].cw_= cw_max;
                         #ifdef MY_DEBUG
                            std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                            <<"退避窗口大小"<<node[rxpacketVector[node_number][i].from].cw_
                            <<" 最大退避窗口"<<cw_max<<std::endl;
                        #endif
                    }
                     

                    //数据问题 
                    node[rxpacketVector[node_number][i].from].tx_packet.state=Fail;
                    #ifdef MY_DEBUG
                        std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                        <<"RTS冲突"
                        <<"id="<<node[rxpacketVector[node_number][i].from].tx_packet.id
                        <<" retransfer_number="<< node[rxpacketVector[node_number][i].from].tx_packet.retransfer_number
                        <<std::endl;
                    #endif
                    if(node[rxpacketVector[node_number][i].from].tx_packet.retransfer_number >= reTx_max){ 
                        #ifdef MY_DEBUG
                            std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                            <<"抛弃的包的id为"<<node[rxpacketVector[node_number][i].from].tx_packet.id
                            <<std::endl;
                        #endif
                        node[rxpacketVector[node_number][i].from].drop_counter_++; //记录节点抛弃包的个数
                    }
                }else{
                    send_data_index.push_back(rxpacketVector[node_number][i].from);  //成功发送RTS帧
                }
            } 
            if(send_num > 0){   
                 
                int nums_=send_num>4?4:send_num;
                double duration= std::ceil((nums_*((Mac+Physical+Payload)/transmission_rate)+node[send_data_index[0]].tx_packet.delay)/slot)*slot;
                // double duration= std::ceil((send_num*((Mac+Physical+Payload)/transmission_rate)+node[send_data_index[0]].tx_packet.delay)/slot)*slot;
                duration+=3*slot; //debug
                #ifdef MY_DEBUG
                    std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"<<"Payload="<<Payload<<" duration/slot="<<duration/slot<<std::endl;
                #endif
                
                if(send_data_index.size()<4){
                    for(   uint32_t j=0; j < send_data_index.size(); j++){ //最多传输四个
                    // for(uint32_t j=0; j <  send_data_index.size() ; j++){
                        #ifdef MY_DEBUG
                            std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                            <<"节点"<<send_data_index[j]<<"第"<<j+1<<"个发送数据"<<std::endl;
                        #endif

                        send_rts_index.push_back(send_data_index[j]);
                        
                        node[send_data_index[j]].SetAlarm(bus_clock+duration,IdleState);
                        node[send_data_index[j]].tx_packet.state=Success;//发送成功后，方便重新初始化数据   
                        if(j==0){
                            node[send_data_index[j]].SendData(bus_clock+2*slot);
                        }
                        else
                        {
                            node[send_data_index[j]].SendData(node[send_data_index[j-1]].tx_packet.rx_end-node[send_data_index[j]].tx_packet.delay);
                        } 
                        
                        node[send_data_index[j]].cw_ = cw_min;
                    }
                }else{
                    for(  uint32_t j=4;j< send_data_index.size();j++){


                        node[send_data_index[j]].work_state_=CollsionState;
                        send_rts_index.push_back(send_data_index[j]);

                        // node[rxpacketVector[node_number][j].from].SetAlarm(bus_clock+slot,IdleState);
                        node[send_data_index[j]].SetAlarm(bus_clock+2*slot,IdleState); //进行握手
                        
                        int size=txpacketVector[send_data_index[j]].size()-1;
                        if(size>=0){
                            txpacketVector[send_data_index[j]][size].description="RTS冲突";
                        }
                        
                        // todo 
                        // 窗口问题
                        if( node[send_data_index[j]].cw_ < cw_max){
                            #ifdef MY_DEBUG
                                std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                                <<"退避窗口大小"<<node[send_data_index[j]].cw_
                                <<" 最大退避窗口"<<cw_max<<std::endl;
                            #endif
                            node[send_data_index[j]].cw_= node[send_data_index[j]].cw_*2;
                            #ifdef MY_DEBUG
                                std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                                <<"退避窗口大小"<<node[send_data_index[j]].cw_
                                <<" 最大退避窗口"<<cw_max<<std::endl;
                            #endif
                        }else
                        {
                            #ifdef MY_DEBUG
                                std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                                <<"退避窗口大小"<<node[send_data_index[j]].cw_
                                <<" 最大退避窗口"<<cw_max<<std::endl;
                            #endif
                            node[send_data_index[j]].cw_= cw_max;
                            #ifdef MY_DEBUG
                                std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                                <<"退避窗口大小"<<node[send_data_index[j]].cw_
                                <<" 最大退避窗口"<<cw_max<<std::endl;
                            #endif
                        }
                        

                        //数据问题 
                        node[send_data_index[j]].tx_packet.state=Fail;
                        #ifdef MY_DEBUG
                            std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                            <<"RTS冲突"
                            <<"id="<<node[send_data_index[j]].tx_packet.id
                            <<" retransfer_number="<< node[send_data_index[j]].tx_packet.retransfer_number
                            <<std::endl;
                        #endif
                        if(node[send_data_index[j]].tx_packet.retransfer_number >= reTx_max){ 
                            #ifdef MY_DEBUG
                                std::cout<<"DEBUG "<<__FILE__<<"/"<<__LINE__<<":"
                                <<"抛弃的包的id为"<<node[send_data_index[j]].tx_packet.id
                                <<std::endl;
                            #endif
                            node[send_data_index[j]].drop_counter_++; //记录节点抛弃包的个数
                        }
    
                    }
                }
            
            }
            channel_state=Busy;
        }
                
        if(!send_rts_index.empty() && channel_state==Busy){
			for(iterVector=send_rts_index.begin();iterVector!=send_rts_index.end();){
				if(node[*iterVector].work_state_==IdleState){
					iterVector = send_rts_index.erase(iterVector);
					if(send_rts_index.empty()){
						channel_state = Idel;
                        rxpacketVector[node_number].clear();
                        send_data_index.clear();
						break;
					}
				}
				else{
					iterVector++;
				}
			}
		}

        #ifdef MY_DEBUG       
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
    // todo 
    // extern double     average_delay;    // 平均时延
    for(uint32_t i=0; i< node_number; i++){
        packets+=node[i].packet_counter_;
        drop_packets+=node[i].drop_counter_;
        energy_consumption+=node[i].send_energy_;
    }
    printf("\npackets=%d\tdrop_packets=%d\tenergy_consumption=%lf\n",packets,drop_packets,energy_consumption);

    return EXIT_SUCCESS;
}