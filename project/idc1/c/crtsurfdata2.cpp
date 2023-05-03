/********************************************************************************/
/* 程序名：crtsurfdata2.cpp  本程序用于生成全国气象站点观测的分钟数据           */
/* 作者：习家宝                                                                 */
/********************************************************************************/

#include "_public.h"

CLogFile logfile; //自定义框架日志文件操作中类CLogFile

//全国气象参数结构体
struct st_stcode
{
  char provname[31];  //省名
  char obtid[11];     //站号
  char obtname[31];   //站名
  double lat;         //纬度
  double lon;         //经度
  double height;      //海拔高度
};

//存放全国气象站点参数的容器。
vector<struct st_stcode> vstcode;

// 把站点参数文件中加载到vstcode容器中。
bool LoadSTCode(const char *inifile);  //自定义框架加载文件参数函数

int main(int argc,char *argv[])
{
  //inifile outpath logfile
  if(argc!=4){
    printf("Using:./crtsurfdata2 inifile outpath logfile\n");
    printf("Example:/project/idc1/bin/crtsurfdata2 /project/idc1/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata2.log\n\n");

    printf("inifle 全国气象站点参数文件名。 \n");
    printf("outpath 全国气象站点数据文件存放的目录。 \n");
    printf("logfile 本程序运行的日志文件名。 \n\n");

    return -1;
  }

// 打开程序的日志文件。
  if(logfile.Open(argv[3],"a+",false)==false)
  {
     printf("logfile.Open(%s) failed.\n",argv[3]);
     return -1;
  }

  logfile.Write("crtsurfdata2 开始运行。 \n");
 
  //把站点参数文件中加载到vstcode容器中。
  if(LoadSTCode(argv[1])==false)
  {
    printf("站点参数文件不存在");
     return -1;
  }


  logfile.Write("crtsurfdata2 运行结束。 \n");

  return 0;
}

// 把站点参数文件中加载到vstcode容器中。
bool LoadSTCode(const char *inifile)
{
  CFile File;  //自定义框架文件操作CFile类

  //打开站点参数文件
  if(File.Open(inifile,"r")==false)
  {
    logfile.Write("File.Open(%s) failed.\n",inifile);
    return false;
  }
  
  char strBuffer[301]; // 存放读取出来的每一行

  CCmdStr CmdStr;  //自定义框架字符串操作CCmdStr类，用于分割字符串
  struct st_stcode stcode; //定义结构体stcode存放拆分的每行数据

  while(true)
  {
    //从站点参数文件中读取一行，如果已读取完，跳出循环。
    if(File.Fgets(strBuffer,300,true)==false)
    {
      break;
    }

    //把读取的一行拆分
    CmdStr.SplitToCmd(strBuffer,",",true);

    if(CmdStr.CmdCount()!=6) //扔掉无效的行，如第一行总名称
    {
      continue;
    }

    //把站点参数的中的每个数据项保存到站点参数结构体中去
    CmdStr.GetValue(0,stcode.provname,30);  //省名
    CmdStr.GetValue(1,stcode.obtid,10);     //站号
    CmdStr.GetValue(2,stcode.obtname,30);   //站名
    CmdStr.GetValue(3,&stcode.lat);         //纬度
    CmdStr.GetValue(4,&stcode.lon);         //经度
    CmdStr.GetValue(5,&stcode.height);      //海拔高度

    //把站点参数结构体放到站点参数容器
    vstcode.push_back(stcode);
   
  }

  /*
  for (int i = 0; i < vstcode.size(); i++)
  {
    logfile.Write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%2.f,height=%.2f\n",vstcode[i].provname,\
                   vstcode[i].obtid,vstcode[i].obtname,vstcode[i].lat,vstcode[i].lon,vstcode[i].height);
  }
  */  

  return true;  

}







