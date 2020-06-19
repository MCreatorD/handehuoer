#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#include <deque>
using namespace std;

#include "general.h"
#include "HbFrame_Operation.h"
#include "runnable.h"
#include "serial_comm.h"
#include "operation_process.h"
#include "SPI_Communication.h"
#include "versionmanager.h"
#include "param_manager.h"
#include "rf_manager.h"
#include "rf_oper_base.h"
#include "MCUToSecureComm.h"
#include "Sm2Certification.h"
#include "IOCtrl.h"
#include "log_process.h"
#define DEVICE_NAME "HB__RFID"

#define DEFAULT_SERVERPORT 5084



/**
 * @brief The Main_Application class
 * by lijin 2017.02.28
 * 各功能模块的主控制类，主要用于协调及调度各功能模块的工作。
 *
 * 2017.03.13
 * 共开了3个线程，一个主线程，一个通信线程，一个标签规则执行线程。
 * 标签操作除执行操作外，其余操作将在主线程中进行。
 */

class Main_Application
{
public:
    Main_Application();
    ~Main_Application();

public:
		Param_Manager *m_pParamManager;
private:
		Serial_Comm *m_pSerialComm;
    void initSerialComm();

		Operation_Process *m_pOperationProcess;
		void initOperationProcess();

		VersionManager *m_pVersionManager;
		void initVersionManager();
		

		void initParamManager();

		Rf_Manager *m_pRfManager;
		void initRfManager();


		void initM2SecComm();

    Sm2Certification *m_pSm2Certification;

		IOCtrl *m_pIOCtrl;
public:
	MCUToSecureComm *m_pM2SecComm;
private:
    void dealRecvMessages(CMessage *pMessage,Base_Comm *pComm);
    int dealGeneralMessage(CMessage *pMessage,Base_Comm *pComm);

    void initDeviceInfo();

		static bool sendMessageWithID(CMessage *,int,llrp_u32_t);
		bool sendMessage();
public:
    Log_Process logProcess; //大家公用的一个日志记录对象
public:
	void Report_OpenSatrt(void);
	void Report_OpenFinish(void);
	void Report_SMNUM(llrp_u8v_t nr);
public:
    void run();


public:
		inline Serial_Comm* getSerial_Comm(){
			return m_pSerialComm;
		}	

		inline Param_Manager* getParamManager(){
			return m_pParamManager;
		}
		
		inline Operation_Process *getOpProcess(){
			return m_pOperationProcess;
		}
		
		void SMCertification();
		bool writeDeviceInfo(llrp_u8v_t device_sn);
		llrp_u8v_t readDeviceInfo();
		
public:
	  llrp_u8v_t deviceSN;
    llrp_utf8v_t deviceManufacturerName;
};

extern Main_Application *g_pMainApplication;

#endif // MAIN_CONTROL_H
