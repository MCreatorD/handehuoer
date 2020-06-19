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
    m_last_position  = 0;  //当前文件的查询位置为0
    m_now_position = 0;
	m_PendingUploadCount = 0;
	printf("1. 日志系统初始化\n");
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
 * @return ，返回的是一个由内核自动分配的内存空间指针，不用去手动释放它
 */
struct tm * Log_Process::convertTimeTToTm(time_t t){
}
time_t Log_Process::convertTmToTimeT(struct tm *t){
}

unsigned long long Log_Process::getCurrentUTC()
{
	
}




//字符串分割函数
vector<string> Log_Process::splitString(string str,string pattern)
{
    string::size_type pos;
    vector<string> result;
    str+=pattern;//扩展字符串以方便操作
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
	//实例化出 上传日志 响应消息 消息类型641
    pack  = (CUploadDeviceLogAck*)HbFrame_Operation::genMessage(UploadDeviceLogAck,0);
    //pack->setSequenceId(stoi(splitstr.at(0)));
    pack->setSequenceId(0);//日志序号
    pack->setStatus(pstatus);//应答状态参数 以实例化了
    while(pLogs->size()>0){
        LOG_CONTENT_UNION logstr = pLogs->front();
        pLogs->pop_front();
        //printf("logstr is %s\n",logstr.c_str());
//        vector<string> splitstr = splitString(logstr,"|");
//        if(splitstr.size()<6){
//            //printf("error log...............\n");
//            continue;
//        }
		//实例化 参数 设备日志参数 参数类型640
        CDeviceLog *pdevicelog = (CDeviceLog*)HbFrame_Operation::genParam(DeviceLog,0,NULL);
        //pdevicelog->setLogSequence(stoi(splitstr.at(0)));
        //printf("d0.............................................\n");
		pdevicelog->setLogSequence(logstr.ContentStruct.curNum);//u64 序列号
        //pdevicelog->setLogSequence(atoi(splitstr.at(0).c_str()));//u64 序列号
        //printf("d1.............................................\n");
        //pdevicelog->setLogType(stoi(splitstr.at(1)));
		pdevicelog->setLogType(logstr.ContentStruct.logtype);//u8 日志类型 0:设备故障 1: 故障恢复 2:运行记录
        //pdevicelog->setLogType(atoi(splitstr.at(1).c_str()));//u8 日志类型 0:设备故障 1: 故障恢复 2:运行记录
        //printf("d2.............................................\n");
        //pdevicelog->setLogCode(stoi(splitstr.at(2)));
		pdevicelog->setLogCode(logstr.ContentStruct.logcode); //u32 日志码
        //pdevicelog->setLogCode(atoi(splitstr.at(2).c_str())); //u32 日志码 stoi函数对于过大的值会崩溃，故改用 atoi
        CUTCTimestamp *putc = (CUTCTimestamp*)HbFrame_Operation::genParam(UTCTimestamp,0,NULL);//时间戳
        //printf("d3.............................................\n");
        putc->setMicroseconds(logstr.ContentStruct.curTime);
        pdevicelog->setUTCTimestamp(putc);
		  
		//日志描述信息
        //string temp = splitstr.at(5);
        //printf("temp.lenght=%d\n",temp.length());
        llrp_utf8v_t des = llrp_utf8v_t(logstr.ContentStruct.length);
		memcpy(des.m_pValue,logstr.ContentStruct.dateArry,des.m_nValue);
        //memcpy(des.m_pValue,splitstr.at(5).c_str(),des.m_nValue);
        pdevicelog->setLogDescInfo(des);
		//添加参数到消息里面
        pack->addDeviceLog(pdevicelog);
    }//end of while
    deque<LOG_CONTENT_UNION>().swap(*pLogs);


    if(uploadLogFinish==true){ //设置是否上传结束标志
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
 * @param descinfo，日志的内容，注意，”|“和换行，回车符为保留字符，不能使用
 * @return
 * 添加日志，日志以每日一个目录，每小时一个文件来进行存贮。
 * 最多可充许存一个月的日期，超过，则删除
 * 单片机实现日志记录存储时间和序号
 * 最多存储4096条循环覆盖
 */
bool Log_Process::addLog(unsigned long long utctime, int type, int code, string descinfo){
	printf("addLog:");
	LOG_CONTENT_UNION test; //带写入的日志内容
	test.ContentStruct.curTime = utctime;
	test.ContentStruct.logtype = type;
	test.ContentStruct.logcode = code;
	test.ContentStruct.length = descinfo.length();
	memcpy(test.ContentStruct.dateArry,descinfo.c_str(),test.ContentStruct.length);//描述拷贝
	test.ContentStruct.writeFlag = 0xA5;
	
	//将记录存储到非易失性存储器 暂时使用的是w25q256的Qspi flash 
	if(LOG_PROC_SUCCEED == LogSysInfoStruct.ContentStruct.record(test))
	{
		printf("sLogIndexUnion.IndexStruct.cntTotal:%d\n",sLogIndexUnion.IndexStruct.cntTotal);
		printf("记录成功\r\n");
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
 * 按条数删除最先的几个日志
 */
bool Log_Process::deleteLogByCount(int count){

}

/**
 * @brief Log_Process::getLogByTime
 * @param utc_start
 * @param utc_end
 * @param max_count，充许查询到的最大日志数
 * @return,返回查询结束时的起始utc时间，如果第于0表示查询失败，大于第于结束时间，则表示查询结束，其它表示查询未结束
 * 由起始时间获取日志
 */
unsigned long long Log_Process::getLogByTime(unsigned long long utc_start,unsigned long long utc_end)
{

}


int Log_Process::getLogByCount(int count)
{
	LOG_CONTENT_UNION test1;//写入读取
	uint8_t tmp = count;
	
	while(count--)
	{
		LogSysInfoStruct.ContentStruct.getSerial(&test1,m_Logcount-count);
		//打印读取到的数据内容
		LogSysInfo_ContentPrint(test1);
		//放入待发送的日志队列
        this->m_UploadLogQueue.push_front(test1);   ///采用头插法
	}
	m_Logcount -= tmp;
}

int Log_Process::dealMessage(CMessage *pMessage, Base_Comm *pComm)
{
    if ( pMessage == NULL ) { return -1; }

	llrp_u32_t messageid = pMessage->getMessageID();
    CMessage *pack = NULL;
    CStatus *pstatus = NULL;

    //根据不同消息处理
    switch(pMessage->m_pType->m_TypeNum){
    case UploadDeviceLog: //上传设备日志请求
    {
        //m_CurrentCommID = commID;
        //上传类型
        printf("uploadDeviceLog type is %d\n",((CUploadDeviceLog*)pMessage)->getUploadType());
        switch (((CUploadDeviceLog*)pMessage)->getUploadType()) {
        case UploadDeviceLogType_SERIAL_NUMBER: //按日志条数上传
        {
            int count = ((CUploadDeviceLog*)pMessage)->getUploadCount();
			m_Logcount = sLogIndexUnion.IndexStruct.cntTotal;//记录下当前的日志总数
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
        case UploadDeviceLogType_SERIAL_TIME: //按时间上传
        {
            llrp_u64_t starttime = ((CUploadDeviceLog*)pMessage)->getStartLogTimestamp()->getMicroseconds();
            llrp_u64_t endtime = ((CUploadDeviceLog*)pMessage)->getEndLogTimestamp()->getMicroseconds();
            m_CurrentUTCStrart = starttime;
            m_CurrentUTCEnd = endtime;

            break;
        }
        case UploadDeviceLogType_FIX_TIME: //定时上传
        {
            /*ClearMessageVector();
            mRegularTime = ((CUploadDeviceLog*)pInMessage)->getMilliseconds()*1000;
            if( mRegularTime != 0 )
            {
                mIsTiming = true; //定时标识
                mStartUTCTime = mpDeviceLog->GetCurrentUTC(); //记录开始的UTC时间
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
            return 0; //已处理
            break;
        }
        break;
    }
    case UploadDeviceLogConfirm: //设备日志上传应答确认
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
            //无返回
        }*/
        break;
    }
    case ClearDeviceLog: //清空设备日志
    {
//        string logdir = WORK_DIR;
//        logdir = logdir + LOG_PATH;
//        string cmdstr = "rm -r -f ";
//        cmdstr = cmdstr + logdir;
//        //printf("cmdstr is %s\n",cmdstr.c_str());
//        system(cmdstr.c_str());
		//待实现清除日志的 方案一 清除索引 方案二 清除索引和清除数据
		
		
        pstatus = (CStatus*)HbFrame_Operation::genParam(Status,0,NULL);
        pstatus->setStatusCode(StatusCode_M_Success);
        pack = HbFrame_Operation::genMessage(ClearDeviceLogAck, 0);
        HbFrame_Operation::addParamToMessage(pstatus, pack);
        break;
    }
    case DeviceLogCount: //设备日志数量查询
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
        return -1; //未处理
        break;
    }

    if( pack != NULL ) //消息不为空，发送消息
    {
        printf("send log message...................\n");
        //HbFrame_Operation::printMessage(retPack);
        if(pComm->immediateSendMessage(pack) == true) //消息发送
        {
            printf("send success!\n");
        }
        else //发送失败
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
    //定时上传
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
    //按时间查询，只要起始时间小于结束时间，即为符合条件的日志
//    if(m_CurrentUTCStrart<m_CurrentUTCEnd){
//        m_CurrentUTCStrart = this->getLogByTime(m_CurrentUTCStrart,m_CurrentUTCEnd);
//        //        cout<<"m_CurrentUTCStrart ------->"<<m_CurrentUTCStrart<<endl;
//        //        cout<<"m_CurrentUTCEnd ------->"<<m_CurrentUTCEnd<<endl;
//        //        if(m_CurrentUTCStrart>=m_CurrentUTCEnd){   // 时间和分包不能兼得
//        //            uploadLogFinish = true;
//        //        }else{
//        //            uploadLogFinish = false;
//        //        }
//        //以下上传日志
//        //cout<<"m_UploadLogQueue size ----------->"<<m_UploadLogQueue.size()<<endl;
//        CUploadDeviceLogAck *uploadack =  this->genUploadAckFromLog(&this->m_UploadLogQueue);
//        if(uploadack!=NULL){//发送日志，发送日志时，不管是否成功，只发一次。
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
    //按条数查询
    if(m_PendingUploadCount>0){
		if(m_PendingUploadCount>5)
		{
			//大于5条连续读取5条
			getLogByCount(5);
			m_PendingUploadCount -= 5;
			this->uploadLogFinish = false;
		}else{//5条之内
			//按条数推到容器中
			getLogByCount(m_PendingUploadCount);
			m_PendingUploadCount = 0;//传输完成
			uploadLogFinish = true;
		}
        //printf("a3............................\n");
        //以下上传日志
        CUploadDeviceLogAck *uploadack =  this->genUploadAckFromLog(&this->m_UploadLogQueue);
        //printf("a4............................\n");
        if(uploadack!=NULL)
        {//发送日志，发送日志时，不管是否成功，只发一次。
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
//    //定时检查日志条数，如超过上限，则删除最先的日志
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
 * @param filestr，要求输入的是全路径名
 * @param dealtype
 */
void Log_Process::dealLogFile(vector<string> *pFiles,char *filestr,int dealtype){
}

int Log_Process::findLogFiles(vector<string> *pfoundfiles,int dealtype){
}

/**
 * @brief Log_Process::findLogFiles
 * @param basePath，目录，要求最后不要有“/”
 * @param dealtype,对查找到的文件的处理类型
 * @return，返回-1表示失败，0表示成功
 * 找到目录下所有日志文件
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
 * 清除
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
            //找到数组中最小的数，并交换
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
            //找到数组中最大的数，并交换
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
*@brief    :  处理安全模块喂插  一直报错  计划  一分钟记录一次
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
// 距离上次击落未满1分钟
return true;
}
    return true;

}
#endif