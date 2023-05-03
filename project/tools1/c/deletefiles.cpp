/*************************/
/*      清理文件模块     */
/*************************/

#include "_public.h"

//程序退出和信号2、15的处理函数
void EXIT(int sig);

int main(int argc,char *argv[])
{
  //程序的帮助 
  if(argc!=4)
  {
    printf("\n");
    
    //pathname:指定扫描的目录；
    //matchstr:需要处理pathname目录下的哪个文件;
    //timeout:时间点，在时间点之前的文件会被压缩，之后的文件不会被压缩
    printf("Using:/project/tools1/bin/deletefiles pathname matchstr timeout\n\n");
   
    //有特殊字符需要用双引号包含起来。比如包含的*号
    printf("Example: /project/tools1/bin/deletefiles /log/idc \"*.log.20*\" 0.02\n"); 
    printf("Example: /project/tools1/bin/deletefiles /tmp/idc/surfdata \"*.xml,*.json\" 0.01\n");
    printf("Example: /project/tools1/bin/procctl 300  /project/tools1/bin/deletefiles /log/idc \".log.20*\" 0.02 \n");
    printf("Example: /project/tools1/bin/procctl 300  /project/tools1/bin/deletefiles /tmp/idc/surfdata \"*.xml,*.json\" 0.01\n\n");

    printf("这是一个工具类，用于删除历史数据文件或日志文件。\n");
    printf("本程序把pathname目录及子目录中timeout天之前的匹配matchstr文件全部删除，timeout可以是小数。\n");
    printf("本程序不写日志文件，也不会在控制台输出任何信息。\n\n");

    return -1;
  }

  //关闭全部的信号和输入输出
  //设置信号，在shell状态下可用"kill + 进程号"正常终止进程
  //但请不要用"kill -9 + 进程号"强行终止
  //CloseIOAndSignal(true);
  signal(SIGINT,EXIT);
  signal(SIGTERM,EXIT);

  //获取文件的超时点。
  char strTimeOut[21];
  LocalTime(strTimeOut,"yyyy-mm-dd hh24:mi:ss",0 - (int)(atof(argv[3])*24*60*60));

  CDir Dir;  //自定义CDir类
  //打开目录，CDir.OpenDir()函数
  //参数：目录名,匹配的规则文件,获取文件的最大数量,是否打开子目录,是否排序。
  if(Dir.OpenDir(argv[1],argv[2],10000,true,false)==false)
  { 
    printf("Dir.OpenDir(%s) failed.\n",argv[1]);
    return -1;
  }

  char strCmd[1024]; //存放gzip压缩文件的命令
  
  //遍历目录中的文件名
  while(true)
  {
    //得到一个文件的信息，CDir.ReadDir()函数。
    if(Dir.ReadDir()==false)
    {
      break;
    }
   
    // printf("DirName=%s,FileName=%s,FullFileName=%s,FileSize=%d,ModifyTime=%s,AccessTime=%s\n",\
    //         Dir.m_DirName,Dir.m_FileName,Dir.m_FullFileName,Dir.m_FileSize,Dir.m_ModifyTime,Dir.m_AccessTime);
    // printf("FullFileName=%s\n",Dir.m_FullFileName);    

    //与超时的时间点比较，如果更早，就需要压缩。
    if(strcmp(Dir.m_ModifyTime,strTimeOut)<0)
    {
      if(REMOVE(Dir.m_FullFileName)==true)  //自定义REMOVE函数删除文件函数
      {
        printf("REMOVE %s ok.\n",Dir.m_FullFileName);
      } 
      else
      {
        printf("REMOVE %s failed.\n",Dir.m_FullFileName);
      }
    }
  }

  return 0;
}

void EXIT(int sig)
{
  printf("程序退出，sig=%d\n\n",sig);
  
  exit(0);
}



