#include "rf_manager.h"
#include "systick.h"
#include "Cancellation.h"
#include "systick.h"
#include "board.h"
#include "cpu_5v_en.h"
#include "EN25Q256.h"
#include "AD5602.h"


Rf_Manager::Rf_Manager()
{
  m_pRfOper = NULL;
	
	PA_POWER = this->readPowerPA();
	if(PA_POWER <= 1600){
		PA_POWER = 1600;
	}
	RFPowerON(PA_POWER);
}

bool Rf_Manager::writePowerPA(uint16_t x){
	
	unsigned char buf[POWER_PA_SIZE];
	int buflen = 2;
	
	buf[0] = x>>8;
	buf[1] = x;
	
	EN25Q256_Write(buf,POWER_PA_ADDR,buflen);
	unsigned char POWER_PA_Flag = flash_Isok;//0x0A:正常	
	EN25Q256_Write(&POWER_PA_Flag,POWER_PA_flag,1);
	return true;
}

uint16_t Rf_Manager::readPowerPA(){
	uint16_t value = 0;
	
	u8 flashMessage_temp = 0;
	printf("Read readPowerPA ....\r\n");
	EN25Q256_Read(&flashMessage_temp,POWER_PA_flag,1); 
	if(flashMessage_temp == flash_Isok)//0x0A:正常	
	{
		//这里从FLASH中读取配置
		unsigned char buf[POWER_PA_SIZE];
		EN25Q256_Read(buf,POWER_PA_ADDR,2);		
		value = buf[0];
		value = value<<8|buf[1];
	}else{
		value = 2700;
	}

	return value;
}

/**
 * @brief Rf_Manager::dealMessage
 * @param pMessage
 * @param commID
 * @return
 * 处理射频管理消息
 */
int Rf_Manager::dealMessage(CMessage *pMessage,Base_Comm *pComm){
		llrp_u32_t messageid = pMessage->getMessageID();
    if(pMessage->m_pType->m_TypeNum!=ManufacturerCommand){
        return -1;
    }
    CManufacturerCommand *pcommd = (CManufacturerCommand*)pMessage;
    llrp_u8v_t bytes = pcommd->getByteStream();
//    printf("bytes is ");
//    for(int i=0;i<bytes.m_nValue;i++){
//        printf("0x%02x,",bytes.m_pValue[i]);
//    }
//    printf("\n");

    STRUCT_ManufacturerFrame manuframe;
    manuframe.OperID = bytes.m_pValue[0];
    manuframe.Datalen_H = bytes.m_pValue[1];
    manuframe.Datalen_L = bytes.m_pValue[2];
    manuframe.DataBuf = bytes.m_pValue+3;

    CManufacturerReport *preportmessage =  (CManufacturerReport *)ExtendHbFrame_Operation::genMessage(ManufacturerReport,0);
    switch(manuframe.OperID){
    case 0x80: //开始定标
    case 0x81: //结束定标
    case 0x82://功率微调
    {
//        llrp_u8v_t bytes = m_pRfOper->RfPowerCalibration(&manuframe); //注意这里会根据不同的射频版本做不同的定标处理
//        if(bytes.m_nValue==0){
//            printf("powercalibration fail!\n");
//        }else{
//            printf("powercalibration success!\n");
//        }
//        preportmessage->setByteStream(bytes);
    }
        break;
    case 0x83: //读取定标数据
    {
//        llrp_u8v_t bytes = m_pRfOper->getPowerCalibrationBytes();
//        preportmessage->setByteStream(bytes);
    }
        break;
    case 0x84: //手动对消
    {
		//先退出低功耗
		FPGA_LowPower(0);//退出低功耗上电
		//开始射频操作
		//Delay_ms(100);
		m_pRfOper->switchCarrier(4,0x00,1);	
		
		Cancellation(); //对消
		  
		//关闭单载波
		m_pRfOper->switchCarrier(4,0x00,0);	
			
	FPGA_LowPower(1);

		llrp_u8v_t bytes = llrp_u8v_t(5);   //响应
		bytes.m_pValue[0] = 0x04;  					//操作标识
		bytes.m_pValue[1] = 0x00;           //帧长度
		bytes.m_pValue[2] = 0x02;           //帧长度
		bytes.m_pValue[3] = 0x04; 			    //帧内容	天线号
		bytes.m_pValue[4] = 0x00;             // 0表示成功。1表示失败。
			preportmessage->setByteStream(bytes);
//		if(preportmessage!=NULL){
//			pComm->immediateSendMessage(preportmessage,pMessage->getMessageID());

//			//delete preportmessage;
//		}
    }
        break;
    case 0x85://开启关闭单载波
    {
				
        llrp_u8v_t bytes = llrp_u8v_t(7);
        bytes.m_pValue[0] = 0x05;
        bytes.m_pValue[1] = 0x04;
        bytes.m_pValue[2] = 0x00;
        bytes.m_pValue[3] = manuframe.DataBuf[0];
        bytes.m_pValue[4] = manuframe.DataBuf[1];
        bytes.m_pValue[5] = manuframe.DataBuf[2];
        if(manuframe.DataBuf[2]==0){//关
						
            if(m_pRfOper->switchCarrier(manuframe.DataBuf[0],manuframe.DataBuf[1],0)){
                bytes.m_pValue[6] = 0;
            }else{
                bytes.m_pValue[6] = 1;
            }
						
						FPGA_LowPower(1);
        }else{//开
						FPGA_LowPower(0);
            //2017-03-21新加，开启载波前，先关一次，以确保成功开闭
            m_pRfOper->switchCarrier(manuframe.DataBuf[0],manuframe.DataBuf[1],0);
            Delay_ms(100);
            if(m_pRfOper->switchCarrier(manuframe.DataBuf[0],manuframe.DataBuf[1],1)){
                bytes.m_pValue[6] = 0;
            }else{
                bytes.m_pValue[6] = 1;
            }
						
//							DAC_SetChannel2Data(DAC_Align_12b_R, DA_I_G[0]);
//							DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q_G[0]);	
//							Delay_ms(10);
//							AD_Value=Get_ADC_Value(ADC1);
//							printf("\n DA_I is :%d   \n",DA_I_G[0]);		
//							printf("\n DA_Q is :%d   \n",DA_Q_G[0]);
//							printf("\n AD_Min is :%d \n",AD_Value);
        }
        preportmessage->setByteStream(bytes);
    }
        break;
    case 0x88://查询泄漏对消状态
    {		
//        string statestr = m_pRfOper->convertADDataToString();
//        int len = statestr.length();
//        llrp_u8v_t bytes = llrp_u8v_t(statestr.length()+3);
//        bytes.m_pValue[0] = 0x08;
//        bytes.m_pValue[1] = len;
//        bytes.m_pValue[2] = len>>8;
//        memcpy(bytes.m_pValue+3,statestr.c_str(),statestr.length());
//        preportmessage->setByteStream(bytes);
    }
        break;
    case 0x8A://2016-11-14新加漫速对消
    {
			//开启前先关再开单载波
//			m_pRfOper->switchCarrier(4,0x00,0);
//			Delay_ms(100);
			
			FPGA_LowPower(0);
		   	m_pRfOper->switchCarrier(4,0x00,1);	
			
				Cancellation00(); //对消
			  
			//关闭单载波
			m_pRfOper->switchCarrier(4,0x00,0);	
		FPGA_LowPower(1);
			llrp_u8v_t bytes = llrp_u8v_t(5);   //响应
			bytes.m_pValue[0] = 0x0A;  					//操作标识
			bytes.m_pValue[1] = 0x00;           //帧长度
			bytes.m_pValue[2] = 0x02;           //帧长度
			bytes.m_pValue[3] = 0x04; 			    //帧内容	天线号
			bytes.m_pValue[4] = 0x00;             // 0表示成功。1表示失败。
			preportmessage->setByteStream(bytes);
    }
        break;
		case 0xA7: //配置功率值
		{
			uint16_t v = 600;
			v = manuframe.DataBuf[0];
			v = v<<8|manuframe.DataBuf[1];
			
			llrp_u8v_t bytes = llrp_u8v_t(4);   //响应
			bytes.m_pValue[0] = 0x27;  					//操作标识
			bytes.m_pValue[1] = 0x00;           //帧长度高字节
			bytes.m_pValue[2] = 0x01;           //帧长度低字节

			if(this->writePowerPA(v)==true){
				bytes.m_pValue[3] = 0x00;             // 0表示成功。1表示失败。
			}else{
				bytes.m_pValue[3] = 0x01;             // 0表示成功。1表示失败。
			}
			preportmessage->setByteStream(bytes);
//			if(preportmessage!=NULL){
//				pComm->immediateSendMessage(preportmessage,pMessage->getMessageID());
//			}
			
			PA_POWER = v;
			RFPowerON(PA_POWER);
		}
			break;
		case 0xA8: //查询功率值
		{
			llrp_u8v_t bytes = llrp_u8v_t(5);   //响应
			bytes.m_pValue[0] = 0x28;  					//操作标识
			bytes.m_pValue[1] = 0x00;           //帧长度高字节
			bytes.m_pValue[2] = 0x02;           //帧长度低字节
			bytes.m_pValue[3] = PA_POWER>>8;
			bytes.m_pValue[4] = PA_POWER;
			preportmessage->setByteStream(bytes);
//			if(preportmessage!=NULL){
//				pComm->immediateSendMessage(preportmessage,pMessage->getMessageID());
//			}
		}
			break;
		case 0xA9: //场强扫描
    {
		
		FPGA_LowPower(0);
        unsigned int len = manuframe.Datalen_H;
        len = len<<8;
        len = len|manuframe.Datalen_L;
        int fieldcount = len/2;
        //unsigned short  fields[fieldcount];
				unsigned short  *fields = (unsigned short  *)malloc(fieldcount);
			
        for(int i=0;i<fieldcount;i++){
            fields[i] = manuframe.DataBuf[i*2+0];
            fields[i] = fields[i]<<8;
            fields[i] = fields[i]|manuframe.DataBuf[i*2+1];
        }
        //unsigned int rssis[fieldcount];
				unsigned int *rssis = (unsigned int *)malloc(fieldcount);
        for(int i=0;i<fieldcount;i++){
            unsigned char antid = fields[i]>>8;
            unsigned char freqid = fields[i];
            unsigned short rssi = m_pRfOper->queryRssi(antid,freqid);
            printf("ant id = %d,freq id = %d,rssi=%d\n",antid,freqid,rssi);
            rssis[i] = rssi;
            rssis[i] = rssis[i]|(antid<<24);
            rssis[i] = rssis[i]|(freqid<<16);
        }
		FPGA_LowPower(1);
        //以下上报结果
        llrp_u8v_t bytes = llrp_u8v_t(fieldcount*4+3);
        bytes.m_pValue[0] = 0x29;
        bytes.m_pValue[1] = (fieldcount*4)>>8;
        bytes.m_pValue[2] = (fieldcount*4);
        for(int i=0;i<fieldcount;i++){
            bytes.m_pValue[i*4+3] = rssis[i]>>24;
            bytes.m_pValue[i*4+4] = rssis[i]>>16;
            bytes.m_pValue[i*4+5] = rssis[i]>>8;
            bytes.m_pValue[i*4+6] = rssis[i];
        }

//        for(int i=0;i<bytes.m_nValue;i++){
//            printf("0x%02x,",bytes.m_pValue[i]);
//        }
//        printf("\n");

        preportmessage->setByteStream(bytes);
				free(fields) ;
				free(rssis);
    }
        break;
    default:
			delete preportmessage;
        return -1;
    }//end of switch();
    if(preportmessage!=NULL){
				if(!pComm->immediateSendMessage(preportmessage,messageid)){
            printf("Send rf mananger message fail!\n");
        }
    }
    delete preportmessage;
    return 0;
}

void Rf_Manager::run(){
	
}
