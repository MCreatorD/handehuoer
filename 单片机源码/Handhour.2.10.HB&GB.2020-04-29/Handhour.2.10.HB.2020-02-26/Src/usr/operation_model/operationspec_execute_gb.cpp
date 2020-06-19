#include "operationspec_execute_gb.h"
#include <stdio.h>

#include "general_functions.h"

#include "systick.h"
#include "fpga.h"
#include "buzzer_led.h"
#include "cpu_5v_en.h"
#include "main.h"
#include "main_application.h"
Operationspec_Execute_GB::Operationspec_Execute_GB()
{
	m_pGBRfOper = (Rf_Oper_GB *)m_pRfOper;
}

Operationspec_Execute_GB::~Operationspec_Execute_GB()
{
}

MCU_TO_SECURE_FRAME *Operationspec_Execute_GB::convertGBFrameToHBSelectFrame(STRUCT_RF2M_FRAME* pGBFrame,CRfSpec *pRfSpec){

    if(pGBFrame==NULL){
        return NULL;
    }
    if(pGBFrame->Head[0]==0x80&&(pGBFrame->Head[1]==0x01||pGBFrame->Head[1]==0x00)){//是读或盘点标签
	
    if(pGBFrame->SuccessMark[1]!=1){ //是错误标志则简单返回空
        //printf("read is error!\n");
        return NULL;
    }

    MCU_TO_SECURE_FRAME *pframe = new MCU_TO_SECURE_FRAME;
    pframe->Head = 0xab;
    pframe->Type = 0x52;

    //以下由国标生成行标数据
    unsigned int buflen = 0;
   //printf("pGBFrame->DataLen[0]=%d,DataLen[1]=%d\n",pGBFrame->DataLen[0],pGBFrame->DataLen[1]);
       buflen = pGBFrame->DataLen[0]<<8|pGBFrame->DataLen[1];
      //printf("buflen=%d\n",buflen);
       unsigned char *buf = new unsigned char[buflen];
       memcpy(buf,pGBFrame->DataBuf,buflen);

//    /////////////////////////////////////////
//    if(pRfSpec->getSelectType()==0){
//        pframe->OperId = 0x01; //设为行标的盘点模式。
//    }else{
//        pframe->OperId = 0x02; //设为行标读模式。
//    }
        if(pGBFrame->Head[1]==0x01){//是读
            pframe->OperId = 0x02;
        }else{
            pframe->OperId = 0x01;
        }
        int tidlen = buf[0]*2 + 2; //注意，字长要换算为字节长，tidlen包括长度的2个字节
    unsigned int datalen = buflen+17+2- tidlen - 2; //数据长度要减去tidlen和数据长度字段的2个字节
    pframe->DataLen[0] = datalen;
    pframe->DataLen[1] = datalen>>8;

    datalen = datalen - 1; //数据区应去除OperId的字节数,并加上成功标志字节
    pframe->DataBuf = new unsigned char[datalen];
    int posi = 0;
    pframe->DataBuf[posi++] = 0x00; //成功标志
//    memset(pframe->DataBuf+posi,0x00,8); //因为国标TID和行标TID长度不一，故将国标全部数据都放入数据区，这里的TID则全填0。
     if(tidlen>=8){
     	memcpy(pframe->DataBuf+posi,buf+2,8); //不管读到的TID是多少，只取前8个字节
	    posi = posi + 8;
        }else{
            memcpy(pframe->DataBuf+posi,buf+2,tidlen);
            posi = posi + tidlen;
            int a = 8-tidlen;
            for(int i=0;i<a;i++){
                pframe->DataBuf[posi++] = 0x00; //不足的TID要补零
            }
        }
    unsigned char temp = pGBFrame->AntId[1];
	    unsigned char a = 0<<4; // 分区号
		   unsigned char b = 0<<2; //分区访问标识
    pframe->DataBuf[posi++] = temp|a|b;

    unsigned long long utc = General_Functions::getCurrentUTC();
    pframe->DataBuf[posi++] = utc>>56;
    pframe->DataBuf[posi++] = utc>>48;
    pframe->DataBuf[posi++] = utc>>40;
    pframe->DataBuf[posi++] = utc>>32;
    pframe->DataBuf[posi++] = utc>>24;
    pframe->DataBuf[posi++] = utc>>16;
    pframe->DataBuf[posi++] = utc>>8;
    pframe->DataBuf[posi++] = utc;



        if(buflen>=tidlen+2){ //用户区有数据
            int userlen = buf[tidlen]<<8|buf[tidlen+1]; //单位是字 //取出GB用户区数据长度
            //printf("userlen=%d,buflen=%d,tidlen=%d\n",userlen,buflen,tidlen);
            if(userlen*2<=buflen-2-tidlen){ //用户区数据长度应和buflen减去tidlen和长度字节后的长度相等
                memcpy(pframe->DataBuf+posi,buf+tidlen+2,userlen*2);

            }
        }
        delete [] buf;

        return pframe;
    }else if(pGBFrame->Head[0]==0x80&&pGBFrame->Head[1]==0x02){ //是写标签
        MCU_TO_SECURE_FRAME *pframe = new MCU_TO_SECURE_FRAME;
        pframe->Head = 0xab;
        pframe->Type = 0x52;

        //以下由国标生成行标数据
        unsigned int buflen = 0;
        //printf("pGBFrame->DataLen[0]=%d,DataLen[1]=%d\n",pGBFrame->DataLen[0],pGBFrame->DataLen[1]);
           buflen = pGBFrame->DataLen[0]<<8|pGBFrame->DataLen[1];
           //printf("buflen=%d\n",buflen);
           unsigned char *buf = new unsigned char[buflen];
           memcpy(buf,pGBFrame->DataBuf,buflen);

    //       printf("readed GB data is: ");
    //       for(int i=0;i<buflen;i++){
    //           printf("0x%02x,",buf[i]);
    //       }
    //       printf("\n");

        /////////////////////////////////////////

        //pframe->OperId = 0x01; //设为行标的盘点模式。
        pframe->OperId = 0x02; //设为行标的读模式


        int tidlen = buf[0]*2 + 2; //注意，字长要换算为字节长，tidlen包括长度的2个字节

        unsigned int datalen = buflen + 17+2 - tidlen - 2; //数据长度要减去tidlen和数据长度字段的2个字节
        pframe->DataLen[0] = datalen;
        pframe->DataLen[1] = datalen>>8;

        datalen = datalen - 1; //数据区应去除OperId的字节数,并加上成功标志字节
        pframe->DataBuf = new unsigned char[datalen];
        int posi = 0;
        pframe->DataBuf[posi++] = 0x00; //成功标志
        //memset(pframe->DataBuf+posi,0x00,8); //因为国标TID和行标TID长度不一，故将国标全部数据都放入数据区，这里的TID则全填0。

        if(tidlen>=8){
            memcpy(pframe->DataBuf+posi,buf+2,8); //不管读到的TID是多少，只取前8个字节
            posi = posi + 8;
        }else{
            memcpy(pframe->DataBuf+posi,buf+2,tidlen);
            posi = posi + tidlen;
        }
        unsigned char temp = pGBFrame->AntId[1];
        unsigned char a = 0<<4; // 分区号
        unsigned char b = 0<<2; //分区访问标识
        pframe->DataBuf[posi++] = temp|a|b;

        unsigned long long utc = General_Functions::getCurrentUTC();
        pframe->DataBuf[posi++] = utc>>56;
        pframe->DataBuf[posi++] = utc>>48;
        pframe->DataBuf[posi++] = utc>>40;
        pframe->DataBuf[posi++] = utc>>32;
        pframe->DataBuf[posi++] = utc>>24;
        pframe->DataBuf[posi++] = utc>>16;
        pframe->DataBuf[posi++] = utc>>8;
        pframe->DataBuf[posi++] = utc;


        if(buflen>=tidlen+2){ //用户区有数据
            int userlen = buf[tidlen]<<8|buf[tidlen+1]; //单位是字 //取出GB用户区数据长度
            //printf("userlen=%d,buflen=%d,tidlen=%d\n",userlen,buflen,tidlen);
            if(userlen*2<=buflen-2-tidlen){ //用户区数据长度应和buflen减去tidlen和长度字节后的长度相等
                memcpy(pframe->DataBuf+posi,buf+tidlen+2,userlen*2);
            }
        }
        delete [] buf;

        return pframe;
    }
}







MCU_TO_SECURE_FRAME *Operationspec_Execute_GB::convertGBFrameToHBAccessFrame(STRUCT_RF2M_FRAME* pGBFrame,CRfSpec *pRfSpec){

    if(pGBFrame==NULL){
        return NULL;
    }

//    if(pGBFrame->Head[0]==0x80&&(pGBFrame->Head[1]==0x01||pGBFrame->Head[1]==0x00)){//是读标签
//    if(pGBFrame->SuccessMark[1]!=1){ //是错误标志则简单返回空
//        //printf("read is error!\n");
//        return NULL;
//    }

//    MCU_TO_SECURE_FRAME *pframe = new MCU_TO_SECURE_FRAME;
//    pframe->Head = 0xab;
//    pframe->Type = 0x52;

//    //以下由国标生成行标数据
//    unsigned int buflen = 0;
//    //printf("pGBFrame->DataLen[0]=%d,DataLen[1]=%d\n",pGBFrame->DataLen[0],pGBFrame->DataLen[1]);
//       buflen = pGBFrame->DataLen[0]<<8|pGBFrame->DataLen[1];
//       //printf("buflen=%d\n",buflen);
//       unsigned char *buf = new unsigned char[buflen];
//       memcpy(buf,pGBFrame->DataBuf,buflen);

////       printf("readed GB data is: ");
////       for(int i=0;i<buflen;i++){
////           printf("0x%02x,",buf[i]);
////       }
////       printf("\n");

//    /////////////////////////////////////////
//    if(pRfSpec->getSelectType()==0){
//        pframe->OperId = 0x01; //设为行标的盘点模式。
//    }else{
//        pframe->OperId = 0x02; //设为行标读模式。
//    }

//    int tidlen = buf[0]*2 + 2; //注意，字长要换算为字节长，tidlen包括长度的2个字节

//    unsigned int datalen = buflen + 17+2 - tidlen - 2; //数据长度要减去tidlen和数据长度字段的2个字节
//    pframe->DataLen[0] = datalen;
//    pframe->DataLen[1] = datalen>>8;

//    datalen = datalen - 1; //数据区应去除OperId的字节数,并加上成功标志字节
//    pframe->DataBuf = new unsigned char[datalen];
//    int posi = 0;
//    pframe->DataBuf[posi++] = 0x00; //成功标志
//    //memset(pframe->DataBuf+posi,0x00,8); //因为国标TID和行标TID长度不一，故将国标全部数据都放入数据区，这里的TID则全填0。

//    if(tidlen>=8){
//        memcpy(pframe->DataBuf+posi,buf+2,8); //不管读到的TID是多少，只取前8个字节
//        posi = posi + 8;
//    }else{
//        memcpy(pframe->DataBuf+posi,buf+2,tidlen);
//        posi = posi + tidlen;
//    }
//    unsigned char temp = pGBFrame->AntId[1];
//    unsigned char a = 0<<4; // 分区号
//    unsigned char b = 0<<2; //分区访问标识
//    pframe->DataBuf[posi++] = temp|a|b;

//    unsigned long long utc = General_Functions::getCurrentUTC();
//    pframe->DataBuf[posi++] = utc>>56;
//    pframe->DataBuf[posi++] = utc>>48;
//    pframe->DataBuf[posi++] = utc>>40;
//    pframe->DataBuf[posi++] = utc>>32;
//    pframe->DataBuf[posi++] = utc>>24;
//    pframe->DataBuf[posi++] = utc>>16;
//    pframe->DataBuf[posi++] = utc>>8;
//    pframe->DataBuf[posi++] = utc;


//    if(buflen>=tidlen+2){ //用户区有数据
//        int userlen = buf[tidlen]<<8|buf[tidlen+1]; //单位是字 //取出GB用户区数据长度
//        //printf("userlen=%d,buflen=%d,tidlen=%d\n",userlen,buflen,tidlen);
//        if(userlen*2<=buflen-2-tidlen){ //用户区数据长度应和buflen减去tidlen和长度字节后的长度相等
//            memcpy(pframe->DataBuf+posi,buf+tidlen+2,userlen*2);

//        }
//    }
//    delete [] buf;

//    return pframe;
//    }else 
		if(pGBFrame->Head[0]==0x80&&pGBFrame->Head[1]==0x02){ //是写标签
        MCU_TO_SECURE_FRAME *pframe = new MCU_TO_SECURE_FRAME;
        pframe->Head = 0xaa;
        pframe->Type = 0xb2;
        pframe->OperId = 0x00; //写分区请求响应
        if(pGBFrame->SuccessMark[1]==0x01){//成功
            pframe->DataLen[1] = 0x00;
            pframe->DataLen[0] = 0x01;
            pframe->DataBuf = new unsigned char[1];
            pframe->DataBuf[0] = 0x00;
        }else{//失败
            pframe->DataLen[1] = 0x00;
            pframe->DataLen[0] = 0x02;
            pframe->DataBuf = new unsigned char[2];
            pframe->DataBuf[0] = 0x02;
            pframe->DataBuf[1] = 0x00;
        }
        pframe->BCCCheck = 0x00;
        return pframe;
    }
}


#if 1


bool Operationspec_Execute_GB::singleSelectAccess(Tag_SelectSpec *pTagSelectSpec,Tag_AccessSpec *pTagAccessSpec,CAntennaSpec *pAntennaSpec,llrp_u16_t antSpecIndex,CRfSpec *pRfSpec){
	int beepcount = 0; //Beep提示音计数
	int select_count = 0; //控制连续盘点数量。
	
	/*
	* 阶段一 配置读卡和写卡参数 第一次配置第二次参数未改变相等则跳出
	**/
	//m_pGBRfOper->setFPGASingleStart();//通知FPGA当前标签操作开始
	//配置安全模块及射频工作参数，如工作参数未变动，则不重新配
	//配置FPGA AB AC  C1 C2 等 依据有没有Access操作设置hasContinueOperation
	m_pGBRfOper->setSelectAccessParam(pTagSelectSpec,pTagAccessSpec,antSpecIndex,pRfSpec);	
	
	if(m_pGBRfOper->m_AntChangeDelay>=SWITCHANT_DELAY*2)
	{
			//下发指令
			m_pGBRfOper->stopRfOper();
			//配置要切换的天线和频率的射频参数，此函数并不会立即改变配置
			m_pGBRfOper->setSelectRfParam();//让 m_pGBRfOper->m_AntChangeDelay 置 0 第二次不会进入此处配置
			//将上述配置发往射频，如参数未变动，则不会发送。
			m_pGBRfOper->sendSelectRfParam(SETRFWORK_ANT);
			m_pGBRfOper->sendSelectRfParam(SETRFWORK_FREQ);
			return;
	}
	
	//配置安全模块及射频工作参数，如工作参数未变动，则不重新配
	//配置FPGA AB AC  C1 C2 等 依据有没有Access操作设置hasContinueOperation
	//m_pGBRfOper->setSelectAccessParam(pTagSelectSpec,pTagAccessSpec,antSpecIndex,pRfSpec);
	//m_pGBRfOper->sendParamToRf(PARAM_AB,0);	
	/*
	* 阶段二 参数配置完毕后 控制FPGA 开始操作
	**/
	m_pGBRfOper->startRfOper();
	rfOper_OvertimerCount = 50;//500ms
	while(m_pGBRfOper->isFPGATurnEnd()==false&&rfOper_OvertimerCount>0){
		//FPGA 在 接收到 startRfOper 后有上报 且不为空
		if(m_pGBRfOper->isRfNeedComm()==true){
			//rfOper_OvertimerCount = 50;
			//提取本张标签的数据 数据处理
			STRUCT_RF2M_FRAME *gbframe = m_pGBRfOper->readGBTag();
			if(gbframe!=NULL)
			{
				//读取到一张标签再重新赋值50ms
				rfOper_OvertimerCount = 50;
				this->m_TagObTagNCount++;
				//现盘点 读 写 都是一次结束 固直接判断收完数据 组帧上报
				//判断是否FPGA 已经告知本轮已结束
				if(m_pGBRfOper->isFPGATurnEnd()==true)
				{
						//先发stop 再组帧上报 节约能耗
						m_pGBRfOper->stopRfOper();
						//break;
				}
				
				//判断是读还是写
				if(pTagSelectSpec->hasContinueOperation==false){
					//安装行标格式转换上报
					MCU_TO_SECURE_FRAME *pframe = convertGBFrameToHBSelectFrame(gbframe,pRfSpec);
					m_pGBRfOper->removeGBFrame(gbframe);

					if(pframe!=NULL){
						//MCUToSecureComm::printfSecFrame(pframe);
						beepcount++;
						OpspecResult *popresult = new OpspecResult();
						popresult->pOpResultFrame = pframe;
						popresult->setOpParams(pTagSelectSpec);
						
						//this->pushOpResult(popresult);
						if(this->getOpResultSize()<4){//限制本轮读卡中的都标签张数
							//if(0){	
							this->pushOpResult(popresult);	
						}else{
							delete popresult;
							//break;
						}
						if(beepcount>=4)//限制本路读卡中的多标签次数
						{
							break;
						}
					}					
				}else{//有写操作				
					//安装行标格式转换上报
					MCU_TO_SECURE_FRAME *pframe = convertGBFrameToHBSelectFrame(gbframe,pRfSpec);
					MCU_TO_SECURE_FRAME *pframe1 = this->convertGBFrameToHBAccessFrame(gbframe,pRfSpec);
					m_pGBRfOper->removeGBFrame(gbframe);
					Tag_SelectAccessResult *pselectaccessresult = NULL;
//					if(pframe != NULL)
//					{
//						printf("------------convertGBFrameToHBSelectFrame----------------\r\n");
//					}
//					if(pframe1 != NULL)
//					{
//						printf("------------convertGBFrameToHBAccessFrame----------------\r\n");
//					}
					if(pframe != NULL&&pframe1 != NULL){
						beepcount++;//响
						printf("------------convertGBFrameToHBSelectFrame----------------\r\n");
            //MCUToSecureComm::printfSecFrame(pframe);
						printf("------------convertGBFrameToHBAccessFrame----------------\r\n");
						//MCUToSecureComm::printfSecFrame(pframe1);
            pselectaccessresult = new Tag_SelectAccessResult();
						pselectaccessresult->ptagSelectResult =  this->genSelectResult(pframe,pTagSelectSpec);
						pselectaccessresult->ptagAccessOpResults = NULL;
						llrp_u8v_t tid = MCUToSecureComm::getTIDFromTagFrame(pframe);
						int antennaid = (pframe->DataBuf[9]&0x03) + 1;
						//MCUToSecureComm::printfSecFrame(pframe1);
            //放入写操作结果
						CAccessSpec *paccess = pTagSelectSpec->tagAccessSpecs.at(0)->getAccessSpec();
						CAccessCommand *paccesscmd = paccess->getAccessCommand();
						CParameter *opspec = *paccesscmd->beginOpSpec();
						if(pselectaccessresult->ptagAccessOpResults==NULL){
							pselectaccessresult->ptagAccessOpResults = new vector<OpspecResult*>;
						}
						OpspecResult *popres = new OpspecResult();
						popres->pOpResultFrame = pframe1; //注意，这里有可能是空
						popres->setOpParams(pTagSelectSpec,opspec);
						popres->operTagTID = tid; //写结果中，必须保存的操作标签的TID号。
						popres->operAntennaID = antennaid;
						pselectaccessresult->ptagAccessOpResults->push_back(popres);
						this->pushSelectAccessResult(pselectaccessresult);	
						break;
					}
					//组装失败帧
				}
			}
		}
	}//end of while
	
	m_pGBRfOper->stopRfOper();
	Delay_ms(50);

	if(beepcount>8)beepcount=8;
	for(int i=0;i<beepcount;i++){
		Beep_Delay(20);
		Delay_ms(15);
	}
	return true;
}

#else

bool Operationspec_Execute_GB::singleSelectAccess(Tag_SelectSpec *pTagSelectSpec,Tag_AccessSpec *pTagAccessSpec,CAntennaSpec *pAntennaSpec,llrp_u16_t antSpecIndex,CRfSpec *pRfSpec){
static	int count = 1;	
	switch(m_SelectStep){
		case 0: //一次select操作的第一步，预配置工作参数，第一次singleSelectAccess需进入此段代码，然后singleSelectAccess不再需要进入此段代码
		{
			m_pGBRfOper->setSelectAccessParam(pTagSelectSpec,pTagAccessSpec,antSpecIndex,pRfSpec);
			printf("--------CASE 0--------\n");
			///////////////////////////////////////////////////
//			if(m_pGBRfOper->m_AntChangeDelay>=SWITCHANT_DELAY*2){
			if(count == 1){
					//CPU_5V_EN_H;
					//FPGA_LowPower(0);//退出低功耗上电
				
					//下发指令
		            Delay_ms(2);
					//m_pGBRfOper->m_AntChangeDelay = 0;
					m_pGBRfOper->stopRfOper();
					//配置要切换的天线和频率的射频参数，此函数并不会立即改变配置
					m_pGBRfOper->setSelectRfParam();
					//将上述配置发往射频，如参数未变动，则不会发送。
					m_pGBRfOper->sendSelectRfParam(SETRFWORK_ANT);
					m_pGBRfOper->sendSelectRfParam(SETRFWORK_FREQ);
					m_SelectStep = 1;
				 count++;
			}else{
				    //开始射频操作
				CPU_5V_EN_H;
				
				m_pGBRfOper->startRfOper();
				printf("--------startRfOper--------\n");
					rfOper_OvertimerCount = 2000;//超时设为5秒
					m_SelectStep = 2;
			}
			///////////////////////////////////////////////////		
		}
			break;
		case 1: //第二步,开始射频操作
			//开始射频操作
			m_pGBRfOper->startRfOper();
			rfOper_OvertimerCount = 2000;//超时设为5秒
			m_SelectStep = 2;
			break;
		case 2: //第三步，多次执行此步，直到有操作结果或超时，才进入下一步	
			if(m_pGBRfOper->isFPGATurnEnd()==false||m_pGBRfOper->isRfNeedComm()==true){
				printf("-----------------------CASE 2_1-----------------------\n");
       	 if(m_pGBRfOper->isRfNeedComm()==true){
				   //printf("-----------------------CASE 2_2-----------------------\n");
						rfOper_OvertimerCount = 2000;
            STRUCT_RF2M_FRAME *gbframe = m_pGBRfOper->readGBTag();
            if(gbframe!=NULL){
                //printf("gbframe is not null!\n");
                if(pTagSelectSpec->hasContinueOperation==false){
                MCU_TO_SECURE_FRAME *pframe = convertGBFrameToHBSelectFrame(gbframe,pRfSpec);
                m_pGBRfOper->removeGBFrame(gbframe);
                if(pframe!=NULL){
                    //MCUToSecureComm::printfSecFrame(pframe);
                    OpspecResult *popresult = new OpspecResult();
                    popresult->pOpResultFrame = pframe;
                    popresult->setOpParams(pTagSelectSpec);
                    this->pushOpResult(popresult);
								  	BUZZER_LED_H;
                    }
                }else{//有写操作
                    Tag_SelectAccessResult *pselectaccessresult = NULL;
                    MCU_TO_SECURE_FRAME *pframe = this->convertGBFrameToHBSelectFrame(gbframe,pRfSpec);
                    m_pGBRfOper->removeGBFrame(gbframe); //删除原GB帧
                    if(pframe!=NULL){
                        //MCUToSecureComm::printfSecFrame(pframe);
                        pselectaccessresult = new Tag_SelectAccessResult();
                        pselectaccessresult->ptagSelectResult =  this->genSelectResult(pframe,pTagSelectSpec);
                        pselectaccessresult->ptagAccessOpResults = NULL;
                        llrp_u8v_t tid = MCUToSecureComm::getTIDFromTagFrame(pframe);
                        int antennaid = (pframe->DataBuf[9]&0x03) + 1;
                        //printf("a0.................\n");
                        //开始写
                        this->m_pGBRfOper->setFPGAWriteGBEnable();
                        //printf("a1.................\n");
                        if(m_pGBRfOper->waitFPGANeedComm(100)){
                            //printf("a2.................\n");
                            gbframe = m_pGBRfOper->readGBTag();
                            if(gbframe!=NULL){
                                MCU_TO_SECURE_FRAME *pframe = this->convertGBFrameToHBAccessFrame(gbframe,pRfSpec);
                                m_pGBRfOper->removeGBFrame(gbframe); //删除原GB帧
                                if(pframe!=NULL){
                                    //MCUToSecureComm::printfSecFrame(pframe);
                                    //放入写操作结果
                                    CAccessSpec *paccess = pTagSelectSpec->tagAccessSpecs.at(0)->getAccessSpec();
                                    CAccessCommand *paccesscmd = paccess->getAccessCommand();

                                    CParameter *opspec = *paccesscmd->beginOpSpec();
                                    if(pselectaccessresult->ptagAccessOpResults==NULL){
                                        pselectaccessresult->ptagAccessOpResults = new vector<OpspecResult*>;
                                    }
                                    OpspecResult *popres = new OpspecResult();
                                    popres->pOpResultFrame = pframe; //注意，这里有可能是空
                                    popres->setOpParams(pTagSelectSpec,opspec);
                                    popres->operTagTID = tid; //写结果中，必须保存的操作标签的TID号。
                                    popres->operAntennaID = antennaid;
                                    pselectaccessresult->ptagAccessOpResults->push_back(popres);
                                    this->pushSelectAccessResult(pselectaccessresult);
																		BUZZER_LED_H;	
                                }
                            }

                        }
                        this->m_pGBRfOper->setFPGAWriteGBDisable();
                    }
                }									
            }        
						m_SelectStep = 3;
        }else{
					if(rfOper_OvertimerCount==0){
							//2017.11.13新加超时复位
							printf("fpga error,now restart fpga!\n");
							printf("isRfNeedComm && rfOper_OvertimerCount==0 \n");
//							m_pGBRfOper->stopRfOper();

							//m_pGBRfOper->clearDoneCancellation();//2018.1.15新加，结束清除对消，以保证下一次启动规则执行时，必定会对消。

							/////////////////////////////////////////////////////////
		//					this->forceExitExecute = true; //强制退出执行，以重新开始操作流程,2017.11.13新加。
		//					m_pGBRfOper->stopRfOper();
							m_SelectStep = 3;
					 }else{ //未超时继续执行此步
						  //m_SelectStep = 3;
							return false;
					 }					
					
				}					
			}else{
					m_SelectStep = 3;
      }			
			break;
		case 3:  //结束本次射频操作
			m_pGBRfOper->stopRfOper();
		  printf("--------stopRfOper--------\n");
		
		
			//低功耗模式
		  //FPGA_LowPower(1);
		  //CPU_5V_EN_L;	
		
			Delay_ms(10);	
			BUZZER_LED_L;			
			Delay_ms(90);	
		
			
			//FPGA_LowPower(0);//退出低功耗上电
				
			//清空参数重新下发指令
      m_pRfOper->resetAA();
      m_pRfOper->resetCurrentSpec();	
			Delay_ms(2);			
			//配置要切换的天线和频率的射频参数，此函数并不会立即改变配置
			m_pGBRfOper->setSelectRfParam();
			//将上述配置发往射频，如参数未变动，则不会发送。
		//m_pGBRfOper->sendSelectRfParam(SETRFWORK_ANT);
			m_pGBRfOper->sendSelectRfParam(SETRFWORK_FREQ);
			Delay_ms(2);
			//2017.12.13新加功率控制
			m_pGBRfOper->sendSelectRfParam(SETRFWORK_POWER);
			rfOper_DelayCount = 1; //延时1毫秒
			m_SelectStep = 4;
			break;
		case 4: //延时一段时间后，结束本次select操作，并返回真
			if(rfOper_DelayCount==0){
				m_SelectStep = 0;
				return true;
			}else{
				return false;
			}
		
			break;
	}
	return false;

}
#endif
void Operationspec_Execute_GB::run(){
	OperationSpec_Execute_Base::run();
}
