/**
*****************************************************************************
* @文  件： mod_logSysInfo.h 
* @作  者： 00Jackey
* @版  本： V1.0.0
* @日  期： 5-Jun-2018
* @描  述： 日志信息系统接口文件
******************************************************************************
* @修改记录：
*   2018/06/05：初始版本
*    
*
******************************************************************************
**/

#ifndef _MOD_LOGSYSINFO_H_
#define _MOD_LOGSYSINFO_H_



//C库
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
	extern "C" {
#endif
//宏定义
#define LOG_INDEX_TOTAL        	   (uint32_t)(15)
#define EACH_INDEX_SPACE           (uint32_t)(16)

#define LOG_CONTENT_TOTAL          (uint32_t)(48)
#define EACH_LOG_SPACE             (uint32_t)(64)

#define LOG_INFO_TOTAL_CNT         (uint32_t)(4096)

#define LOG_WRITE_FLAG             (uint8_t)(0xA5)
		

//定义枚举
typedef enum {
    LOG_NOT_USE = 0, 
    LOG_BE_WRITE = 1, 
    LOG_BE_READ = 2
}LOG_OPERATE_STATE_ENUM;

typedef enum{
	LOG_PROC_SUCCEED = 0,
	LOG_PROC_FAILED = 1
}LOG_PROC_STATE_ENUM;

//定义结构体
typedef struct{
	uint8_t dateArry[LOG_CONTENT_TOTAL];
	uint32_t curNum;//4 日志序号
	uint32_t logcode;// 4 日志码
	uint32_t curTime;//4 时间戳
	uint8_t length; //2 日志描述信息长度
	uint8_t logtype;//1 日志类型
//	uint8_t  verifyVal;// 
	uint8_t  writeFlag;//1 是否写入日志标志位
}LOG_CONTENT_STRUCT;

//定义联合体
typedef union{
	LOG_CONTENT_STRUCT ContentStruct;
	uint8_t ConentArry[EACH_LOG_SPACE];
}LOG_CONTENT_UNION;//公用一块64字节的内存空间

//定义结构体
typedef struct{
	uint32_t curIndex;
	uint32_t cntTotal;
    uint32_t rawTotal;
	uint8_t  useless[3]; 	
	uint8_t  writeFlag;
}LOG_INDEX_STRUCT;

//定义联合体
typedef union{
	LOG_INDEX_STRUCT IndexStruct;
	uint8_t IndexArry[LOG_INDEX_TOTAL+1];
}LOG_INDEX_UNION;

//函数封装
typedef struct
{
	LOG_PROC_STATE_ENUM (*init)(void);
	LOG_PROC_STATE_ENUM (*record)(LOG_CONTENT_UNION uLogInfoUnion);
	LOG_PROC_STATE_ENUM (*getRecent)(LOG_CONTENT_UNION* pLogInfoUnion, int32_t recentNum);
    LOG_PROC_STATE_ENUM (*getSerial)(LOG_CONTENT_UNION* pLogInfoUnion, int32_t serialNum);
}LOG_SYS_CONTENT_STRUCT;

typedef struct
{
	LOG_PROC_STATE_ENUM (*clr)(void);
	LOG_PROC_STATE_ENUM (*set)(LOG_CONTENT_UNION  uLogInfoUnion);
	LOG_PROC_STATE_ENUM (*get)(LOG_CONTENT_UNION* pLogInfoUnion);
}LOG_SYS_OUTAGE_STRUCT;

typedef struct
{
	LOG_OPERATE_STATE_ENUM   LogOperateFlag;  //日志读、写、空闲状态

	LOG_SYS_OUTAGE_STRUCT 	 OutageStruct;  //断电日志封装
	LOG_SYS_CONTENT_STRUCT   ContentStruct; //主日志封装

	LOG_PROC_STATE_ENUM (*init)(void);  //日志系统初始化
	
	uint32_t (*getRawTotal)(void);  //获取日志数量
}LOG_SYS_INFO_STRUCT;

extern LOG_INDEX_UNION sLogIndexUnion;
//外部调用
extern LOG_SYS_INFO_STRUCT LogSysInfoStruct;
void LogSysInfo_ContentPrint(LOG_CONTENT_UNION LogContent);

#ifdef __cplusplus
	}
#endif

#endif

