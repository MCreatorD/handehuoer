#include "log_process.h"
#include <stdio.h>
#include "main_application.h"
#include "mod_logSysInfo.h"

Log_Process::Log_Process()
{
    this->logSerial = 0;
    this->uploadLogFinish = true;
    m_UploadPeriod = 0;
    m_LastUploadUTC = this->getCurrentUTC();
    m_LastClearLogUTC = this->getCurrentUTC();
    this->m_CurrentCommID = 0;
    m_last_position  = 0;  //��ǰ�ļ��Ĳ�ѯλ��Ϊ0
    m_now_position = 0;
	m_PendingUploadCount = 0;
	printf("1. ��־ϵͳ��ʼ��\n");
	if(LogSysInfoStruct.init() == LOG_PROC_FAILED)
	{
		printf("LOG_PROC_FAILED\r\n");
	}else{
		printf("LOG_PROC_SUCCEED\r\n");
	}
    /////////////////////////////////////////
    /*string testpath = WORK_DIR;
    testpath = testpath + LOG_PATH;
    testpath.erase(testpath.length()-1);
    this->findLogFiles((char*)testpath.c_str(),DEALTYPE_COUNT);
    printf("log count = %d.......................................\n",m_FoundLogFiles.size());*/
    //int c = this->getLogCount();
    //printf("log count = %d________________________________\n",c);
}

Log_Process::~Log_Process(){

}

/**
 * @brief Log_Process::convertTmToTimeT
 * @param t
 * @return �����ص���һ�����ں��Զ�������ڴ�ռ�ָ�룬����ȥ�ֶ��ͷ���
 */
struct tm * Log_Process::convertTimeTToTm(time_t t){
}
time_t Log_Process::convertTmToTimeT(struct tm *t){
}

unsigned long long Log_Process::getCurrentUTC()
{
	
}




//�ַ����ָ��
vector<string> Log_Process::splitString(string str,string pattern)
{
    string::size_type pos;
    vector<string> result;
    str+=pattern;//��չ�ַ����Է������
    int size=str.size();

    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

CUploadDeviceLogAck *Log_Process::genUploadAckFromLog(deque<LOG_CONTENT_UNION> *pLogs)
{
    //printf("genUploadAckFromLog:start!\n");
    /*if(pLogs->size()==0){
            return NULL;
        }*/
    CUploadDeviceLogAck *pack = NULL;
    CStatus *pstatus = (CStatus *)HbFrame_Operation::genParam(Status,0,NULL);
    pstatus->setStatusCode(StatusCode_M_Success);
	//ʵ������ �ϴ���־ ��Ӧ��Ϣ ��Ϣ����641
    pack  = (CUploadDeviceLogAck*)HbFrame_Operation::genMessage(UploadDeviceLogAck,0);
    //pack->setSequenceId(stoi(splitstr.at(0)));
    pack->setSequenceId(0);//��־���
    pack->setStatus(pstatus);//Ӧ��״̬���� ��ʵ������
    while(pLogs->size()>0){
        LOG_CONTENT_UNION logstr = pLogs->front();
        pLogs->pop_front();
        //printf("logstr is %s\n",logstr.c_str());
//        vector<string> splitstr = splitString(logstr,"|");
//        if(splitstr.size()<6){
//            //printf("error log...............\n");
//            continue;
//        }
		//ʵ���� ���� �豸��־���� ��������640
        CDeviceLog *pdevicelog = (CDeviceLog*)HbFrame_Operation::genParam(DeviceLog,0,NULL);
        //pdevicelog->setLogSequence(stoi(splitstr.at(0)));
        //printf("d0.............................................\n");
		pdevicelog->setLogSequence(logstr.ContentStruct.curNum);//u64 ���к�
        //pdevicelog->setLogSequence(atoi(splitstr.at(0).c_str()));//u64 ���к�
        //printf("d1.............................................\n");
        //pdevicelog->setLogType(stoi(splitstr.at(1)));
		pdevicelog->setLogType(logstr.ContentStruct.logtype);//u8 ��־���� 0:�豸���� 1: ���ϻָ� 2:���м�¼
        //pdevicelog->setLogType(atoi(splitstr.at(1).c_str()));//u8 ��־���� 0:�豸���� 1: ���ϻָ� 2:���м�¼
        //printf("d2.............................................\n");
        //pdevicelog->setLogCode(stoi(splitstr.at(2)));
		pdevicelog->setLogCode(logstr.ContentStruct.logcode); //u32 ��־��
        //pdevicelog->setLogCode(atoi(splitstr.at(2).c_str())); //u32 ��־�� stoi�������ڹ����ֵ��������ʸ��� atoi
        CUTCTimestamp *putc = (CUTCTimestamp*)HbFrame_Operation::genParam(UTCTimestamp,0,NULL);//ʱ���
        //printf("d3.............................................\n");
        putc->setMicroseconds(logstr.ContentStruct.curTime);
        pdevicelog->setUTCTimestamp(putc);
		  
		//��־������Ϣ
        //string temp = splitstr.at(5);
        //printf("temp.lenght=%d\n",temp.length());
        llrp_utf8v_t des = llrp_utf8v_t(logstr.ContentStruct.length);
		memcpy(des.m_pValue,logstr.ContentStruct.dateArry,des.m_nValue);
        //memcpy(des.m_pValue,splitstr.at(5).c_str(),des.m_nValue);
        pdevicelog->setLogDescInfo(des);
		//��Ӳ�������Ϣ����
        pack->addDeviceLog(pdevicelog);
    }//end of while
    deque<LOG_CONTENT_UNION>().swap(*pLogs);


    if(uploadLogFinish==true){ //�����Ƿ��ϴ�������־
        pack->setIsLastedFrame(1);
    }else{
        pack->setIsLastedFrame(0);
    }

    return pack;
}


/**
 * @brief Log_Process::addLog
 * @param utctime
 * @param type
 * @param code
 * @param descinfo����־�����ݣ�ע�⣬��|���ͻ��У��س���Ϊ�����ַ�������ʹ��
 * @return
 * �����־����־��ÿ��һ��Ŀ¼��ÿСʱһ���ļ������д�����
 * ���ɳ����һ���µ����ڣ���������ɾ��
 * ��Ƭ��ʵ����־��¼�洢ʱ������
 * ���洢4096��ѭ������
 */
bool Log_Process::addLog(unsigned long long utctime, int type, int code, string descinfo){
	printf("addLog:");
	LOG_CONTENT_UNION test; //��д�����־����
	test.ContentStruct.curTime = utctime;
	test.ContentStruct.logtype = type;
	test.ContentStruct.logcode = code;
	test.ContentStruct.length = descinfo.length();
	memcpy(test.ContentStruct.dateArry,descinfo.c_str(),test.ContentStruct.length);//��������
	test.ContentStruct.writeFlag = 0xA5;
	
	//����¼�洢������ʧ�Դ洢�� ��ʱʹ�õ���w25q256��Qspi flash 
	if(LOG_PROC_SUCCEED == LogSysInfoStruct.ContentStruct.record(test))
	{
		printf("sLogIndexUnion.IndexStruct.cntTotal:%d\n",sLogIndexUnion.IndexStruct.cntTotal);
		printf("��¼�ɹ�\r\n");
		return true;
	}
	return false;
}

bool Log_Process::addLog(int type, int code, string descinfo){
    return addLog(getCurrentUTC(),type,code,descinfo);
}

bool Log_Process::deleteLogByTime(){

}

/**
 * @brief Log_Process::deleteLogByCount
 * @param count
 * @return
 * ������ɾ�����ȵļ�����־
 */
bool Log_Process::deleteLogByCount(int count){

}

/**
 * @brief Log_Process::getLogByTime
 * @param utc_start
 * @param utc_end
 * @param max_count�������ѯ���������־��
 * @return,���ز�ѯ����ʱ����ʼutcʱ�䣬�������0��ʾ��ѯʧ�ܣ����ڵ��ڽ���ʱ�䣬���ʾ��ѯ������������ʾ��ѯδ����
 * ����ʼʱ���ȡ��־
 */
unsigned long long Log_Process::getLogByTime(unsigned long long utc_start,unsigned long long utc_end)
{

}


int Log_Process::getLogByCount(int count)
{
	LOG_CONTENT_UNION test1;//д���ȡ
	uint8_t tmp = count;
	
	while(count--)
	{
		LogSysInfoStruct.ContentStruct.getSerial(&test1,m_Logcount-count);
		//��ӡ��ȡ������������
		LogSysInfo_ContentPrint(test1);
		//��������͵���־����
        this->m_UploadLogQueue.push_front(test1);   ///����ͷ�巨
	}
	m_Logcount -= tmp;
}

int Log_Process::dealMessage(CMessage *pMessage, Base_Comm *pComm)
{
    if ( pMessage == NULL ) { return -1; }

	llrp_u32_t messageid = pMessage->getMessageID();
    CMessage *pack = NULL;
    CStatus *pstatus = NULL;

    //���ݲ�ͬ��Ϣ����
    switch(pMessage->m_pType->m_TypeNum){
    case UploadDeviceLog: //�ϴ��豸��־����
    {
        //m_CurrentCommID = commID;
        //�ϴ�����
        printf("uploadDeviceLog type is %d\n",((CUploadDeviceLog*)pMessage)->getUploadType());
        switch (((CUploadDeviceLog*)pMessage)->getUploadType()) {
        case UploadDeviceLogType_SERIAL_NUMBER: //����־�����ϴ�
        {
            int count = ((CUploadDeviceLog*)pMessage)->getUploadCount();
			m_Logcount = sLogIndexUnion.IndexStruct.cntTotal;//��¼�µ�ǰ����־����
			if(count>m_Logcount) 
			{
				this->m_PendingUploadCount = m_Logcount;
			}
			else
			{
				this->m_PendingUploadCount = count;
			}
			
            //printf("m_PendingUploadCount=%d\n",this->m_PendingUploadCount);
            break;
        }
        case UploadDeviceLogType_SERIAL_TIME: //��ʱ���ϴ�
        {
            llrp_u64_t starttime = ((CUploadDeviceLog*)pMessage)->getStartLogTimestamp()->getMicroseconds();
            llrp_u64_t endtime = ((CUploadDeviceLog*)pMessage)->getEndLogTimestamp()->getMicroseconds();
            m_CurrentUTCStrart = starttime;
            m_CurrentUTCEnd = endtime;

            break;
        }
        case UploadDeviceLogType_FIX_TIME: //��ʱ�ϴ�
        {
            /*ClearMessageVector();
            mRegularTime = ((CUploadDeviceLog*)pInMessage)->getMilliseconds()*1000;
            if( mRegularTime != 0 )
            {
                mIsTiming = true; //��ʱ��ʶ
                mStartUTCTime = mpDeviceLog->GetCurrentUTC(); //��¼��ʼ��UTCʱ��
                m_LinkID = linkID;
            }
            else
            {
                mIsTiming = false;
            }*/
            m_UploadPeriod =  ((CUploadDeviceLog*)pMessage)->getMilliseconds()*1000;
            m_LastUploadUTC = this->getCurrentUTC();
            break;
        }
        default:
            delete pstatus;
            return 0; //�Ѵ���
            break;
        }
        break;
    }
    case UploadDeviceLogConfirm: //�豸��־�ϴ�Ӧ��ȷ��
    {
        /*llrp_u16_t sequenceId = ((CUploadDeviceLogConfirm*)pInMessage)->getSequenceId();
        printf("sequenceId=%d,mSequenceIdMax=%d\n",sequenceId,mSequenceIdMax);
        if( sequenceId < mSequenceIdMax - 1 )
        {
            //printf("a0...................\n");
            retPack = FetchUploadAckFromVector();
        }
        else
        {
            //printf("a1...................\n");
            //�޷���
        }*/
        break;
    }
    case ClearDeviceLog: //����豸��־
    {
//        string logdir = WORK_DIR;
//        logdir = logdir + LOG_PATH;
//        string cmdstr = "rm -r -f ";
//        cmdstr = cmdstr + logdir;
//        //printf("cmdstr is %s\n",cmdstr.c_str());
//        system(cmdstr.c_str());
		//��ʵ�������־�� ����һ ������� ������ ����������������
		
		
        pstatus = (CStatus*)HbFrame_Operation::genParam(Status,0,NULL);
        pstatus->setStatusCode(StatusCode_M_Success);
        pack = HbFrame_Operation::genMessage(ClearDeviceLogAck, 0);
        HbFrame_Operation::addParamToMessage(pstatus, pack);
        break;
    }
    case DeviceLogCount: //�豸��־������ѯ
    {
        //m_CurrentCommID = commID;
        pstatus = (CStatus*)HbFrame_Operation::genParam(Status,0,NULL);
        pstatus->setStatusCode(StatusCode_M_Success);
        pack = HbFrame_Operation::genMessage(DeviceLogCountAck, 0);
		((CDeviceLogCountAck*)pack)->setLogCount(LogSysInfoStruct.getRawTotal());
//        ((CDeviceLogCountAck*)pack)->setLogCount(this->getLogCount());
//        printf("log count is %d\n",this->getLogCount());
        HbFrame_Operation::addParamToMessage(pstatus, pack);
        break;
    }
    default:
        return -1; //δ����
        break;
    }

    if( pack != NULL ) //��Ϣ��Ϊ�գ�������Ϣ
    {
        printf("send log message...................\n");
        //HbFrame_Operation::printMessage(retPack);
        if(pComm->immediateSendMessage(pack) == true) //��Ϣ����
        {
            printf("send success!\n");
        }
        else //����ʧ��
        {
            printf("send fail!\n");
        }
        //printf("a3...................\n");
        delete pack;
        //printf("a4...................\n");
    }
    return 0;
}


void Log_Process::run(){
    //��ʱ�ϴ�
//    if(m_UploadPeriod>10000){
//        llrp_u64_t currentutc = this->getCurrentUTC();
//        if(currentutc-m_LastUploadUTC>=m_UploadPeriod){
//            printf("Period upload log start!\n");
//            this->addLog(currentutc,LOGTYPE_RUN_LOG, DEVICE_START_SUCCESS, "Period upload Log test info!");
//            m_CurrentUTCStrart = m_LastUploadUTC + 1;
//            m_CurrentUTCEnd = currentutc;
//            m_LastUploadUTC = currentutc;
//        }
//    }
    //��ʱ���ѯ��ֻҪ��ʼʱ��С�ڽ���ʱ�䣬��Ϊ������������־
//    if(m_CurrentUTCStrart<m_CurrentUTCEnd){
//        m_CurrentUTCStrart = this->getLogByTime(m_CurrentUTCStrart,m_CurrentUTCEnd);
//        //        cout<<"m_CurrentUTCStrart ------->"<<m_CurrentUTCStrart<<endl;
//        //        cout<<"m_CurrentUTCEnd ------->"<<m_CurrentUTCEnd<<endl;
//        //        if(m_CurrentUTCStrart>=m_CurrentUTCEnd){   // ʱ��ͷְ����ܼ��
//        //            uploadLogFinish = true;
//        //        }else{
//        //            uploadLogFinish = false;
//        //        }
//        //�����ϴ���־
//        //cout<<"m_UploadLogQueue size ----------->"<<m_UploadLogQueue.size()<<endl;
//        CUploadDeviceLogAck *uploadack =  this->genUploadAckFromLog(&this->m_UploadLogQueue);
//        if(uploadack!=NULL){//������־��������־ʱ�������Ƿ�ɹ���ֻ��һ�Ρ�
//            printf("send upload log ack..............................!\n");

//            if(this->sendMessageFunc(uploadack,m_CurrentCommID)==false){
//                printf("send upload log ack fail!\n");
//            }
//            delete uploadack;
//        }else{
//            printf("uploadack is null!\n");
//        }
//    }//end of if
    //////////////////////////////////////////////////////////////////////////////////
    //��������ѯ
    if(m_PendingUploadCount>0){
		if(m_PendingUploadCount>5)
		{
			//����5��������ȡ5��
			getLogByCount(5);
			m_PendingUploadCount -= 5;
			this->uploadLogFinish = false;
		}else{//5��֮��
			//�������Ƶ�������
			getLogByCount(m_PendingUploadCount);
			m_PendingUploadCount = 0;//�������
			uploadLogFinish = true;
		}
        //printf("a3............................\n");
        //�����ϴ���־
        CUploadDeviceLogAck *uploadack =  this->genUploadAckFromLog(&this->m_UploadLogQueue);
        //printf("a4............................\n");
        if(uploadack!=NULL)
        {//������־��������־ʱ�������Ƿ�ɹ���ֻ��һ�Ρ�
            printf("send upload log ack..............................!\n");
            //printf("uploadack is: \n");
            //HbFrame_Operation::printMessage(uploadack);
            if(g_pMainApplication->getSerial_Comm()->immediateSendMessage(uploadack) ==false)
            {
                printf("send upload log ack fail!\n");
            }
            delete uploadack;
            uploadack=NULL;
        }else
        {
            printf("uploadack is null!\n");
        }
    }
    //////////////////////////////////////////////////////////////////////////////////
//    //��ʱ�����־�������糬�����ޣ���ɾ�����ȵ���־
//    llrp_u64_t log_currentutc = this->getCurrentUTC();
//    if(log_currentutc-m_LastClearLogUTC>=CLEARLOG_TIMER*1000000){

//        m_LastClearLogUTC = log_currentutc;

//        string logpath = WORK_DIR;
//        logpath = logpath + LOG_PATH;
//        logpath.erase(logpath.length()-1);
//        this->clearLogEmptyDir((char *)logpath.c_str());
//        vector<string> logfiles;
//        this->findLogFiles(&logfiles,DEALTYPE_SORT);
//        int filecount = logfiles.size();
//        printf("check log file count,count =%d..............................................!\n",filecount);
//        if(filecount>=MAX_LOGCOUNT){
//            printf("delete %d log files!\n",CLEARLOG_COUNT);
//            if(this->deleteLogByCount(CLEARLOG_COUNT)){
//                printf("delete %d logfiles success!\n",CLEARLOG_COUNT);
//            }else{
//                printf("delete %d logfiles fail!\n",CLEARLOG_COUNT);
//            }
//        }
//    }
    ///////////////////////////////////////////////////////////////////////////////////
}


#if 0
////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Log_Process::dealLogFile
 * @param pFiles
 * @param filestr��Ҫ���������ȫ·����
 * @param dealtype
 */
void Log_Process::dealLogFile(vector<string> *pFiles,char *filestr,int dealtype){
}

int Log_Process::findLogFiles(vector<string> *pfoundfiles,int dealtype){
}

/**
 * @brief Log_Process::findLogFiles
 * @param basePath��Ŀ¼��Ҫ�����Ҫ�С�/��
 * @param dealtype,�Բ��ҵ����ļ��Ĵ�������
 * @return������-1��ʾʧ�ܣ�0��ʾ�ɹ�
 * �ҵ�Ŀ¼��������־�ļ�
 */
int Log_Process::findLogFiles(char *basePath,vector<string> *pfoundfiles,int dealtype)
{
}
////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Log_Process::clearLogEmptyDir
 * @param basePath
 * @param pfoundfiles
 * @param dealtype
 * @return
 * ���
 */
int Log_Process::clearLogEmptyDir(char *basePath)
{
}



void Log_Process::logFileSort(vector<string> *pfoundfiles){
    if(pfoundfiles->size()==0){
        return;
    }
    string tmp;
    for(int i=0; i<pfoundfiles->size()-1; i++)
    {
        tmp = pfoundfiles->at(i);

        for(int j=pfoundfiles->size()-1; j>i; j--)
        {
            //�ҵ���������С������������
            if(tmp > pfoundfiles->at(j))
            {
                pfoundfiles->at(i) = pfoundfiles->at(j);
                pfoundfiles->at(j) = tmp;
                tmp = pfoundfiles->at(i);
            }
        }
    }
}

void Log_Process::logFileReverse(vector<string> *pfoundfiles)
{
    if(pfoundfiles->size()==0){
        return;
    }
    string tmp;
    for(int i=0; i<pfoundfiles->size()-1; i++)
    {
        tmp = pfoundfiles->at(i);

        for(int j=pfoundfiles->size()-1; j>i; j--)
        {
            //�ҵ���������������������
            if(tmp < pfoundfiles->at(j))
            {
                pfoundfiles->at(i) = pfoundfiles->at(j);
                pfoundfiles->at(j) = tmp;
                tmp = pfoundfiles->at(i);
            }
        }
    }
}

bool Log_Process::addSecureLog(int type, int code, string descinfo)
{/*
*@brief    :  ����ȫģ��ι��  һֱ����  �ƻ�  һ���Ӽ�¼һ��
*@param :
*@return :
->Damon :
*/
    time_t timep;
    time (&timep);

    if(timep - m_last_record_secure_logtime > 60){
        m_last_record_secure_logtime = timep;
        return addLog(getCurrentUTC(),type,code,descinfo);
}else{
// �����ϴλ���δ��1����
return true;
}
    return true;

}
#endif