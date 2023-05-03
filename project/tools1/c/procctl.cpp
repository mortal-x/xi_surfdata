/******************************/
/*******服务调度程序***********/
/******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char *argv[])
{
  if (argc<3)
  {
    printf("Using:./procctl timetvl program argv ...\n");
    printf("Example:/project/tools1/bin/procctl 5 /usr/bin/tar zcvf /tmp/test_day1 /usr/include\n\n");

    printf("本程序是服务程序的调度程序，周期性启动服务程序或shell脚本。\n");
    printf("timetvl 运行周期，单位：秒。被调度的程序运行结束后，在timetvl秒后会被procctl重新启动。\n");
    printf("program 被调度的程序名，必须使用全路径。\n");
    printf("argvs   被调度的程序的参数。\n");
    printf("注意，本程序不会被kill杀死，但可以用kill -9强行杀死。\n\n\n");

    return -1;
  }  
  
  //关闭信号和IO，本程序不希望被打扰
  for(int i=0;i<64;i++)
  {
    signal(i,SIG_IGN);
    close(i);
  }  
   //生成子进程，父进程退出，让程序运行在后台，由系统1号托管。
  if(fork()!=0) exit(0);
  
  //启用SIGCHLD(子进程结束信号)信号，让父进程可以wait子进程退出的状态。
  signal(SIGCHLD,SIG_DFL); //SIG_DFL:恢复参数signum所指信号的处理方法为默认值。

  char *pargv[argc];
  for(int i=2;i<argc;i++)
  { 
    pargv[i-2]=argv[i];
  }  
  pargv[argc-2]=NULL;

  //该段程序含义：每隔5秒子进程调用一次excel函数，调用ls命令。
  while(true)
  {
    if(fork()==0)
    {
      execv(argv[2],pargv);
      exit(0); //execv掉用成功则不会调用该命令，调用失败则会运行该命令。
    }
    else
    {
      int status;
      wait(&status); //等待已终止的子进程，回收子进程
      sleep(atoi(argv[1]));
    }
  }

  return 0;
}


