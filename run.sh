#!/bin/bash
echo -e "执行仿真程序.....\n"

if [ -x "./build/mfama.exe" ] && [  -e "./build/mfama.exe"  ] 
then
    echo  -e "可以执行程序\n"
    read  -p "请输入仿真节点个数、仿真窗口、仿真拓扑个数、仿真时间（s）:" node_number CW  topology_number simu_time
    if [ -z ${node_number} ] 
    then
	    node_number=5
    fi
    if  [ -z ${CW} ]  
    then
        CW=4
    fi
    if  [ -z ${simu_time} ] 
    then
        simu_time=1800
    fi
    if  [ -z ${topology_number} ] 
    then
        topology_number=50
    fi

    # 对比不同仿真节点个数的归一化吞吐量
    node_number=15
    for i in `seq 5 ${node_number}`
    do
        echo -e "\n"${i}"个仿真节点"
        ./build/mfama.exe -n ${i} -C ${CW} -s ${simu_time} -N ${topology_number}
        cat "${i} nodes of simulator result.txt" | awk 'NR%4==0' | awk ' { sum += $2 };END {print "\n\n"'${i}'"个节点的吞吐量: "sum/'${topology_number}' >>"simulator results of '${node_number}' nodes.txt"}' 
        # rm  "${i} nodes of simulator result.txt"
        # rm  "${i} nodes of simulator.txt"
    done

    # 对比不同窗口的归一化吞吐量
    # CW=10
    # for i in `seq 2 ${CW}`
    # do
    #     echo -e "\n"${node_number}"个仿真节点"
    #     ./build/mfama.exe -n ${node_number} -C ${i} -s ${simu_time} -N ${topology_number}
    #     cat "${node_number} nodes of simulator result.txt" | awk 'NR%4==0' | awk ' { sum += $2 };END {print "\n\n"'${i}'"个窗口的吞吐量: "sum/'${topology_number}' >> "simulator results of '${node_number}' nodes CW.txt"}' 
    #     rm  "${node_number} nodes of simulator result.txt"
    #     rm  "${node_number} nodes of simulator.txt"
    # done

else
	echo -e "不可以执行程序,开始编译程序.....\n"
    if [ ! -d "./build" ]
    then
        mkdir build
    fi
    which g++
	if [ $? == 0 ] 
    then
		g++ -std=c++11 *.h *.cpp -o ./build/mfama
		g++ -std=c++11 *.h *.cpp -o ./build/mfama-d -D MY_DEBUG
		echo -e "mfama为执行版本  mfama-d为调试版本\n"
	else
		echo -e "请添加g++到环境变量或者安装g++编译器，完成编译;且g++编译器支持c++11\n"
	fi
fi