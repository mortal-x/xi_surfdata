/********************************************************************************/
/* 程序名：crtsurfdata4.cpp  本程序用于生成全国气象站点观测的分钟数据           */
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

//全国气象站点分钟观测数据结构体
struct st_surfdata
{
  char obtid[11];    //站点代码
  char time[21];     //数据时间：格式yyyymmddhh24miss
  int temp;          //气温：单位，0.1摄氏度
  int pressure;      //气压：0.1百帕
  int RH;            //相对湿度(relative humidity),0-100之间的值
  int wind_dir;      //风向(wind direction):0-360之间的值。
  int wind_speed;    //风速:单位0.1m/s
  int rainfall;      //降雨量：0.1mm
  int vis;           //能见度（visibility）:0.1米
};

//存放全国气象站点分钟观测数据的容器
vector<struct st_surfdata> vsurfdata;

char strTime[21]; //观测数据的时间（全局变量）

//模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
void CrtSurfData();

//把容器vsurfdata容器中的全国气象站点分钟观测数据写入文件。
bool CrtSurfFile(const char *outpath,const char *datafmt);

int main(int argc,char *argv[])
{
  //inifile outpath logfile datafmt
  if(argc!=5){
    //如果参数非法，给出帮助文档
    printf("Using:./crtsurfdata4 inifile outpath logfile datafmt\n");
    printf("Example:/project/idc1/bin/crtsurfdata4 /project/idc1/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata4.log xml,josn,csv\n\n");

    printf("inifle 全国气象站点参数文件名。 \n");
    printf("outpath 全国气象站点数据文件存放的目录。 \n");
    printf("logfile 本程序运行的日志文件名。 \n\n");
    printf("datafmt 生成数据文件的格式，支持xml、json和csv三种格式，中间用逗号分隔。 \n\n");    

    return -1;
  }

  // 打开程序的日志文件。
  if(logfile.Open(argv[3],"a+",false)==false)
  {
     printf("logfile.Open(%s) failed.\n",argv[3]);
     return -1;
  }

  logfile.Write("crtsurfdata4 开始运行。 \n");
 
  //把站点参数文件中加载到vstcode容器中。
  if(LoadSTCode(argv[1])==false)
  {
    printf("站点参数文件不存在");
     return -1;
  }

  //模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
  CrtSurfData();

  //把容器vsurfdata容器中的全国气象站点分钟观测数据写入文件。
  if(strstr(argv[4],"xml")!=0) CrtSurfFile(argv[2],"xml");  //strstr()字符串操作函数
  if(strstr(argv[4],"josn")!=0) CrtSurfFile(argv[2],"josn");
  if(strstr(argv[4],"csv")!=0) CrtSurfFile(argv[2],"csv");  

  logfile.Write("crtsurfdata4 运行结束。 \n");

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
  }  */  

  return true;  
}


//模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
void CrtSurfData()
{
  //播随机数种子
  srand(time(0));

  //获取当前时间，当作观测时间
  memset(strTime,0,sizeof(strTime));
  LocalTime(strTime,"yyyymmddhh24miss");  

  struct st_surfdata stsurfdata; //定义结构体对象

  //遍历气象站点参数的vscode容器。
  for(int i=0;i<vstcode.size();i++)
  {
    memset(&stsurfdata,0,sizeof(struct st_surfdata));   

    //用随机数填充分钟观测数据的结构体
    strncpy(stsurfdata.obtid,vstcode[i].obtid,10);  //站点代码
    strncpy(stsurfdata.time,strTime,14);             //数据时间：格式yyyymmddhh24miss
    stsurfdata.temp = rand()%351;                    //气温：单位，0.1摄氏度
    stsurfdata.pressure = rand()%265+10000;          //气压：0.1百帕
    stsurfdata.RH = rand()%101;                      //相对湿度(relative humidity),0-100之间的值
    stsurfdata.wind_dir = rand()%361;                //风向(wind direction):0-360之间的值。
    stsurfdata.wind_speed = rand()%150;              //风速:单位0.1m/s
    stsurfdata.rainfall = rand()%16;                 //降雨量：0.1mm
    stsurfdata.vis = rand()%5001+100000;             //能见度（visibility）:0.1米

    //把观测数据的结构体放入vsurfdata容器。
    vsurfdata.push_back(stsurfdata);  
  }
}

//把容器vsurfdata容器中的全国气象站点分钟观测数据写入文件。
bool CrtSurfFile(const char *outpath,const char *datafmt)
{
  CFile File; 
 
  //拼接生成数据的文件名，例如：/tmp/surfdata/SURF_ZH_20210629092200_2254.csv
  char strFileName[301];
  sprintf(strFileName,"%s/SURF_ZH_%s_%d.%s",outpath,strTime,getpid(),datafmt);  //加上进程编号是为了保证生成的文件名不重复。 

  //打开文件
  if(File.OpenForRename(strFileName,"w")==false)
  {
    logfile.Write("File.OpenForRename(%s) failed.\n",strFileName);
    return false;
  }

  //写入第一行的标题（为了增加可读性）
  if(strcmp(datafmt,"csv")==0) //只有csv文件才能写入第一行增加可读性。
  {
    File.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");
  } 

  //遍历存放观测数据的vsufdata容器。
  for(int i=0;i<vsurfdata.size();i++)
  {  
    if(strcmp(datafmt,"csv")==0)
    {
      File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",\
      vsurfdata[i].obtid,vsurfdata[i].time,vsurfdata[i].temp/10.0,vsurfdata[i].pressure/10.0,\
      vsurfdata[i].RH,vsurfdata[i].wind_dir,vsurfdata[i].wind_speed/10.0,vsurfdata[i].rainfall/10.0,\
      vsurfdata[i].vis/10.0);
    }
  }

  //关闭文件。
  File.CloseAndRename();

  logfile.Write("生成数据文件%s成功，数据时间%s，记录数%d。\n",strFileName,strTime,vsurfdata.size());

  return true;
}












