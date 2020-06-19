#include "main_application.h"
#include <stdio.h>

#include <rtc.h>

#include "general.h"
#include "GetDeviceCapabilities.h"
#include "ExtendLLRP.h"
#include "ManufacturerFrame.h"
#include "ExtendHbFrame_Operation.h"
#include "sim_Spi.h"
#include "sim_Spi1.h"
#include "systick.h"
#include "lowpower.h"
#include "general_functions.h"
#include "EN25Q256.h"
#include "main.h"
#include "Bsp_init.h"
Main_Application *g_pMainApplication;


Main_Application::Main_Application()
{

  //////////////////////////////////////////////////////////////
//	hw_uart_init();
    g_pMainApplication = this;
	
	deviceSN = this->readDeviceInfo();
	string name = "Bellon Desktop ISSUSER";
	deviceManufacturerName = llrp_utf8v_t(name.length()+1);
	strcpy((char *)deviceManufacturerName.m_pValue,name.c_str());
	deviceManufacturerName.m_pValue[deviceManufacturerName.m_nValue] = 0x00; //加上字符串结尾

	m_pIOCtrl = new IOCtrl();
	initSerialComm();
	initOperationProcess();
	initVersionManager();
	initParamManager();//读取flash中的配置 并且配置1 2 3 4 天线
	
	initRfManager();
	Report_OpenSatrt();
	m_pRfManager->setRfOper(m_pOperationProcess->getOpExecute()->getRfOper());
	
	initM2SecComm();
	m_pM2SecComm->setIOCtrl(m_pIOCtrl);
	
//	hw_init();
	//这里一定要记住设置，否则会出错
    m_pVersionManager->m_UpdateSM.setIOCtrl(m_pIOCtrl);
	m_pVersionManager->m_UpdateSM.setM2SComm(m_pM2SecComm);
	
	m_pParamManager->setM2SecComm(m_pM2SecComm);
	m_pParamManager->setIOCtrl(m_pIOCtrl);
	
	m_pOperationProcess->getOpExecute()->setMCUToSecComm(m_pM2SecComm);
	m_pOperationProcess->getOpExecute()->getRfOper()->setIOCtrl(m_pIOCtrl);
	m_pOperationProcess->getOpExecute()->getRfOper()->setM2SecComm(m_pM2SecComm);
	
	m_pSm2Certification = new Sm2Certification();
	m_pSm2Certification->setM2SecComm(m_pM2SecComm);
    m_pSm2Certification->setIOCtrl(m_pIOCtrl);

	//行标中使用
	if(RF_USE_GBFPGA){
		
	}
	else if(RF_USE_HBFPGA){
		this->SMCertification(); //进行安全认证
	}
   
	RFPowerON(PA_POWER);
//	//开机对消
	Getcancellparam();
//	m_pOperationProcess->getOpExecute()->getRfOper()->switchCarrier(4,0x00,1);	
//		
//	Cancellation(); //对消
//			
//	//关闭单载波
//	m_pOperationProcess->getOpExecute()->getRfOper()->switchCarrier(4,0x00,0);
	
	
	//测试按键读卡规则
	
	//m_pOperationProcess->getOpSpecManager()->addSelectSpec((CAddSelectSpec*)pMessage,true)	;
	//m_pOperationProcess->getOpSpecManager()->activeGPISelectSpec();
	m_pOperationProcess->getOpSpecManager()->enableSelectSpec(3001);
	
	
	//llrp_u8v_t nr = m_pVersionManager->getSecSerial();
	//printf("sec serial is: %s\n",(char *)(nr.m_pValue));
	//this->Report_SMNUM(nr);
	//m_pParamManager->echoGetCfg();
}

Main_Application::~Main_Application(){
	delete m_pSerialComm;
	delete m_pOperationProcess;
}


void Main_Application::SMCertification(){
    //进行安全认证工作。
    //printf("Start Certification...\n");

    //m_pSm2Certification->CertificationID(false);
	for(int i=0;i<5;i++){
		
		int res = m_pSm2Certification->CertificationID(false);
		if(res>=0){
			break;
		}
	}
    //printf("Certification end!\n");
}

bool Main_Application::sendMessageWithID(CMessage *,int,llrp_u32_t){
	
}

bool Main_Application::sendMessage(){
	
}

void Main_Application::initSerialComm(){
	m_pSerialComm = new Serial_Comm(0); //串口通信口，默认ID号为3
}

void Main_Application::initOperationProcess(){
	m_pOperationProcess = new Operation_Process();
}

void Main_Application::initVersionManager(){
	m_pVersionManager = new VersionManager();
}

void Main_Application::initParamManager(){
	m_pParamManager = new Param_Manager();
}

void Main_Application::initRfManager(){
	m_pRfManager = new Rf_Manager();
}

void Main_Application::initM2SecComm(){
	m_pM2SecComm = new MCUToSecureComm();
}

llrp_u8v_t Main_Application::readDeviceInfo(){

	llrp_u8v_t sn = llrp_u8v_t(8);	
	
	u8 flashMessage_temp = 0;
	printf("Read SN flash ....\r\n");
	EN25Q256_Read(&flashMessage_temp,SN_STORAGE_flag,1); 
	if(flashMessage_temp == flash_Isok)//0x0A:正常
	{
		
		EN25Q256_Read(sn.m_pValue,SN_STORAGE_ADDR,sn.m_nValue);
		
	}else{
		unsigned char deviceSN[8] = {0x42,0x4c,0x12,0x01,0x00,0x00,0x03,0xe9};

		memcpy(sn.m_pValue,deviceSN,8);	
	}
	
	return sn;	

}

bool Main_Application::writeDeviceInfo(llrp_u8v_t device_sn){
	
	printf("Set device sn is:");
	for(int i=0;i<device_sn.m_nValue;i++){
		printf("0x%02x,",device_sn.m_pValue[i]);		
	}
	printf("\n");
	
	deviceSN = device_sn;
	
	EN25Q256_Write(deviceSN.m_pValue,SN_STORAGE_ADDR,deviceSN.m_nValue);
	
	//更新外部设备序列号标志位
	//更新到flash存储正常 
	unsigned char SN_Flash_Flag= flash_Isok;//0x0A:正常					
	EN25Q256_Write(&SN_Flash_Flag,SN_STORAGE_flag,1);
	
	
	return true;
}

/**
 * @brief Main_Application::dealGeneralMessage
 * @param pMessage
 * @return
 * 处理通用的消息
 */
int Main_Application::dealGeneralMessage(CMessage *pMessage,Base_Comm *pComm){
    CMessage *pack = NULL;
    switch(pMessage->m_pType->m_TypeNum){
    case GetDeviceCapabilities:
    {
        llrp_u8_t requesteddata;
        CGetDeviceCapabilities *pGetDeviceCapabilities = (CGetDeviceCapabilities *)pMessage;
        requesteddata = pGetDeviceCapabilities->getRequestedData();
        GetDeviceCapabilitiesFuction capabilities;
        pack = capabilities.getDeviceCapabilitiesRequestedDataMessage(requesteddata);
    }
        break;
    case ResetDevice:
    {
        pack = HbFrame_Operation::genMessage(ResetDeviceAck,0);
        CStatus *pstatus = (CStatus*)HbFrame_Operation::genParam(Status,0,NULL);
        pstatus->setStatusCode(StatusCode_M_Success);
        HbFrame_Operation::addParamToMessage(pstatus,pack);
			
				if(pack!=NULL){
					printf("Send Reset ACK...!\n");
					pComm->immediateSendMessage(pack,pMessage->getMessageID());
					delete pack;
				}
				printf("Reset ..........!\n");
				g_pMainApplication->logProcess.addLog(LOGTYPE_RUN_LOG, DEVICE_RESET, "Device reset !");
				//set_reset_config(flash_Isok);
//				Beep_Delay(500);
//				Delay_ms(1000);
				//单片机复位
				SoftReset();
			
    }
        break;
    default:
        break;
    }
    if(pack!=NULL){
			pComm->immediateSendMessage(pack,pMessage->getMessageID());
			delete pack;
			return 0;
		}else{
			return -1;
		}
}

void Main_Application::dealRecvMessages(CMessage *pMessage,Base_Comm *pComm){
    llrp_u32_t messageid = pMessage->getMessageID();
    llrp_u64_t messagedevsn = pMessage->getDeviceSN(); //取出命令消息的deviceSN,新规范中，可能会要求只对和设备SN一致的消息响应。

    printf("Main_Application:deal message name is %s,MessageID is %d\n",pMessage->m_pType->m_pName,pMessage->getMessageID());

    switch(pMessage->m_pType->m_TypeNum){
    case KeepaliveAck: //心跳返回消息
        //2017.06.27将心跳包的接收处理放入各个链路中自行处理。这里不再会有心跳包在此处理
        //pComm->clearKeepOverTimer();
        break;
		case ManufacturerCommand: //厂家自定义消息
    {	
				CManufacturerCommand *pcommd = (CManufacturerCommand*)pMessage;
				llrp_u8v_t bufs = pcommd->getByteStream();

				STRUCT_ManufacturerFrame manuframe;
				manuframe.OperID = bufs.m_pValue[0];    //操作标识
				manuframe.Datalen_H = bufs.m_pValue[1]; //帧长度 2字节(大端模式)
				manuframe.Datalen_L = bufs.m_pValue[2];
				manuframe.DataBuf = bufs.m_pValue+3;    //帧内容 N字节
				CManufacturerReport *preportmessage =  (CManufacturerReport *)ExtendHbFrame_Operation::genMessage(ManufacturerReport,0);

				switch(manuframe.OperID){
				case 0x87://设置设备序列号
				{
					llrp_u8v_t bytes = llrp_u8v_t(4);
					bytes.m_pValue[0] = 0x07;
					bytes.m_pValue[1] = 0x01;
					bytes.m_pValue[2] = 0x00;
					llrp_u8v_t device_sn = llrp_u8v_t(8);
					for(int i=0;i<8;i++)
					{
							device_sn.m_pValue[i] = manuframe.DataBuf[i];
					}
					if(writeDeviceInfo(device_sn))
					{
							bytes.m_pValue[3] = 0x00;
							printf("Write Device Info Success!\n");
					}
					else
					{
							bytes.m_pValue[3] = 0x01;
					}
					preportmessage->setByteStream(bytes);
					if(preportmessage!=NULL){
						if(!pComm->immediateSendMessage(preportmessage,pMessage->getMessageID())){
								//printf("send ack fail!\n");
						}else{
								//printf("send ack success!\n");
						}
						delete preportmessage;
					}	
				}
				break;
				case 0x91: //手动校时
				{
						llrp_u8v_t bytes = llrp_u8v_t(4);   //响应
						bytes.m_pValue[0] = 0x11;  					//操作标识
						bytes.m_pValue[1] = 0x00;           //帧长度
						bytes.m_pValue[2] = 0x01;           //帧长度
						bytes.m_pValue[3] = 0x00; //成功    //帧内容
						llrp_u64_t utctime = 0;
						llrp_u64_t temp = 0;
						//取出UTC，注意，命令中的UTC是低字节在前
						for(int i=0;i<8;i++){
								temp = manuframe.DataBuf[i];
								utctime = utctime | (temp<<(i*8));
						}
						
						//printf("utctime=%llu\n",utctime);
						//this->m_ntpClient.setUTCTime(utctime);
						//下发微秒 / 1000000 到 s
						if(setRtc((unsigned int)(utctime/1000000))>0){
							bytes.m_pValue[3] = 0x00; //成功
							unsigned long long utc = General_Functions::getCurrentUTC();
							printf("utc is %lld\n",utc);
						}else{
							bytes.m_pValue[3] = 0x01; //失败
						}
						preportmessage->setByteStream(bytes);
						if(preportmessage!=NULL){
							if(!pComm->immediateSendMessage(preportmessage,pMessage->getMessageID())){
									//printf("send ack fail!\n");
							}else{
									//printf("send ack success!\n");
							}
							delete preportmessage;
						}	
						
				}
						break;
				
				case 0x92://设备关机命令
				{

					//发送响应
					llrp_u8v_t bytes = llrp_u8v_t(3);   //响应
					bytes.m_pValue[0] = 0x12;  					//操作标识
					bytes.m_pValue[1] = 0x00;           //帧长度
					bytes.m_pValue[2] = 0x00;           //帧长度
					
					preportmessage->setByteStream(bytes);
					if(preportmessage!=NULL){
						if(!pComm->immediateSendMessage(preportmessage,pMessage->getMessageID())){
								//printf("send ack fail!\n");
						}else{
								//printf("send ack success!\n");
						}
						delete preportmessage;
					}
					
					All_Off();
					lowpower();
					//PWR_STOP();
				}
				break;
				
				case 0x93://设备开机命令
				{
					//发送响应
					llrp_u8v_t bytes = llrp_u8v_t(3);   //响应
					bytes.m_pValue[0] = 0x13;  					//操作标识
					bytes.m_pValue[1] = 0x00;           //帧长度
					bytes.m_pValue[2] = 0x00;           //帧长度
					
					preportmessage->setByteStream(bytes);
					if(preportmessage!=NULL){
						if(!pComm->immediateSendMessage(preportmessage,pMessage->getMessageID())){
								//printf("send ack fail!\n");
						}else{
								//printf("send ack success!\n");
						}
						delete preportmessage;
					}
					g_pMainApplication->logProcess.addLog(LOGTYPE_RUN_LOG, DEVICE_RESET, "Device reset !");
					//直接重启单片机
					printf("Reset ..........!\n");
					//set_reset_config(flash_Isok);
					//Beep_Delay(500);
					//Delay_ms(500);
					//单片机复位
					SoftReset();
				}
				break;
				case 0x95://
				{
					llrp_u8v_t nr = m_pVersionManager->getSecSerial();
					//printf("sec serial is: %s\n",(char *)(nr.m_pValue));
					this->Report_SMNUM(nr);
//					//3.发送响应
//					llrp_u8v_t bytes = llrp_u8v_t(15); 									
//					//2.1雷达信息查询的检测查询			
//					bytes.m_pValue[0] = 0x15;  					
//					bytes.m_pValue[1] = 0x00;           
//					bytes.m_pValue[2] = 0x0C;	
//				
//					memcpy(&bytes.m_pValue[3],nr.m_pValue,0x0C);
//					
//					
//					preportmessage->setByteStream(bytes);	
//					if(preportmessage!=NULL)
//					{				
//						//printf("pMessage->getMessageID() :%d\n",pMessage->getMessageID());
//						if(false == m_pSerialComm->immediateSendMessage(preportmessage,0))
//						{
//								printf("send ack fail!\n");
//						}		
//						delete preportmessage;				
//					}	
				}
				break;
				default: //各个模块处理自定义消息
				{
					delete  preportmessage; 
					printf("deal other mode Message\n");
					if(m_pRfManager->dealMessage(pMessage,pComm)<0){
						if(m_pSm2Certification->dealMfeMessage(pMessage,pComm)<0){
							if(m_pVersionManager->dealMfeMessage(pMessage,pComm)<0){								
							}
						}
					}
				}
						break;
				}//end of switch



		}
				break;
    default://处理常规消息
				if(this->dealGeneralMessage(pMessage,pComm)<0){
					if(m_pOperationProcess->dealMessage(pMessage,pComm)<0){
						if(m_pVersionManager->dealMessage(pMessage,pComm)<0){
							if(m_pParamManager->dealMessage(pMessage,pComm)<0){
								if(m_pSm2Certification->dealMessage(pMessage,pComm)<0){
									if(logProcess.dealMessage(pMessage,pComm)<0)
									{
									}
								}
							}
						}
					}
				}
				break;
    }
}

void Main_Application::run(){


		logProcess.run();
	
		m_pSerialComm->run();
		//含有立即触发、开始标签选择规则、读卡流程
	  //标签信息上报
		m_pOperationProcess->run(); 
	
	
		//////////////////////////////////////////////////////
		if(m_pSerialComm->getRecMessage()!=NULL){
			printf("recv some message!\n");
			//HbFrame_Operation::printMessage(m_SerialComm.getRecMessage());

			dealRecvMessages(m_pSerialComm->getRecMessage(),m_pSerialComm);
			m_pSerialComm->deleteRecMessage(); //删除消息本身
		}
		
		
		//将读标签报告发往各通信链路
    OperationReport_Manager *preportmanager = m_pOperationProcess->getOpReportManager();
    if(preportmanager->getSelectAccessReportsSize()>0){

        CTagSelectAccessReport *preport = preportmanager->frontSelectAccessReport();

        //HbFrame_Operation::printMessage(preport);

				if(m_pSerialComm->sendMessage(preport)==true){
						//printf("send success...............................\n");
				}else{
					
				}
        //删除当前的报告
        preportmanager->popSelectAccessReport();
        delete preport;
    }

		
}
//开始开机(MCU ->PC)
void Main_Application::Report_OpenSatrt(void)
{
			//记录日志
		g_pMainApplication->logProcess.addLog(LOGTYPE_RUN_LOG, DEVICE_START_SUCCESS, "Device starte!");
	
		CManufacturerReport *preportmessage =  (CManufacturerReport *)ExtendHbFrame_Operation::genMessage(ManufacturerReport,0);			
		//3.发送响应
		//发送响应
		llrp_u8v_t bytes = llrp_u8v_t(3);   //响应
		bytes.m_pValue[0] = 0x13;  					//操作标识
		bytes.m_pValue[1] = 0x00;           //帧长度
		bytes.m_pValue[2] = 0x00;           //帧长度
		
		preportmessage->setByteStream(bytes);
		if(preportmessage!=NULL){
			if(false == m_pSerialComm->immediateSendMessage(preportmessage,0)){
					//printf("send ack fail!\n");
			}else{
					//printf("send ack success!\n");
			}
			delete preportmessage;
		}
}


//开机完成响应(MCU ->PC)
void Main_Application::Report_OpenFinish(void)
{
		//记录日志
		g_pMainApplication->logProcess.addLog(LOGTYPE_RUN_LOG, DEVICE_START_SUCCESS, "Device started successfully!");
	
		CManufacturerReport *preportmessage =  (CManufacturerReport *)ExtendHbFrame_Operation::genMessage(ManufacturerReport,0);			
	  //3.发送响应
		llrp_u8v_t bytes = llrp_u8v_t(3); 									
		//2.1雷达信息查询的检测查询			
		bytes.m_pValue[0] = 0x14;  					
		bytes.m_pValue[1] = 0x00;           
		bytes.m_pValue[2] = 0x00;	
		
		preportmessage->setByteStream(bytes);	
		if(preportmessage!=NULL)
		{				
			//printf("pMessage->getMessageID() :%d\n",pMessage->getMessageID());
			if(false == m_pSerialComm->immediateSendMessage(preportmessage,0))
			{
					printf("send ack fail!\n");
			}		
			delete preportmessage;				
		}		
}

//开机完成响应 安全模块序号上报(MCU ->PC)
void Main_Application::Report_SMNUM(llrp_u8v_t nr)
{
		CManufacturerReport *preportmessage =  (CManufacturerReport *)ExtendHbFrame_Operation::genMessage(ManufacturerReport,0);			
	  //3.发送响应
		llrp_u8v_t bytes = llrp_u8v_t(15); 									
		//2.1雷达信息查询的检测查询			
		bytes.m_pValue[0] = 0x15;  					
		bytes.m_pValue[1] = 0x00;           
		bytes.m_pValue[2] = 0x0C;	
	
		memcpy(&bytes.m_pValue[3],nr.m_pValue,0x0C);
		
		
		preportmessage->setByteStream(bytes);	
		if(preportmessage!=NULL)
		{				
			//printf("pMessage->getMessageID() :%d\n",pMessage->getMessageID());
			if(false == m_pSerialComm->immediateSendMessage(preportmessage,0))
			{
					printf("send ack fail!\n");
			}		
			delete preportmessage;				
		}		
}