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

//#define TD_F 0 //�ҵ��ļ�
//#define TD_NS 1 //�޷���ȡ��Ϣ
//#define TD_DNR 2 //�޷���ȡĿ¼

#define DEALTYPE_COUNT 0 //����
#define DEALTYPE_DELETE 1 //ɾ��
#define DEALTYPE_SORT 2 //����
///////////////////////////////////*************wjc79**************////////////////////////
#define MAX_LOGCAP  50 //һ����Ϣ�У����ɰ��� ������־
#define CLEARLOG_TIMER  60 //��ʱ�����־�ļ��ʱ������λ��
#define CLEARLOG_COUNT 5 //��ʱ�����־������
#define MAX_LOGCOUNT 20 //��־������ļ���(.log����  Сʱ��Ŀ)

//��־����
#define LOGTYPE_DEVICE_FAULT    0   //�豸����
#define LOGTYPE_FAULT_SOLVE     1   //���ϻָ�
#define LOGTYPE_RUN_LOG         2   //���м�¼

//��־��
#define DEVICE_START_SUCCESS        0x00000001  //��д�������ɹ�
#define DEVICE_START_FAILURE        0x00000002  //��д������ʧ��
#define DEVICE_RESET                0x00000003  //��д����λ
#define DEVICE_DOWNLOAD_VERSION     0x00000004  //��д���汾����
#define DEVICE_NTP_EXCEPTION        0x00000005  //��д��NTPͬ���쳣
#define DEVICE_PLL_FAILURE          0x00000006  //��д�����໷ʧЧ
#define DEVICE_PSAM_FAULT           0x00000007  //��д��PSAM����

#define SECURE_START_SUCCESS        0x01000001  //��ȫģ�������ɹ�
#define SECURE_START_FAILURE        0x01000002  //��ȫģ������ʧЧ
#define SECURE_RESET                0x01000003  //��ȫģ�鸴λ
#define SECURE_DOWNLOAD_VERSION     0x01000004  //��ȫģ��汾����
#define SECURE_SPI_FAULT            0x01000005  //��ȫģ��SPIͨ���쳣

//////////////////////////////////////////////////////////////////////////////////
//����Ϊ��չ������־
#define COMM_LINK_LIVE        0x02000001 //ͨ����·����
#define COMM_LINK_BREAK     0x02000002 //��չ������־����ʾͨ����·�ж�
#define CPU_IDLE_LOW            0x02000003 //CPUռ���ʹ���
#define DEVICE_NTP_RESTORE  0x02000004 //NTPͬ���ָ�

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
 * ��־ģ�飬��Ҫ������ʵ�ָ�����־�ļ�¼����ѯ��ɾ����
 * ��־�ļ�¼���ı��ļ���ʽ���С�Ŀ¼�ṹ����Ӧ���꽨һ��Ŀ¼���½�һ��Ŀ¼
 * �ս�һ��Ŀ¼��һСʱ����һ���ļ���
 */

class Log_Process
{
public:
    Log_Process();
    ~Log_Process();

private:
    struct tm *convertTimeTToTm(time_t t);
    llrp_u16_t logSerial; //��־���к�
//    queue<string> m_UploadLogQueue; //�����͵���־����     ///wjc  ���������ʹPOP Ҳ�����ͷ��ڴ� �����ĳ�deque
      deque<LOG_CONTENT_UNION> m_UploadLogQueue; //�����͵���־����
    int m_CurrentCommID; // ��ǰ����ͨ�ŵ�ͨ��ID��
    vector<string> m_FoundLogFiles; //�ҵ�����־�ļ��б�
    int m_PendingUploadCount; //�������ʣ����־�ϴ�����������������ѯ��־
    int m_Logcount;//��־�洢����
    llrp_u64_t m_UploadPeriod; //��ʱ�ϴ���ʱ��������λ΢�롣����0��ʾ��ʱ�ϴ��ر�
    llrp_u64_t m_LastUploadUTC; //���һ�ζ�ʱ�ϴ���ʱ��UTCʱ��
    llrp_u64_t m_LastClearLogUTC;//���һ�ζ�ʱ�����־��UTCʱ��
private:
    time_t convertTmToTimeT(struct tm *t);
    STRUCT_LOCALTIME_STRING convertUTCToLocal(unsigned long long utc_time);
    unsigned long long getCurrentUTC();
    int confirmDir(char *dirPath);
    string getFilenameByTime(unsigned long long utc_time);
    vector<string> splitString(string str,string pattern);
//    CUploadDeviceLogAck *genUploadAckFromLog(queue<string> *pLogs); ///wjc ��������
      CUploadDeviceLogAck *genUploadAckFromLog(deque<LOG_CONTENT_UNION> *pLogs);
    int findLogFiles(vector<string> *pfoundfiles,int dealtype);
    bool uploadLogFinish; //�ϴ���־����
    llrp_u64_t m_CurrentUTCStrart; //��ǰ���ڽ��е���־��ѯ��ʼʱ��
    llrp_u64_t m_CurrentUTCEnd; //��ǰ���ڽ��е���־��ѯ����ʱ��
public:
    int dealMessage(CMessage *pMessage, Base_Comm *pComm);

    bool addLog(int type, int code, string descinfo);
    bool addLog(unsigned long long utctime, int type, int code, string descinfo);
    int getLogCount();
    bool deleteLogByTime(); //����ʼ����ɾ����־
    bool deleteLogByCount(int count); //������ɾ�����ȵ���־
    unsigned long long  getLogByTime(unsigned long long utc_start,unsigned long long utc_end); //����ʼ����ȡ����־
    int getLogByCount(int count); //������ȡ����־

private:
    void timedUpdateLog(unsigned int timer_ms); //��ʱ�ϴ���־

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
    int m_last_position;  //  ��¼��ǰ��־��ѯ����λ��
    int m_now_position;
public:
        //���������尲ȫģ������
  bool addSecureLog(int type, int code, string descinfo);
   time_t  m_last_record_secure_logtime; // �ϴμ�¼��ȫģ���쳣��ʱ��


};

#endif // LOG_PROCESS_H
