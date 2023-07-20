#include "stm32f411xe.h"
#include "systick_timer.h"

void systick_init(){

	SysTick->CTRL=0;
	SysTick->VAL=0;
	SysTick->CTRL |=SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
}


void delaymili(){
	SysTick->LOAD =100000;
	SysTick->VAL=0;

	while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)==0);

}

void delaymicro(){
	SysTick->LOAD =100;
	SysTick->VAL=0;

	while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)==0);

}



int i_c;


void delayms(int D){
	for(i_c=0;i_c<D;i_c++){
	delaymili();
	}
}

void delayus(int D){
	for(i_c=0;i_c<D;i_c++){
	delaymicro();
	}
}