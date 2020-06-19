#ifndef LOG_PROCESS_H
#define LOG_PROCESS_H

#include <time.h>
#include <string>
#include <vector>
#include <queue>
#include <deque>
using namespace std;

//#include "runnable.h"
#include "HbFrame_Operation.h"
#include "base_comm.h"
#include "mod_logSysInfo.h"
#define LOG_PATH "SD/Log/"

//#define TD_F 0 //找到文件
//#define TD_NS 1 //无法获取信息
//#define TD_DNR 2 //无法读取目录

#define DEALTYPE_COUNT 0 //计数
#define DEALTYPE_DELETE 1 //删除
#define DEALTYPE_SORT 2 //排序
///////////////////////////////////*************wjc79**************////////////////////////
#define MAX_LOGCAP  50 //一条消息中，最大可包含 几条日志
#define CLEARLOG_TIMER  60 //定时清除日志的间隔时长，单位秒
#define CLEARLOG_COUNT 5 //定时清除日志的条数
#define MAX_LOGCOUNT 20 //日志的最大文件数(.log条数  小时数目)

//日志类型
#define LOGTYPE_DEVICE_FAULT    0   //设备故障
#define LOGTYPE_FAULT_SOLVE     1   //故障恢复
#define LOGTYPE_RUN_LOG         2   //运行记录

//日志码
#define DEVICE_START_SUCCESS        0x00000001  //读写器启动成功
#define DEVICE_START_FAILURE        0x00000002  //读写器启动失败
#define DEVICE_RESET                0x00000003  //读写器复位
#define DEVICE_DOWNLOAD_VERSION     0x00000004  //读写器版本下载
#define DEVICE_NTP_EXCEPTION        0x00000005  //读写器NTP同步异常
#define DEVICE_PLL_FAILURE          0x00000006  //读写器锁相环失效
#define DEVICE_PSAM_FAULT           0x00000007  //读写器PSAM故障

#define SECURE_START_SUCCESS        0x01000001  //安全模块启动成功
#define SECURE_START_FAILURE        0x01000002  //安全模块启动失效
#define SECURE_RESET                0x01000003  //安全模块复位
#define SECURE_DOWNLOAD_VERSION     0x01000004  //安全模块版本下载
#define SECURE_SPI_FAULT            0x01000005  //安全模块SPI通信异常

//////////////////////////////////////////////////////////////////////////////////
//以下为扩展出的日志
#define COMM_LINK_LIVE        0x02000001 //通信链路建立
#define COMM_LINK_BREAK     0x02000002 //扩展出的日志，表示通信链路中断
#define CPU_IDLE_LOW            0x02000003 //CPU占用率过大
#define DEVICE_NTP_RESTORE  0x02000004 //NTP同步恢复

typedef struct{
    string Year;
    string Month;
    string Day;
    string Hour;
    string Min;
    string Sec;
}STRUCT_LOCALTIME_STRING;

/**
 * @brief Log_Process::Log_Process
 * by lijin 2017.06.16
 * 日志模块，主要功能是实现各类日志的记录，查询，删除等
 * 日志的记录以文本文件方式进行。目录结构是相应的年建一个目录，月建一个目录
 * 日建一个目录。一小时，建一个文件。
 */

class Log_Process
{
public:
    Log_Process();
    ~Log_Process();

private:
    struct tm *convertTimeTToTm(time_t t);
    llrp_u16_t logSerial; //日志序列号
//    queue<string> m_UploadLogQueue; //待发送的日志队列     ///wjc  这个容器即使POP 也不会释放内存 决定改成deque
      deque<LOG_CONTENT_UNION> m_UploadLogQueue; //待发送的日志队列
    int m_CurrentCommID; // 当前正在通信的通信ID号
    vector<string> m_FoundLogFiles; //找到的日志文件列表
    int m_PendingUploadCount; //待处理的剩作日志上传数，用作按条数查询日志
    int m_Logcount;//日志存储总数
    llrp_u64_t m_UploadPeriod; //定时上传的时间间隔，单位微秒。等于0表示定时上传关闭
    llrp_u64_t m_LastUploadUTC; //最后一次定时上传的时间UTC时间
    llrp_u64_t m_LastClearLogUTC;//最后一次定时清除日志的UTC时间
private:
    time_t convertTmToTimeT(struct tm *t);
    STRUCT_LOCALTIME_STRING convertUTCToLocal(unsigned long long utc_time);
    unsigned long long getCurrentUTC();
    int confirmDir(char *dirPath);
    string getFilenameByTime(unsigned long long utc_time);
    vector<string> splitString(string str,string pattern);
//    CUploadDeviceLogAck *genUploadAckFromLog(queue<string> *pLogs); ///wjc 更换容器
      CUploadDeviceLogAck *genUploadAckFromLog(deque<LOG_CONTENT_UNION> *pLogs);
    int findLogFiles(vector<string> *pfoundfiles,int dealtype);
    bool uploadLogFinish; //上传日志结束
    llrp_u64_t m_CurrentUTCStrart; //当前正在进行的日志查询起始时间
    llrp_u64_t m_CurrentUTCEnd; //当前正在进行的日志查询结束时间
public:
    int dealMessage(CMessage *pMessage, Base_Comm *pComm);

    bool addLog(int type, int code, string descinfo);
    bool addLog(unsigned long long utctime, int type, int code, string descinfo);
    int getLogCount();
    bool deleteLogByTime(); //按起始日期删除日志
    bool deleteLogByCount(int count); //按条数删除最先的日志
    unsigned long long  getLogByTime(unsigned long long utc_start,unsigned long long utc_end); //按起始日期取出日志
    int getLogByCount(int count); //按条数取出日志

private:
    void timedUpdateLog(unsigned int timer_ms); //定时上传日志

    void dealLogFile(vector<string> *pFiles,char *filestr,int dealtype);
    int findLogFiles(char *basePath,vector<string> *pfoundfiles,int dealtype);
    int clearLogEmptyDir(char *basePath);

public:
    void run();

private:
	

private:
    void logFileSort(vector<string> *pfoundfiles);
    void logFileReverse(vector<string> *pfoundfiles);
    /////////////////////////
    int m_last_position;  //  记录当前日志查询到的位置
    int m_now_position;
public:
        //单独处理不插安全模块的情况
  bool addSecureLog(int type, int code, string descinfo);
   time_t  m_last_record_secure_logtime; // 上次记录安全模块异常的时间


};

#endif // LOG_PROCESS_H
