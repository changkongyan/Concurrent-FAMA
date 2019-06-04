!/bin/bash

echo -e "执行仿真程序.....\n"

if [ -x "./build/c-fama.exe" ] && [  -e "./build/c-fama.exe"  ]
then
	echo -e "可以执行程序\n"
    read  -p "请输入仿真节点个数、仿真窗口:" node_number CW simu_time
    if [ -z ${node_number} ] && [ -z ${CW} ] && [ -z ${simu_time} ]
    then
	    node_number=2
        CW=`expr ${node_number} + 2`
    fi

    # 程序执行的次数
    times=60
    for i in `seq 1 ${times}`
    do
        echo -e "\n第"${i}"次执行"
        ./build/c-fama.exe -n ${node_number} -C ${CW} -s ${simu_time}
        sleep 2
    done
    
    cat "${node_number} nodes of simulator result.txt" | awk 'NR%4==0' | awk ' { sum += $2 };END {print  '${node_number}'"个节点的吞吐量: "sum/'${times}' }' 

else
	echo -e "不可以执行程序,开始编译程序.....\n"
    g++.exe -v
	if [ $? == 0 ]
    then
		g++ -std=c++11 *.h *.cpp -o ./build/c-fama
		g++ -std=c++11 *.h *.cpp -o ./build/c-fama-d -D MY_DEBUG
		echo -e "c-fama为执行版本  c-fama-d为调试版本\n"
	else
		echo -e "请添加g++到环境变量或者安装g++编译器，完成编译\n"
	fi
fi