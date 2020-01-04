/**
 * 
 * Copyright (c) 2019 南京航空航天大学 航空通信网络研究室
 * 
 * @file     d:\code\matlab\SimWiFi\DCF\CA\main.cpp
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
#include <iostream>
#include <fstream>
#include <getopt.h>       //命令行参数
#include <stdio.h>
#include <stdlib.h>       //某些结构体定义和宏定义，如EXIT_FAILURE、EXIT_SUCCESS 、atoi()函数
#include <stdarg.h>       //可变参数函数的宏的声明
#include <string.h>       //memset
#include <iomanip>


const char short_options[] = "n:s:t:N:r:S:R:p:P:e:j:m:o:C:y:a:l:X:Y:c:h";
const struct option long_options[] = { 
    {"number", required_argument, NULL, 'n' },
{"simulation", required_argument, NULL, 's' },
    { "time" , required_argument, NULL, 't' },
    {"numbers",required_argument, NULL, 'N' },
    { "rate" , required_argument, NULL, 'r' },
    { "speed", required_argument, NULL, 'S' },
    { "range" , required_argument, NULL,'R' },
{"propagate_power", required_argument, NULL, 'p' },
{"receive_power",   required_argument, NULL, 'P' },
    { "error", required_argument, NULL, 'e' },
    {"jitter", required_argument, NULL, 'j' },
{"max_retransfer", required_argument, NULL, 'm' },
    { "slot",  required_argument, NULL, 'o' },
    {"CW",   required_argument, NULL,  'C' },
{"physical",   required_argument, NULL, 'y' },
    { "mac",   required_argument, NULL, 'a' },
{ "payload",   required_argument, NULL, 'l' },
    { "rts",   required_argument, NULL, 'X' },
    { "cts",   required_argument, NULL, 'Y' },
    { "ack",   required_argument, NULL, 'c' },
    { "help" , no_argument,       NULL, 'h' }, 
    { 0, 0, 0, 0 } 
    };
void usage(FILE * fp, int argc, char ** argv) 
{
    fprintf(fp, "Usage: %s [options]\n\n"
        "Options:\n"
        "-n | --number     number of nodes\n"
        "-s | --simulation time of simulation\n"
        "-t | --time       time uint of simulation(second)\n"
        "-N | --numbers    number of topology that is simulated\n"
        "-r | --rate       transmission rate\n"
        "-S | --speed      propagate speed\n"
        "-R | --range      propagate range\n"
        "-p | --propagate_power propagate power\n"
        "-P | --receive_power   receive power\n"
        "-e | --error      propagate error\n"
        "-j | --jitter     propagate speed jitter\n"
        "-m | --max_retransfer  the maximum times of retransmission\n"
        "-o | --slot       time unit of the mac \n"
        "-C | --CW         the value of backoff window\n"
        "-y | --physical   the header of physical\n"
        "-a | --mac        the header of mac\n"
        "-l | --payload    the length of data\n"
        "-X | --rts        the length of rts\n"
        "-Y | --cts        the length of cts\n"
        "-c | --ack        the length of ack\n"
        "", argv[0]);
}

int main(int argc , char* argv[])
{ 
    /*
     * 仿真程序内部运算毫妙为基本单位，输入秒为基本单位
     * 这是因为计算机认为相差级别小于10^-5的数相等
     * 如果以秒为基本单位，则对DCF协议的仿真(微妙级别的仿真)产生重大影响
     */
    channel_state     = Idel;           // 信道状态  空闲
    transmission_rate = 3000/1000.0;    // 传输速率  3  kb/s
    propagate_speed   = 1500/1000.0;    // 传播速度  1500 m/s
    propagate_range   = 1500;            // 传播范围  750 m
    propagate_power   = 10/1000.0;      // 传播功率  10 J/s
    receive_power     = 0.08/1000.0;    // 接收功率  80 mJ/s
    propagate_error   = 0;              // 传播误码率
    propagate_speed_jitter=50/1000.0;   // 传播速度变化 50 m/s

    Rts=176;        // RTS帧
    Cts=320;        // CTS帧
    Ack=14*8;       // 应答帧 
    Physical=0;     // 物理层头
    Mac=0;          // MAC头
    Payload=transmission_rate*(propagate_range/propagate_speed+(Rts+Cts)/transmission_rate);// 数据负载
   
    reTx_max=2;     // 最大重发次数
    slot=propagate_range/propagate_speed+Cts/transmission_rate;// 最小时隙单元 0.2s
    cw_min=2;       // 最小退避窗口
    // cw_max=16;   // 最大退避窗口 
    cw_max=pow(2,reTx_max)*cw_min;  // 最大退避窗口 
   
    node_number =1;                 // 仿真的节点个数
    simulation_time=1200*1000.0;    // 仿真时间
    time_unit=slot;                 // 仿真运行基本时间单元 ms
    topology_number=60;             // 拓扑个数

    
    energy_consumption=0;// 总能耗
    average_energy_consumption=0; // 能耗
    total_packets=0;     // 总的发包个数
    packets=0;           // 成功的个数
    drop_packets=0;      // 丢包的个数
    average_delay=0;     // 平均时延
    throughput=0;        // 吞吐量
    packet_loss_rate=0;  // 丢包率
    
    for (;;) {
		int index;
		int c;
           /*
            *  
            * int getopt_long(int argc, char * const argv[],  const char *optstring, const struct option *longopts, int *longindex); 
            * 参数argc和argv从main()继承，
            * optsting：选项参数组成的字符串,包含下列元素:
            * 1.单个字符,表示选项,
            * 2.单个字符后接一个冒号:表示该选项后必须跟一个参数。参数紧跟在选项后或者以空格隔开,该参数的指针赋给optarg
            * 3.单个字符后跟两个冒号:表示该选项后可以有参数也可以没有参数。如果有参数,参数必须紧跟在选项后不能以空格隔开,该参数的指针赋给optarg
            * optstring是一个字符串,表示可以接受的参数。例如,"a:b:c:d:",表示可以接受的参数是a,b,c,d，其中，a和b参数后面跟有更多的参数值
            * (例如：-a host -b name)
            * longopts：其实是一个结构的实例：
            * struct option {
            * const char *name; //name表示的是长参数名
            * int has_arg; //has_arg有3个值，no_argument(或者是0)，表示该参数后面不跟参数值
            *              // required_argument(或者是1),表示该参数后面一定要跟个参数值
            *              // optional_argument(或者是2),表示该参数后面可以跟，也可以不跟参数值
            * int *flag;   //判断getopt_long()的返回值
            *              //flag为null,则函数会返回与该项option匹配的val值;
            *              //flag不是NULL,则将val值赋予flag所指向的内存,并且返回值设置为0
            * int val;     //和flag决定返回值
            * }
            * longindex:
            * longindex 非空,它指向的变量将记录当前找到参数符合longopts里的第几个元素的描述，即是longopts的下标值
           */
		c = getopt_long(argc, argv, short_options, long_options, &index);

		if (-1 == c) //跳出循环
			break;
		switch (c) {
		case 0: /* getopt_long() flag */
			break;
		case 'n':
            /*
                * 
                * atof()    将字符串转换为双精度浮点型值
                * atoi()    将字符串转换为整型值
                * atol()    将字符串转换为长整型值
                * strtod()  将字符串转换为双精度浮点型值，并报告不能被转换的所有剩余数字
                * strtol()  将字符串转换为长整值，并报告不能被转换的所有剩余数字
                * strtoul() 将字符串转换为无符号长整型值，并报告不能被转换的所有剩余数字
            */
            /*
                * 
                * itoa()   将整型值转换为字符串
                * ltoa()   将长整型值转换为字符串
                * ultoa()  将无符号长整型值转换为字符串
                * gcvt()   将浮点型数转换为字符串，取四舍五入
                * ecvt()   将双精度浮点型值转换为字符串，转换结果中不包含十进制小数点
                * fcvt()：指定位数为转换精度，其余同ecvt()
            */
            node_number=atol(optarg);
			break;
		case 's':
            simulation_time=atof(optarg)*1000.0; //毫秒为基本单位
			break;
		case 't':
            time_unit=atof(optarg)*1000.0;        //毫秒为基本单位
			break;
        case 'N':
            topology_number=atol(optarg);
            break;
		case 'r':
            transmission_rate=atof(optarg)/1000.0;//毫秒为基本单位
			break;
		case 'S':
            propagate_speed=atof(optarg)/1000.0;  //毫秒为基本单位
			break;
	    case 'R':
            propagate_range=atof(optarg); //最大传播范围
            break;
        case 'p':
            propagate_power=atof(optarg)/1000.0;//毫秒为基本单位
            break;
        case 'P':
            receive_power  =atof(optarg)/1000.0;//毫秒为基本单位
            break;
        case 'e':
            propagate_error=atof(optarg);
            break;
        case 'j':
            propagate_speed_jitter=atof(optarg)/1000.0;//毫秒为基本单位
            break;
        case 'm':
            reTx_max=atol(optarg);
            cw_max=pow(2,reTx_max)*cw_min;  // 最大退避窗口 
            break;
        case 'o':
            slot =atof(optarg)*1000.0;//毫秒为基本单位
            break;
        case 'C':
            cw_min =atol(optarg);
            break;
        case 'y':
            Physical =atol(optarg);
            break;
        case 'a':
            Mac =atol(optarg);
            break;
        case 'l':
            Payload =atol(optarg);
            break;
        case 'X':
            Rts =atol(optarg);
            break;
        case 'Y':
            Cts =atol(optarg);
            break;
        case 'c':
            Ack =atol(optarg);
            break;
        case 'h':
            usage(stderr,argc,argv);
            return EXIT_SUCCESS;
		default:
		    usage(stderr,argc,argv);
			break;
		}
	}

    // e.seed(time(NULL));
    // e();
    srand(time(NULL)+e());
    for(int nums=0; nums<topology_number; nums++ ){
        CreateNodes();

        int times=2; //每个拓扑仿真三次
        for(int num=0; num<times; num++){
            
            SimulatorRun(simulation_time);
            
            char s[100];sprintf(s,"%d nodes of simulator.txt",node_number);
            std::ofstream out;
            out.setf(std::ios::fixed | std::ios::left);
            out.open(s, std::ios::app);
            out<<"################################"<<"第"<<num+1<<"次仿真结果"<<"################################"<<std::endl;
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
                    <<std::setw(22)<<"数据长度"
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
                    <<std::setw(22)<<"数据长度"
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
            // 清除状态
            std::vector<std::vector<struct Packet>> ().swap(cachepacketVector);
            std::vector<std::vector<struct Packet>> ().swap(rxpacketVector);
            std::vector<std::vector<struct Packet>> ().swap(txpacketVector);
            channel_state=Idel;
        }
    
        packets/=times;
        drop_packets/=times;
        energy_consumption/=times;

        
        total_packets=packets+drop_packets;
        //这里我们设置接收功率和空闲功率都为80mw，发送功率为10w
        energy_consumption=energy_consumption*propagate_power+(simulation_time-energy_consumption)*receive_power;
        
        printf("Result:\n\t total_packets=%d packets=%d drop_packets=%d energy_consumption=%lf\n\n",total_packets,packets,drop_packets,energy_consumption);
        packet_loss_rate=1.0*drop_packets/total_packets;               //丢包率
        // throughput=packets*Payload*1000.0/simulation_time;          //吞吐量
        throughput=(packets*Payload/transmission_rate)/simulation_time;//归一化吞吐量
        average_energy_consumption=energy_consumption/node_number;     //节点平均能耗
        
        char s[100];sprintf(s,"%d nodes of simulator result.txt",node_number);
        std::ofstream out;
        out.setf(std::ios::fixed | std::ios::left);
        out.open(s, std::ios::app);
        if(out.is_open()){
            out<<std::endl<<"第"<<nums+1<<"个拓扑仿真结果"<<std::endl;
            out<<std::setw(18)<<"仿真时间"<<std::setw(18)<<"吞吐量"<<std::setw(18)<<"丢包率"<<std::setw(18)<<"平均能耗"<<std::endl;
            out<<std::setw(13)<<simulation_time/1000.0
            <<std::setw(15)<<std::setprecision(2)/* 保留两位小数*/<<throughput
            <<std::setw(14)<<packet_loss_rate
            <<std::setw(14)<<average_energy_consumption<<std::endl;;
            out.close();
        }

        //清除状态
        nodes.clear();         // 每个节点的位置
        neighbor_node.clear(); // 邻居节点之间的距离
        channel_state=Idel;
        energy_consumption=0;// 总能耗
        average_energy_consumption=0; // 能耗
        total_packets=0;     // 总的发包个数
        packets=0;           // 成功的个数
        drop_packets=0;      // 丢包的个数
        average_delay=0;     // 平均时延
        throughput=0;        // 吞吐量
        packet_loss_rate=0;  // 丢包率
    }

	return EXIT_SUCCESS;
}