#include "main.h"
float CarDirectionAngel;
float Angel[2]={0};
u32 i=0;
void CAN2_Configuration(void)
{
	CAN_InitTypeDef        can;
	CAN_FilterInitTypeDef  can_filter;
	GPIO_InitTypeDef       gpio;
	NVIC_InitTypeDef       nvic;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);


	gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 ;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &gpio);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2); 

	nvic.NVIC_IRQChannel = CAN2_RX0_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	can.CAN_TTCM = DISABLE;
	can.CAN_ABOM = DISABLE;    
	can.CAN_AWUM = DISABLE;       
	can.CAN_RFLM = DISABLE;    
	can.CAN_TXFP = ENABLE;     
	can.CAN_Mode = CAN_Mode_Normal;
	//can.CAN_Mode = CAN_Mode_LoopBack;
	can.CAN_SJW  = CAN_SJW_1tq;
	can.CAN_BS1 = CAN_BS1_9tq;
	can.CAN_BS2 = CAN_BS2_5tq;
	can.CAN_Prescaler = 3;   //CAN BaudRate 45/(1+9+5)/3=1Mbps
	CAN_Init(CAN2, &can);
	
	can_filter.CAN_FilterNumber=14; 
	can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
	can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
	can_filter.CAN_FilterIdHigh=0x0000;
	can_filter.CAN_FilterIdLow=0x0000;
	can_filter.CAN_FilterMaskIdHigh=0x0000;
	can_filter.CAN_FilterMaskIdLow=0x0000;
	can_filter.CAN_FilterFIFOAssignment=0;//the message which pass the filter save in fifo0
	can_filter.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&can_filter);
	
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
}


void CAN2_RX0_IRQHandler(void)
{
	if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET)
	{
		CanRxMsg canRxMsg;
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		CAN_ClearFlag(CAN2, CAN_FLAG_FF0);
		CAN_Receive(CAN2, CAN_FIFO0, &canRxMsg);
		if(i==0)
		{
			Angel[0]= (-0.01) * ((int32_t)(canRxMsg.Data[0]<<24)|(int32_t)(canRxMsg.Data[1]<<16)|(int32_t)(canRxMsg.Data[2]<<8)|(int32_t)(canRxMsg.Data[3]));
			i++;
		}
		else
		{
			i=1;
		}
		Angel[1]= (-0.01) * ((int32_t)(canRxMsg.Data[0]<<24)|(int32_t)(canRxMsg.Data[1]<<16)|(int32_t)(canRxMsg.Data[2]<<8)|(int32_t)(canRxMsg.Data[3]));
		CarDirectionAngel=Angel[1]-Angel[0];
	}
}
