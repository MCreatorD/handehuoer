/**
*****************************************************************************
* @文  件： mod_logSysInfo.c 
* @作  者： 00Jackey
* @版  本： V1.0.0
* @日  期： 6-Jun-2018
* @描  述： 日志信息系统主文件
******************************************************************************
* @修改记录：
*   2018/06/06：初始版本，待通信完成后做完整性测试
*   2018/07/01：做完完整性测试，改了一些问题 
*
******************************************************************************
**/

//接口头文件
#include "mod_logSysInfo.h"

//硬件驱动
#include "EN25Q256.h"
#include <stdio.h>
#include "rtc.h"
#include <time.h>
/**
 * @brief General_Functions::getCurrentUTC
 * @return
 * 返回当前UTC时间。单位是毫秒
 */
unsigned long long getCurrentUTC()
{
    /*struct timeval nowTime;
    gettimeofday(&nowTime, NULL);
    unsigned long long utc = (unsigned long long)nowTime.tv_sec * 1000000 + (unsigned long long)nowTime.tv_usec;
    return utc;*/
	
	RTC_TimeTypeDef time_struct;
	RTC_GetTime(RTC_Format_BIN, &time_struct);
	RTC_DateTypeDef date_struct;
	RTC_GetDate(RTC_Format_BIN, &date_struct);
	
	//printf("y=%d,m=%d,d=%d,h=%d,m=%d,s=%d\n",date_struct.RTC_Year,date_struct.RTC_Month,date_struct.RTC_Date,
	//time_struct.RTC_Hours,time_struct.RTC_Minutes,time_struct.RTC_Seconds);
	
	struct tm *gm_date;
	time_t utc =0;
	gm_date = localtime(&utc);//1970 0 1 000,此句必须有，否则会死机
	gm_date->tm_year = date_struct.RTC_Year + 100;
	gm_date->tm_mon = date_struct.RTC_Month - 1;
	gm_date->tm_mday = date_struct.RTC_Date;
	gm_date->tm_wday = date_struct.RTC_WeekDay;
	gm_date->tm_hour = time_struct.RTC_Hours;
	gm_date->tm_min = time_struct.RTC_Minutes;
	gm_date->tm_sec = time_struct.RTC_Seconds;
	
	utc = mktime(gm_date);
	//printf("unix timestamp:%d \r\n",utc);
	unsigned long long lutc = (unsigned long long)utc*1000; //llrp中的UTC是精确到毫秒的
	return lutc;
}



uint32_t RTC_GetCounter(void){
	return getCurrentUTC()/1000;
	//return 20190307;
}
//void W25qxx_eraseOneSector(uint32_t addr){}
//void W25qxx_readBuffer(char *buf,uint32_t addr,uint32_t size){}
//void W25qxx_writeBuffer(char *buf,uint32_t addr,uint32_t size){}	


//宏定义
#define CHIPSET_PAGE               (uint32_t)256
#define CHIPSET_SECTOR_SIZE		   (uint32_t)(CHIPSET_PAGE * 16)
#define CHIPSET_BLOCK_SIZE		   (uint32_t)(CHIPSET_SECTOR_SIZE * 16)
#define CHIPSET_TOTAL_SIZE		   (uint32_t)(CHIPSET_BLOCK_SIZE * 128)

#define LOG_SYS_OUTAGE_ADDRESS     (uint32_t)(CHIPSET_SECTOR_SIZE * 13)
//#define LOG_SYS_INDEX_ADDRESS      (uint32_t)(CHIPSET_SECTOR_SIZE * 14)
//#define LOG_SYS_INFO_ADDRESS       (uint32_t)(CHIPSET_SECTOR_SIZE * 16)

#define LOG_SYS_INFO_TOAL_SIZE     (uint32_t)(LOG_INFO_TOTAL_CNT * EACH_LOG_SPACE)

#define LOG_SYS_INDEX_TOAL_SIZE    (uint32_t)(CHIPSET_SECTOR_SIZE * 2)

//#define LOG_SYS_WRITE				W25qxx_writeBuffer
//#define LOG_SYS_READ				W25qxx_readBuffer
//#define LOG_SYS_ERASE				W25qxx_eraseOneSector

#define LOG_SYS_WRITE				EN25Q256_Write
#define LOG_SYS_READ				EN25Q256_Read
#define LOG_SYS_ERASE				EN25Q256_Erase_Sector


#define LOG_INDEX_ADDR  			sLogIndexAddr


//静态函数
static LOG_PROC_STATE_ENUM LogSysInfo_initInfo(void);
static LOG_PROC_STATE_ENUM LogSysInfo_recordInfo(LOG_CONTENT_UNION uLogContentUnion);
static LOG_PROC_STATE_ENUM LogSysInfo_getRecentInfo(LOG_CONTENT_UNION* pLogContentUnion, int32_t recentNum);
static LOG_PROC_STATE_ENUM LogSysInfo_getSerialInfo(LOG_CONTENT_UNION* pLogContentUnion, int32_t serialNum);

static LOG_PROC_STATE_ENUM LogSysInfo_initIndex(void);
static LOG_PROC_STATE_ENUM LogSysInfo_setIndex(LOG_INDEX_UNION uLogIndexUnion);
static LOG_PROC_STATE_ENUM LogSysInfo_getIndex(LOG_INDEX_UNION* pLogIndexUnion);

static LOG_PROC_STATE_ENUM LogSysInfo_clrOutage(void);
static LOG_PROC_STATE_ENUM LogSysInfo_setOutage(LOG_CONTENT_UNION  uLogContentUnion);
static LOG_PROC_STATE_ENUM LogSysInfo_getOutage(LOG_CONTENT_UNION* pLogContentUnion);

static LOG_PROC_STATE_ENUM LogSysInfo_init(void);

static uint32_t LogSysInfo_getRawTotal(void);

static uint8_t LogSysInfo_calcXor8(uint8_t *pVarArry, uint8_t len);

static void LogSysInfo_IndexPrint(LOG_INDEX_UNION LogIndex);
// void LogSysInfo_ContentPrint(LOG_CONTENT_UNION LogContent);
//常量
const LOG_INDEX_UNION  cDefaultLogIndexUnion = {

	.IndexStruct.curIndex = (LOG_SYS_INFO_ADDRESS - EACH_LOG_SPACE),
    .IndexStruct.cntTotal = 0,
	.IndexStruct.rawTotal = 0,
    .IndexStruct.writeFlag = LOG_WRITE_FLAG,
};

//静态变量
LOG_INDEX_UNION sLogIndexUnion;
uint32_t sLogIndexAddr;

//全局变量
LOG_SYS_INFO_STRUCT LogSysInfoStruct = {
	//LOG_SYS_CONTENT_STRUCT   ContentStruct; //主日志封装
	.ContentStruct.init = LogSysInfo_initInfo,//日志系统读取掉电日志到正常日志中 返回执行状态
    .ContentStruct.record = LogSysInfo_recordInfo,//记录一条日志信息，索引更新          	   ,返回执行状态
    .ContentStruct.getRecent = LogSysInfo_getRecentInfo,//获取最近某一条日志信息
    .ContentStruct.getSerial = LogSysInfo_getSerialInfo,//获取指定序号的日志

	//LOG_SYS_OUTAGE_STRUCT 	 OutageStruct;  //断电日志封装
    .OutageStruct.clr = LogSysInfo_clrOutage, //清楚掉电区日志数据
    .OutageStruct.set = LogSysInfo_setOutage, //存储掉电数据
    .OutageStruct.get = LogSysInfo_getOutage, // 获取掉电数据

	//LOG_PROC_STATE_ENUM (*init)(void);  //日志系统初始化
    .init = LogSysInfo_init, //日志系统初始化

	//uint32_t (*getRawTotal)(void);  //获取日志数量
	.getRawTotal = LogSysInfo_getRawTotal,//获取日志中所存储的日志数量
};



/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_init
*	功能说明: 日志系统初始化
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_init(void)
{
	if(LOG_PROC_FAILED == LogSysInfo_initIndex())
	{
		LogSysInfo_IndexPrint(sLogIndexUnion);
		return LOG_PROC_FAILED;
	}
	else
	{
		LogSysInfo_IndexPrint(sLogIndexUnion);
		return LOG_PROC_SUCCEED;	
	}		

}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_getRawTotal
*	功能说明: 获取日志中所存储的日志数量
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
uint32_t LogSysInfo_getRawTotal(void)
{
	return sLogIndexUnion.IndexStruct.rawTotal;
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_initIndex
*	功能说明: 日志索引初始化
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_initIndex(void)
{
	//查找日志系统记录的数量
    LOG_PROC_STATE_ENUM rState;
	LOG_INDEX_UNION tLogIndexUnion;
	int32_t i_reord = -1;

	/* 遍历第一个块，判断每个位置是否都有写入标记*/
	for(int32_t i = 0; i < 256; i++){
		LOG_SYS_READ(tLogIndexUnion.IndexArry,LOG_SYS_INDEX_ADDRESS+(i*EACH_INDEX_SPACE),sizeof(LOG_INDEX_UNION));
		if(tLogIndexUnion.IndexArry[LOG_INDEX_TOTAL] == LOG_WRITE_FLAG){//16个字节，最后一个位置
			i_reord = i;//第一个块 记录有标记的位置从头遍历记录最后一个 查找最后一个记录的地方
		}else{
			break;
		}
	}
	/* 如果255个位置中不全是有标志的，则提前当前位置的日志索引 */
	if((i_reord != 255)&&(i_reord != -1)){
		//以当前的i_reord * EACH_INDEX_SPACE 得到起始地址 赋值给 LOG_INDEX_ADDR(全局变量uint32_t sLogIndexAddr)
		LOG_INDEX_ADDR =  LOG_SYS_INDEX_ADDRESS + (i_reord * EACH_INDEX_SPACE);//前面都是有标记的且内存空间没有装满
        LOG_SYS_READ(tLogIndexUnion.IndexArry,LOG_INDEX_ADDR,sizeof(LOG_INDEX_UNION));//读出16个字节索引数据
		memcpy(sLogIndexUnion.IndexArry,tLogIndexUnion.IndexArry,sizeof(LOG_INDEX_UNION));//拷贝起始有标记索引 到（全局变量）LOG_INDEX_UNION sLogIndexUnion
		LOG_SYS_ERASE((LOG_SYS_INDEX_ADDRESS + CHIPSET_SECTOR_SIZE)/4096);//第一块4K 一个扇区找到 格式化第二个扇区
		return LOG_PROC_SUCCEED;//在第一块就找到空的位置 则记录位置LOG_INDEX_ADDR 及时退出
	}
	/* 遍历第二个块，判断每个位置是否都有写入标记*/
	for(int32_t i = 256; i < 512; i++){
		LOG_SYS_READ(tLogIndexUnion.IndexArry,LOG_SYS_INDEX_ADDRESS+(i*EACH_INDEX_SPACE),sizeof(LOG_INDEX_UNION));
		if(tLogIndexUnion.IndexArry[LOG_INDEX_TOTAL] == LOG_WRITE_FLAG){
			i_reord = i;
		}else{
			break;
		}
	}
	/* 如果255个位置中不全是有标志的，则提前当前位置的日志索引 */
	if((i_reord != 511)&&(i_reord != -1)){
		LOG_INDEX_ADDR =  LOG_SYS_INDEX_ADDRESS + (i_reord * EACH_INDEX_SPACE);
        LOG_SYS_READ(tLogIndexUnion.IndexArry,LOG_INDEX_ADDR,sizeof(LOG_INDEX_UNION));
		memcpy(sLogIndexUnion.IndexArry,tLogIndexUnion.IndexArry,sizeof(LOG_INDEX_UNION));
		LOG_SYS_ERASE(LOG_SYS_INDEX_ADDRESS/4096);
		return LOG_PROC_SUCCEED;
	}
	/* 如果两个块中都没有标记，则写入默认值 */
	if(i_reord == -1){
		//LOG_INDEX_ADDR =  LOG_SYS_INDEX_ADDRESS;
		LOG_SYS_WRITE((uint8_t*)cDefaultLogIndexUnion.IndexArry,LOG_SYS_INDEX_ADDRESS,sizeof(LOG_INDEX_UNION));
		rState = LogSysInfo_getIndex(&tLogIndexUnion);
		if(rState == LOG_PROC_FAILED){
			return LOG_PROC_FAILED;
		}else{
			memcpy(sLogIndexUnion.IndexArry,tLogIndexUnion.IndexArry,sizeof(LOG_INDEX_UNION));
			return LOG_PROC_SUCCEED;
		}
	}
	/* 所有512个位置都有标记，则设定最后一个为索引存储位置 */
	LOG_INDEX_ADDR = LOG_SYS_INDEX_ADDRESS + (511 * EACH_INDEX_SPACE);
	//所有512个存储空间都有数据则设定最后一个为索引位置，同是更新倒数第二个为当前存储情况 
	memcpy(sLogIndexUnion.IndexArry,tLogIndexUnion.IndexArry,sizeof(LOG_INDEX_UNION));
	LOG_SYS_ERASE(LOG_SYS_INDEX_ADDRESS/4096);

	return LOG_PROC_SUCCEED;

}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_setIndex
*	功能说明: 存储日志索引
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_setIndex(LOG_INDEX_UNION uLogIndexUnion)
{
	//memcpy(sLogIndexUnion.IndexArry,uLogIndexUnion.IndexArry,sizeof(LOG_INDEX_UNION));
	LOG_INDEX_ADDR = LOG_INDEX_ADDR + EACH_INDEX_SPACE;
	if(LOG_INDEX_ADDR >= (LOG_SYS_INDEX_ADDRESS + LOG_SYS_INDEX_TOAL_SIZE)){
		LOG_INDEX_ADDR = LOG_SYS_INDEX_ADDRESS;
		LOG_SYS_ERASE(LOG_SYS_INDEX_ADDRESS/4096);//从头写擦除第一处扇区
		LOG_SYS_ERASE((LOG_SYS_INDEX_ADDRESS + CHIPSET_SECTOR_SIZE)/4096);//第一块4K 一个扇区找到 格式化第二个扇区
		LOG_SYS_WRITE(uLogIndexUnion.IndexArry,LOG_INDEX_ADDR,sizeof(LOG_INDEX_UNION));
	}else{
		LOG_SYS_WRITE(uLogIndexUnion.IndexArry,LOG_INDEX_ADDR,sizeof(LOG_INDEX_UNION));
	}
	
	return LOG_PROC_SUCCEED;
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_getIndex
*	功能说明: 读取日志索引
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_getIndex(LOG_INDEX_UNION* pLogIndexUnion)
{
	LOG_SYS_READ(pLogIndexUnion->IndexArry,LOG_INDEX_ADDR,sizeof(LOG_INDEX_UNION));
	if(pLogIndexUnion->IndexArry[LOG_INDEX_TOTAL] != LOG_WRITE_FLAG){
		return LOG_PROC_FAILED;
	}else{
		return LOG_PROC_SUCCEED;
	}
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_initInfo
*	功能说明: 日志系统读取掉电日志到正常日志中
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_initInfo(void)
{
	LOG_INDEX_UNION tLogIndexUnion;
	LOG_CONTENT_UNION  tLogOutageUnion;

	//1st: get the SYS_LOG_INDEX_ADDRESS page 
	LogSysInfo_getIndex(&tLogIndexUnion);
	//2nd: get the powerdown logging
	LogSysInfo_getOutage(&tLogOutageUnion);
	//3rd: write the log at the syslog
	if(tLogOutageUnion.ContentStruct.writeFlag == LOG_WRITE_FLAG){
		LogSysInfo_recordInfo(tLogOutageUnion);
	}else{
		/* warnning  --------------------*/
	}
	//4th: erase the power outage buff page
	LogSysInfo_clrOutage();
	
	return LOG_PROC_SUCCEED;
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_recordInfo
*	功能说明: 记录一条日志信息，索引更新
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_recordInfo(LOG_CONTENT_UNION uLogContentUnion)
{

	/*1st: According to index calculate the adress */
	sLogIndexUnion.IndexStruct.cntTotal += 1;
//	sLogIndexUnion.IndexStruct.rawTotal = (sLogIndexUnion.IndexStruct.cntTotal >= LOG_INFO_TOTAL_CNT) ?\
//		LOG_INFO_TOTAL_CNT : sLogIndexUnion.IndexStruct.cntTotal;
	sLogIndexUnion.IndexStruct.rawTotal = sLogIndexUnion.IndexStruct.cntTotal%LOG_INFO_TOTAL_CNT;
	sLogIndexUnion.IndexStruct.curIndex = (sLogIndexUnion.IndexStruct.cntTotal - 1)%\
        LOG_INFO_TOTAL_CNT * EACH_LOG_SPACE + LOG_SYS_INFO_ADDRESS;
	if(sLogIndexUnion.IndexStruct.curIndex >= (LOG_SYS_INFO_TOAL_SIZE + LOG_SYS_INFO_ADDRESS))
		sLogIndexUnion.IndexStruct.curIndex = LOG_SYS_INFO_ADDRESS;//防止过界
	/* Earaze the sector , cut down the total log */
	//	if(0 == (sLogIndexUnion.IndexStruct.curIndex % CHIPSET_SECTOR_SIZE)){
	//		LOG_SYS_ERASE(sLogIndexUnion.IndexStruct.curIndex/4096);//写入地址整扇区 则直接擦除
	//		sLogIndexUnion.IndexStruct.rawTotal -= (CHIPSET_SECTOR_SIZE / EACH_LOG_SPACE);
	//	}
	/*2nd: record the log index to eeprom */
    sLogIndexUnion.IndexStruct.writeFlag = LOG_WRITE_FLAG;
	LogSysInfo_setIndex(sLogIndexUnion);//存储Index
	LogSysInfo_IndexPrint(sLogIndexUnion);
	/*3rd: get the log number */        //存储Log data
	uLogContentUnion.ContentStruct.curNum = sLogIndexUnion.IndexStruct.cntTotal;
	uLogContentUnion.ContentStruct.curTime = RTC_GetCounter();	//to update
	//uLogContentUnion.ContentStruct.verifyVal = LogSysInfo_calcXor8(uLogContentUnion.ConentArry,LOG_CONTENT_TOTAL);
	uLogContentUnion.ContentStruct.writeFlag = LOG_WRITE_FLAG;
    /*4th: record the loginfo */
	LOG_SYS_WRITE(uLogContentUnion.ConentArry,sLogIndexUnion.IndexStruct.curIndex,EACH_LOG_SPACE);

	return LOG_PROC_SUCCEED;
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_getRecentInfo
*	功能说明: 获取最近某一条日志信息
*	形    参: pLogContentUnion:日志信息指针 recentNum: 0表示最近的日志，(0~LOG_INFO_TOTAL_CNT-1)
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_getRecentInfo(LOG_CONTENT_UNION* pLogContentUnion, int32_t recentNum)
{
	int32_t tSysLogAddr = 0;

	/*Only more than need is ok*/
	if(sLogIndexUnion.IndexStruct.cntTotal <= recentNum){
		return LOG_PROC_FAILED;
	}
	
	/* Get the real index */
	tSysLogAddr = ((sLogIndexUnion.IndexStruct.cntTotal - recentNum - 1) % LOG_INFO_TOTAL_CNT) *\
		EACH_LOG_SPACE + LOG_SYS_INFO_ADDRESS;
	/* For protect , do not reach there */
	if(tSysLogAddr < LOG_SYS_INFO_ADDRESS){
		tSysLogAddr = LOG_SYS_INFO_TOAL_SIZE + LOG_SYS_INFO_ADDRESS - EACH_LOG_SPACE;
	}
	
	LOG_SYS_READ(pLogContentUnion->ConentArry,tSysLogAddr,EACH_LOG_SPACE);
	
	/* Judge the log info is right  */
	if(pLogContentUnion->ContentStruct.writeFlag == LOG_WRITE_FLAG)
		return LOG_PROC_SUCCEED;
	else
		return LOG_PROC_FAILED;
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_getSerialInfo
*	功能说明: 获取指定序号的日志
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_getSerialInfo(LOG_CONTENT_UNION* pLogContentUnion, int32_t serNum)
{
	int32_t tSysLogAddr = 0x000000;
	int32_t tSysLogNum = 0;

	/*Only more than need is ok*/
	if(serNum <= 0){
		return LOG_PROC_FAILED;
	}
	
	if(sLogIndexUnion.IndexStruct.cntTotal < serNum){
		return LOG_PROC_FAILED;
	}
	
	tSysLogNum = serNum % LOG_INFO_TOTAL_CNT;
	tSysLogAddr = (tSysLogNum -1) * EACH_LOG_SPACE + LOG_SYS_INFO_ADDRESS; 

	LOG_SYS_READ(pLogContentUnion->ConentArry,tSysLogAddr,EACH_LOG_SPACE);
	
	/* Judge the log info is right  */
	if(pLogContentUnion->ContentStruct.writeFlag == LOG_WRITE_FLAG)
		return LOG_PROC_SUCCEED;
	else
		return LOG_PROC_FAILED;

}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_clrLogOutage
*	功能说明: 清楚掉电区日志数据
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_clrOutage(void)
{
	uint8_t tBytesFill[EACH_LOG_SPACE]= {0};
	
	LOG_SYS_WRITE(tBytesFill,LOG_SYS_OUTAGE_ADDRESS,EACH_LOG_SPACE);
	
	return LOG_PROC_SUCCEED;
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_setLogOutage
*	功能说明: 存储掉电数据
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_setOutage(LOG_CONTENT_UNION uLogContentUnion)
{
	LOG_SYS_WRITE(uLogContentUnion.ConentArry,LOG_SYS_OUTAGE_ADDRESS,EACH_LOG_SPACE);
	
	return LOG_PROC_SUCCEED;
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_getSerLogInfo
*	功能说明: 获取掉电数据
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
LOG_PROC_STATE_ENUM LogSysInfo_getOutage(LOG_CONTENT_UNION* pLogContentUnion)
{
	LOG_SYS_READ(pLogContentUnion->ConentArry,LOG_SYS_OUTAGE_ADDRESS,EACH_LOG_SPACE);

	return LOG_PROC_SUCCEED;
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_calcXor8
*	功能说明: 计算校验值
*	形    参: pVarArry:需要校验的数组 len：数据长度
*	返 回 值: 执行状态
*********************************************************************************************************
*/
uint8_t LogSysInfo_calcXor8(uint8_t *pVarArry, uint8_t len)
{
	uint8_t rvalue;
	rvalue = pVarArry[0];

	for(uint8_t i = 1; i < len; i++){
		rvalue ^= pVarArry[i];
	}

	return rvalue;
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_IndexPrint
*	功能说明: 日志索引打印
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
static void LogSysInfo_IndexPrint(LOG_INDEX_UNION LogIndex)
{
//	printf("1. 日志索引打印具体存储地址 :%x\r\n",LogIndex.IndexStruct.curIndex);
//	printf("2. 日志索引打印具体消息序号 :%x\r\n",LogIndex.IndexStruct.cntTotal);
//	printf("3. 日志索引打印具体日志数量 :%x\r\n",LogIndex.IndexStruct.rawTotal);
//	printf("4. 日志索引打印具体写入标致 :%x\r\n",LogIndex.IndexStruct.writeFlag);
	printf("1. LogIndex.IndexStruct.curIndex :%d\r\n",LogIndex.IndexStruct.curIndex-LOG_SYS_INFO_ADDRESS);
	printf("2. LogIndex.IndexStruct.cntTotal :%d\r\n",LogIndex.IndexStruct.cntTotal);
	printf("3. LogIndex.IndexStruct.rawTotal :%d\r\n",LogIndex.IndexStruct.rawTotal);
	printf("4. LogIndex.IndexStruct.writeFlag :%x\r\n",LogIndex.IndexStruct.writeFlag);
}

/*
*********************************************************************************************************
*	函 数 名: LogSysInfo_ContentPrint
*	功能说明: 日志内容打印
*	形    参: 无
*	返 回 值: 执行状态
*********************************************************************************************************
*/
void LogSysInfo_ContentPrint(LOG_CONTENT_UNION LogContent)
{
//	printf("1. 日志索引打印具体存储地址 :%x\r\n",LogIndex.IndexStruct.curIndex);
//	printf("2. 日志索引打印具体消息序号 :%x\r\n",LogIndex.IndexStruct.cntTotal);
//	printf("3. 日志索引打印具体日志数量 :%x\r\n",LogIndex.IndexStruct.rawTotal);
//	printf("4. 日志索引打印具体写入标致 :%x\r\n",LogIndex.IndexStruct.writeFlag);
	printf("1. LogContent.ContentStruct.curTime :%d\r\n",LogContent.ContentStruct.curTime);
	printf("2. LogContent.ContentStruct.curNum :%d\r\n",LogContent.ContentStruct.curNum);
	printf("3. LogContent.ContentStruct.length :%d\r\n",LogContent.ContentStruct.length);
	printf("4. LogContent.ContentStruct.dateArry :\r\n");
	//打印数组内容
	for(int i=0;i<LogContent.ContentStruct.length;i++)
	{
		//printf("%c",*(LogContent.ContentStruct.dateArry+i));
		printf("%c",LogContent.ContentStruct.dateArry[i]);
	}
	printf("\r\n");
}

