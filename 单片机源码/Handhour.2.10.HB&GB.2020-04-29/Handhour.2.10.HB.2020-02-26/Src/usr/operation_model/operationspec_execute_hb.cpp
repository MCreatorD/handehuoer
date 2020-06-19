#include "OperationSpec_Execute_HB.h"
#include <stdio.h>

#include "main.h"
#include "systick.h"
#include "main_application.h"
#include "general_functions.h"

#include "fpga.h"
#include "buzzer_led.h"
#include "cpu_5v_en.h"
#define readlow 1
OperationSpec_Execute_HB::OperationSpec_Execute_HB(){

	m_pHBRfOper = (Rf_Oper_HB *)m_pRfOper;
	m_pCurrentSelectAccessResult = NULL;
}

OperationSpec_Execute_HB::~OperationSpec_Execute_HB(){

}

static unsigned int read_speed = 0;
bool OperationSpec_Execute_HB::singleSelectAccess(Tag_SelectSpec *pTagSelectSpec,Tag_AccessSpec *pTagAccessSpec,CAntennaSpec *pAntennaSpec,llrp_u16_t antSpecIndex,CRfSpec *pRfSpec){

	///////////////////////////////////////////////////////////////
	int beepcount = 0; //Beep提示音计数
	CHbPrivateWriteSpec *pprivatewritespec = NULL;
	int select_count = 0; //控制连续盘点数量。
	
	//控制读卡速度
	if(!((read_speed++)%50 == 0))
	{
		m_pRfOper->stopRfOper();
	#if readlow		
		FPGA_LowPower(1);
	#endif	
		Delay_ms(1);

		return false;
	}
	//FPGA_LowPower(0);
			/*
	* 读卡间低功耗
	*/
	#if readlow
		FPGA_LowPower(0);
		m_pRfOper->resetAA();
		m_pRfOper->resetCurrentSpec();
	#endif
//	//以单线程方式重写代码
	m_pHBRfOper->setFPGASingleStart();
	
	if(m_pHBRfOper->m_AntChangeDelay>=SWITCHANT_DELAY*2){
			//下发指令
			m_pHBRfOper->stopRfOper();
			//配置要切换的天线和频率的射频参数，此函数并不会立即改变配置
			m_pHBRfOper->setSelectRfParam();
			//将上述配置发往射频，如参数未变动，则不会发送。
//			if(lowpower_change == 1)//首次退出低功耗需要配置等待
//			{
//				lowpower_change = 0;
//				//Delay_ms(2500);
//			}
			m_pHBRfOper->sendSelectRfParam(SETRFWORK_ANT);
			m_pHBRfOper->sendSelectRfParam(SETRFWORK_FREQ);


	}
	//配置安全模块及射频工作参数，如工作参数未变动，则不重新配
	//m_pHBRfOper->printCurrentSelectSpec();
	m_pHBRfOper->setSelectAccessParam(pTagSelectSpec,pTagAccessSpec,antSpecIndex,pRfSpec);
	m_pHBRfOper->sendParamToRf(PARAM_AB,0);	

	if(m_pHBRfOper->isPrivateWriteOper==true){
		m_pHBRfOper->setPrivateWriteAB();
		
	}

	
	m_pHBRfOper->startRfOper();
	rfOper_OvertimerCount = 50;//本轮结束的超时控制
	if(pTagSelectSpec->hasContinueOperation==false){ //无continue操作
		//while(m_pHBRfOper->isFPGARWOver()==false&&m_pHBRfOper->isFPGATurnEnd()==false){						
		while(m_pHBRfOper->isFPGATurnEnd()==false && rfOper_OvertimerCount > 0){ //一轮未结束，一直循环
			
			if(m_pHBRfOper->isFPGARWOver()==true){
				
				//发出本张标签操作结束的信号
				this->m_pHBRfOper->setFPGASingleEnd();
				
				if(m_pHBRfOper->m_pM2SecComm->isSecReadyForComm()){
						STRUCT_M2SEC_FRAME *pframe = m_pHBRfOper->readTagData();
						while(pframe!=NULL){
							
							//printf("0:read one frame!\n");
							//MCUToSecureComm::printfSecFrame(pframe);
							
							this->m_TagObTagNCount++;
							this->m_TagObTLastUTC = General_Functions::getCurrentUTC();
							OpspecResult *popresult = new OpspecResult();												
							popresult->pOpResultFrame = pframe;
							popresult->setOpParams(pTagSelectSpec);	
							
							if(pframe->DataBuf[0] == 0)	
							{
									//Beep_Delay(100);
								beepcount++;
							}
							if(this->getOpResultSize()<10){
							//if(0){	
								this->pushOpResult(popresult);	
							}else{
								delete popresult;
							}

							if(m_pHBRfOper->m_pM2SecComm->isSecReadyForComm()){
									pframe = m_pHBRfOper->readTagData();
							}else{
									break;
							}
						}//end of while
				}else{
					//Delay_ms(2);				
				}
			
				select_count++;
				if(select_count>=1){
					break;
				}
			}

			Delay_ms(3);		
			m_pHBRfOper->setFPGASingleStart(); //配置下一次标签操作开始信号
			
			
		}//end of while
		
		if(this->m_pHBRfOper->isFPGATurnEnd()==true||select_count>=1){
			
			m_pRfOper->stopRfOper();
			//配置要切换的天线和频率的射频参数
			m_pHBRfOper->setSelectRfParam();

			m_pHBRfOper->sendSelectRfParam(SETRFWORK_FREQ);
			
			Delay_ms(3);
			m_pHBRfOper->setFPGASingleStart(); //配置下一次标签操作开始信号
			
		}
		
		//读取可能未读出的数据
		if(m_pHBRfOper->m_pM2SecComm->isSecReadyForComm()){

			STRUCT_M2SEC_FRAME *pframe = m_pHBRfOper->readTagData();
			if(pframe!=NULL){
				//printf("2:read one frame!\n");
				//MCUToSecureComm::printfSecFrame(pframe);
				this->m_TagObTagNCount++;
				this->m_TagObTLastUTC = General_Functions::getCurrentUTC();
				OpspecResult *popresult = new OpspecResult();												
				popresult->pOpResultFrame = pframe;
				popresult->setOpParams(pTagSelectSpec);	

				if(pframe->DataBuf[0] == 0)	
				{
					//Beep_Delay(100);	
					beepcount++;
				}									
				if(this->getOpResultSize()<10){
				//if(0){	
					this->pushOpResult(popresult);	
				}else{
					delete popresult;
				}

			}
		}//end of if
		
	  for(int i=0;i<beepcount;i++){
			Beep_Delay(10);
			Delay_ms(10);
		}
		
				
	}else{//有continue操作
		
		
		
		if(m_pHBRfOper->isPrivateWriteOper==true){ //私有写只能操作一次，安全模块就会退出叙有写模式。

			while(m_pHBRfOper->isFPGATurnEnd()==false){
				if(m_pHBRfOper->isFPGARWOver()==true){ //读写结束
					m_pHBRfOper->isPrivateWriteOper = false;
					Delay_ms(2);
					if(m_pHBRfOper->m_pM2SecComm->isSecReadyForComm()){	
						STRUCT_M2SEC_FRAME *pframe = m_pHBRfOper->getPrivateWriteACK();
						if(pframe!=NULL){
						
							//MCUToSecureComm::printfSecFrame(pframe);
							if(pframe->DataBuf[0] == 0)	
							{
								//Beep_Delay(50);
								beepcount++;
							}else{
								//MCUToSecureComm::removeFrame(pframe);
								Delay_ms(100);
								Beep_Delay(250);
								//continue;
							}
						}else{
						}

						//发出本张标签操作结束的信号
						this->m_pHBRfOper->setFPGASingleEnd();
						Delay_ms(2);
						this->m_pHBRfOper->setFPGASingleStart();
				
						if(m_pCurrentSelectAccessResult!=NULL){
							OpspecResult *popres = new OpspecResult();
							popres->pOpResultFrame = pframe; //注意，这里有可能是空
							popres->setOpParams(pTagSelectSpec,m_pCurrentPrivateWriteSpec);
							popres->operTagTID = m_CurrentPrivateWriteTID; //写结果中，必须保存的操作标签的TID号。
							popres->operAntennaID = m_CurrentPrivateWriteAntID;
							m_pCurrentSelectAccessResult->ptagAccessOpResults->push_back(popres);
							
							this->pushSelectAccessResult(m_pCurrentSelectAccessResult);
							//delete m_pCurrentSelectAccessResult;
							m_pCurrentSelectAccessResult = NULL;
						}else{ //2018.11.26新加（实际可能不会出现这种状态）
							if(pframe!=NULL){
								//printf("b1....\n");
								MCUToSecureComm::removeFrame(pframe);
							}
						}
					} //end of if(m_pHBRfOper->m_pM2SecComm->isSecReadyForComm())
					if(m_pCurrentSelectAccessResult!=NULL){
						//printf("b2....\n");
						delete m_pCurrentSelectAccessResult;
						m_pCurrentSelectAccessResult = NULL;
					}
					select_count++;
					if(select_count>=1){
						break;
					}
				}//end of if(m_pHBRfOper->isFPGARWOver()==true)
			}//end of while (m_pHBRfOper->isFPGATurnEnd()==false)
		}else{/////////////////////////////////////////////
			bool isfirstresult = true;
			while(m_pHBRfOper->isFPGATurnEnd()==false){
				
				if(m_pHBRfOper->isFPGARWOver()==true){ //读写结束

						Delay_ms(2);
						//读写结束时，把Select操作的数据读出来
						if(m_pHBRfOper->m_pM2SecComm->isSecReadyForComm()){

							STRUCT_M2SEC_FRAME *pframe = m_pHBRfOper->readTagData();
							
							if(pframe!=NULL){
								//Delay_ms(100);
								//Beep_Delay(50);
								if(pframe->DataBuf[0] == 0)	
								{
									//Beep_Delay(50);
									beepcount++;
								}
								//printf("1:!\n");
								this->m_TagObTagNCount++;
								this->m_TagObTLastUTC = General_Functions::getCurrentUTC();
								
								if(m_pHBRfOper->isPrivateWriteOper==false){
									if(m_pCurrentSelectAccessResult!=NULL){
										delete m_pCurrentSelectAccessResult;
										m_pCurrentSelectAccessResult = NULL;
									}
									m_pCurrentSelectAccessResult = new Tag_SelectAccessResult();
									m_pCurrentSelectAccessResult->ptagSelectResult = this->genSelectResult(pframe,pTagSelectSpec);
									m_pCurrentSelectAccessResult->ptagAccessOpResults = NULL;
								}else{
									MCUToSecureComm::removeFrame(pframe);
								}
							}
						}//end of if(m_pHBRfOper->m_pM2SecComm->isSecReadyForComm())
						
						if(m_pCurrentSelectAccessResult!=NULL){
							STRUCT_M2SEC_FRAME *pframe = m_pCurrentSelectAccessResult->ptagSelectResult->at(0)->pOpResultFrame;
						
							if(pframe!=NULL){
								if(pframe->DataBuf[0]==0){ //读回的数据无错则继续进行continue操作。
									//printf("continue operation...........................\n");
									//Beep_Delay(5);
									if(m_pCurrentSelectAccessResult->ptagAccessOpResults==NULL){
											m_pCurrentSelectAccessResult->ptagAccessOpResults = new vector<OpspecResult*>;
									}
									llrp_u8v_t tid = MCUToSecureComm::getTIDFromTagFrame(pframe);
									int antennaid = (pframe->DataBuf[9]&0x03) + 1;

									//遍历该规则所附的所有TagAccess，依次根据匹配条件执行Access
									for(int i=0;i<pTagSelectSpec->tagAccessSpecs.size();i++){
										CAccessSpec *paccess = pTagSelectSpec->tagAccessSpecs.at(i)->getAccessSpec();
										//printf("check Access state...........................\n");
										if(paccess->getAccessCurrentState()==AccessCurrentState_Active){
											//printf("Select[%d],Access[%d] operation started....\n",pTagSelectSpec->getSelectSpec()->getSelectSpecID(),paccess->getAccessSpecID());
											CAccessCommand *paccesscmd = paccess->getAccessCommand();
											if(paccesscmd==NULL){
												continue;
											}
											CHbMatchSpec *pmatch = paccesscmd->getHbMatchSpec();
											bool ismatch = false;

											ismatch = true;
											if(ismatch==false){
												continue;
											}

											//accesscommand 开始
											for(list<CParameter*>::iterator it=paccesscmd->beginOpSpec();
													it!=paccesscmd->endOpSpec();it++){
													//printf("d3...........................\n");
												CParameter *opspec = *it;
												if(m_pCurrentSelectAccessResult->ptagAccessOpResults==NULL){
														m_pCurrentSelectAccessResult->ptagAccessOpResults = new vector<OpspecResult*>;
												}
												switch(opspec->m_pType->m_TypeNum+PARAM_BASE){
												case HbReadSpec:
												{												
													//printf("start bank read...........................\n");
													STRUCT_M2SEC_FRAME *pframe = this->m_pHBRfOper->readTagBankData(tid,(CHbReadSpec*)opspec);
													if(pframe->DataBuf[0] == 0)	
													{
														//Beep_Delay(50);
														beepcount++;
													}
													this->genAccessResult(pframe,pTagSelectSpec,opspec,m_pCurrentSelectAccessResult->ptagAccessOpResults);
													//printf("bank read end.......................\n");
													m_pRfOper->waitFPGARWOver();
													this->pushSelectAccessResult(m_pCurrentSelectAccessResult);
													m_pCurrentSelectAccessResult = NULL;
													//发出本张标签操作结束的信号
													//this->m_pHBRfOper->setFPGASingleEnd();
												}
														break;
												case HbWriteSpec:
												{											
													//printf("start bank write...........................\n");
													STRUCT_M2SEC_FRAME * pframe = m_pHBRfOper->writeTagBankData(tid,(CHbWriteSpec*)opspec);
													OpspecResult *popres = new OpspecResult();
													popres->pOpResultFrame = pframe; //注意，这里有可能是空
													popres->setOpParams(pTagSelectSpec,opspec);
													popres->operTagTID = tid; //写结果中，必须保存的操作标签的TID号。
													popres->operAntennaID = antennaid;
													m_pCurrentSelectAccessResult->ptagAccessOpResults->push_back(popres);
													
													m_pRfOper->waitFPGARWOver();
													
													this->pushSelectAccessResult(m_pCurrentSelectAccessResult);
													m_pCurrentSelectAccessResult = NULL;
													
													//MCUToSecureComm::printfSecFrame(pframe);
													if(pframe->DataBuf[0] == 0)	
													{
														//Beep_Delay(50);
														beepcount++;
													}
													//发出本张标签操作结束的信号
													//this->m_pHBRfOper->setFPGASingleEnd();
													//printf("bank write end.......................\n");
												}
													break;
												case HbPrivateWriteSpec: //私有写则是配置安全模块为“专用写分区”状态
												{
													//发出本张标签操作结束的信号
													this->m_pHBRfOper->setFPGASingleEnd();
													Delay_ms(2);
													this->m_pHBRfOper->setFPGASingleStart();
																									
													if(isfirstresult==true){
														isfirstresult = false;
														m_pHBRfOper->isPrivateWriteOper = true; 
														//生成专用写分区帧
														pprivatewritespec = (CHbPrivateWriteSpec *)opspec;
														

														m_pCurrentPrivateWriteSpec = pprivatewritespec;
														m_CurrentPrivateWriteTID = tid;
														m_CurrentPrivateWriteAntID = antennaid;
													}											

												}					
													break;
												}//end of switch
												if(m_pHBRfOper->isPrivateWriteOper==true){ //2018.12.17
													break;  //退出循环													
												}
												
											}//end of accesscommand for
											//发出本张标签操作结束的信号
											//2018.12.17移动这里，以便多个ACCESSSPEC可被执行
											this->m_pHBRfOper->setFPGASingleEnd();
											Delay_ms(2);
											this->m_pHBRfOper->setFPGASingleStart();
											
										}else{
											printf("Access is not active....................!\n");
										}

									}//end of for

								}else{ //select有错误，则上报错误结果
										m_pCurrentSelectAccessResult->ptagAccessOpResults = NULL;
										this->pushSelectAccessResult(m_pCurrentSelectAccessResult);
										//delete m_pCurrentSelectAccessResult;
										m_pCurrentSelectAccessResult = NULL;
								}
							}//end if(pframe!=NULL)

						}//end if m_pCurrentSelectAccessResult!=NULL	
					
				
						select_count++;
						if(select_count>=1){
							break;							
						}
				}//end of if(m_pHBRfOper->isFPGARWOver()==true)
				
			}//end of while(m_pHBRfOper->isFPGATurnEnd()==false)
			
		}//end of else
		
		if(this->m_pHBRfOper->isFPGATurnEnd()==true||select_count>=1){
			
			m_pRfOper->stopRfOper();
			//配置要切换的天线和频率的射频参数
			m_pHBRfOper->setSelectRfParam();

			m_pHBRfOper->sendSelectRfParam(SETRFWORK_FREQ);
			
			Delay_ms(3);
			m_pHBRfOper->setFPGASingleStart(); //配置下一次标签操作开始信号（此信号作用仅是拉低，FPGA在当前标签结束后，即可开始下一轮盘点）
			
		}
		
		//读取可能未读出的数据，并直接抛弃
		if(m_pHBRfOper->m_pM2SecComm->isSecReadyForComm()){

			STRUCT_M2SEC_FRAME *pframe = m_pHBRfOper->readTagData();
			if(pframe!=NULL){
				MCUToSecureComm::removeFrame(pframe);
			}
		}
		
	  for(int i=0;i<beepcount;i++){
			Beep_Delay(50);
			Delay_ms(50);
		}
		if(m_pHBRfOper->isPrivateWriteOper==true&&pprivatewritespec!=NULL){

			STRUCT_M2SEC_FRAME *pprivateframe = m_pHBRfOper->sendPrivateWriteRequest(pprivatewritespec);
			if(pprivateframe!=NULL){
				//Beep_Delay(200);
				printf("Send private write rquest success!\n");
				if(pprivateframe->OperId==1){
					if(pprivateframe->DataBuf[0]==0){
						Delay_ms(100);
						Beep_Delay(50);		
						MCUToSecureComm::removeFrame(pprivateframe); //2018.11.09新加，删除帧，不删除则有内存泄漏
						return false;
					}else{ //2018.12.12新加，如配置失败，则直接结束写卡操作
						Delay_ms(100);
						Beep_Delay(250);	
						if(m_pCurrentSelectAccessResult!=NULL){
							OpspecResult *popres = new OpspecResult();
							popres->pOpResultFrame = pprivateframe; //注意，这里有可能是空
							popres->setOpParams(pTagSelectSpec,m_pCurrentPrivateWriteSpec);
							popres->operTagTID = m_CurrentPrivateWriteTID; //写结果中，必须保存的操作标签的TID号。
							popres->operAntennaID = m_CurrentPrivateWriteAntID;
							m_pCurrentSelectAccessResult->ptagAccessOpResults->push_back(popres);
							
							this->pushSelectAccessResult(m_pCurrentSelectAccessResult);
							//delete m_pCurrentSelectAccessResult;
							m_pCurrentSelectAccessResult = NULL;
						}else{ //2018.11.26新加（实际可能不会出现这种状态）
							if(pprivateframe!=NULL){
								//printf("b1....\n");
								MCUToSecureComm::removeFrame(pprivateframe);
							}
						}
						m_pHBRfOper->isPrivateWriteOper = false;
						return true; 
					}						
				}else{
						if(m_pCurrentSelectAccessResult!=NULL){
							delete m_pCurrentSelectAccessResult;
							m_pCurrentSelectAccessResult = NULL;
						}
						m_pHBRfOper->isPrivateWriteOper = false;
						MCUToSecureComm::removeFrame(pprivateframe); //2018.11.09新加，删除帧，不删除则有内存泄漏
						return true; 									
					
				}
				
				
			}else{						
				printf("Send private write request fail!\n");
				Delay_ms(100);
				Beep_Delay(250);	
				if(m_pCurrentSelectAccessResult!=NULL){
					this->pushSelectAccessResult(m_pCurrentSelectAccessResult);
					m_pCurrentSelectAccessResult = NULL;
				}
				m_pHBRfOper->isPrivateWriteOper = false;

				return true; 
			}
			

		}		
		
		
	}	//end of if
		/////////////////////////////////////////////////////	

		
	//执行完退出前，删除可能未放入队列的操作结果。
	if(m_pCurrentSelectAccessResult!=NULL){
		//printf("b2....\n");
		delete m_pCurrentSelectAccessResult;
		m_pCurrentSelectAccessResult = NULL;
	}
	
	return true;
}



void OperationSpec_Execute_HB::run(){
    OperationSpec_Execute_Base::run();
}


