#shell->./njush

部分用于验证功能所使用的代码注释在源文件中；

可以支持的功能:
    1.make命令即可以完成编译，删除过程中的*.o文件，生成可执行文件njush,并运行；
	2.显示提示符(仅显示目前所位于的目录);
	3.设置读取一行不超过1024个字符
	4.解析命令和执行已经完成
	5.后台运行已实现
     
    6.内置命令:cd,exit,history:
        1)cd可以更改工作目录
        2)exit退出程序，在测试中由于检验多个进程，所以如果还有未结束的进程
          exit退出时，将会有提示.(再次输入字符y/n可以决定继续退出还是暂不推出)
        3)history 无参数  ：将显示最近的100条命令；
          history -c      ：清除历史命令；
          history [numer] ：将显示执行的number条命令;
          history !!      : 将执行最近的一条命令
          history !string : 查找以string开头的命令，例如执行过得ls 命令;
        history功能建立在一个cmd_table[CMD_TABLES]字符指针数组，将命令存储在其中，直到exit退出shell后才释放资源；
    
    7.管道重定向见讲义所示:
        输出重定向可以实现append追加模式，如: ls >> junk(append模式没有开启O_CREAT模式)；
		输入输出重定向测试时，我建立了文件input,执行 wc -l < input > junk将可以把input中内容行数输入到junk中；
		关于管道的个数，可以在lib.h中更改 #define PIPELINE 的大小
    
	注:很多提示消息(会以Debug开头)，是设定输出，测试时可以注释掉。