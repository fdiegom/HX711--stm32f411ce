#include "stm32f411xe.h"
#include "hx711.h"
#include <stdbool.h>
#include "systick_timer.h"
#include "clock_driver.h"
	uint8_t PD_SCK; //Pin del clock
	uint8_t DOUT;		//Pin de datos
	uint8_t GAIN;		//ganancia
	char Port;      //periferico GPIOx a usar (x=A y B)
	long Offset=0;	//Offset para tarar
	float Scale=1;	//Scale para obtener las unidades correctas
	
	
	
	uint8_t shiftInSlow(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder){ //registro de desplazamiento de 8 bits con un periodo de 2us
		uint8_t value;
		uint8_t i;
		value=0;
		for(i=0;i<8;i++){
			if(Port=='a'){
						GPIOA->ODR|=1<<clockPin;
						delayus(1);
						if(bitOrder==0){  //registro LSB
							value|=(((GPIOA->IDR)&1<<dataPin)>>dataPin)<<i;  //Obtener solo el bit del IDR para luego desplazarlo i veces.
						}
						else{							//registro MSB
							value|=(((GPIOA->IDR)&1<<dataPin)>>dataPin)<<(7-i); 
						}
						GPIOA->ODR&=~1<<clockPin;
						delayus(1);
			}
			if(Port=='b'){
						GPIOB->ODR|=1<<clockPin;
						delayus(1);
						if(bitOrder==0){  //registro LSB
							value|=(((GPIOB->IDR)&1<<dataPin)>>dataPin)<<i;  //Obtener solo el bit del IDR para luego desplazarlo i veces.
						}
						else{							//registro MSB
							value|=(((GPIOB->IDR)&1<<dataPin)>>dataPin)<<(7-i); 
						}
						GPIOB->ODR&=~1<<clockPin;
						delayus(1);
			}
	}
	return value;
}
	


void begin(uint8_t dout , uint8_t pd_sck, uint8_t gain,char p){
	setRCC_100MHz();
	systick_init();
	Port=p;
	PD_SCK=pd_sck;
	DOUT=dout;
	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOAEN;
	if(Port=='a'){
		//Programar GPIO para la Salida del clock
		GPIOA->MODER|=1<<(2*PD_SCK);
		GPIOA->OTYPER&=~(1<<PD_SCK);
		GPIOA->OSPEEDR|=(3<<(2*PD_SCK));
		
		//Programar GPIO para la entrada de datos(DOUT)
		GPIOA->MODER&=~3<<(2*DOUT);
		GPIOA->PUPDR|=1<<(2*DOUT);
	}
	if(Port=='b'){
		//Programar GPIO para la Salida del clock
		GPIOB->MODER|=1<<(2*PD_SCK);
		GPIOB->OTYPER&=~(1<<PD_SCK);
		GPIOB->OSPEEDR|=(3<<(2*PD_SCK));
		
		//Programar GPIO para la entrada de datos(DOUT)
		GPIOB->MODER&=~3<<(2*DOUT);
		GPIOB->PUPDR|=1<<(2*DOUT);
	}
	set_gain(gain);
}

bool is_ready(){
	if(Port=='b'){
		if((GPIOB->IDR & 1<<DOUT)==0){
			return true;
		}
		else{
			return false;
		}
	}
	else if(Port=='a'){
		if((GPIOA->IDR & 1<<DOUT)==0){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}


void set_gain(uint8_t gain){
	switch(gain){
		case 128:		//factor de ganancia 128
			GAIN=1;
		break;
		case 64:		//factor de ganancia 64
			GAIN=3;
		break;
		case 32:		//factor de ganancia 32
			GAIN=2;
		break;
		
	}
}

long read(){
	wait_ready();
	unsigned long value=0;
	uint8_t data[3]={0};
	uint8_t filler=0x00;
	
	//Para el correcto funcionamiento descativar las interrupciones
	
	data[2]=shiftInSlow(DOUT,PD_SCK,1);
	data[1]=shiftInSlow(DOUT,PD_SCK,1);
	data[0]=shiftInSlow(DOUT,PD_SCK,1);
	
	int i;
	
	//establecer la ganancia para la siguiente lectura
	
	for(i=0;i<GAIN;i++){
		if(Port=='a'){
			GPIOA->ODR|=1<<PD_SCK;
			delayus(1);
			GPIOA->ODR&=~(1<<PD_SCK);
			delayus(1);
		}
		else if(Port=='b'){
			GPIOB->ODR|=1<<PD_SCK;
			delayus(1);
			GPIOB->ODR&=~(1<<PD_SCK);
			delayus(1);
		}
	}
	
	//Hablitar las interrupciones
	
	if(data[2] & 0x80){ //llenar el ultimo dato con 1 o 0 dependiendo si el valor es positivo o negativo
		filler=0xFF;
	}
	else{
		filler=0x00;
	}
	value=(filler<<24|
				 data[2]<<16|
	       data[1]<<8|
				 data[0]);
	
	return value;
}


void wait_ready(){
	while(!(is_ready())){} //esperar a que este listo
}

bool wait_ready_retry(int retries , unsigned long delay_Ms ){
	int count = 0;
	while (count < retries) {
		if (is_ready()) {
			return true;
		}
		delayms(delay_Ms);
		count++;
	}
	return false;
}


long read_average(uint8_t times) {
	long sum = 0;
	uint8_t i;
	for ( i= 0; i < times; i++) {
		sum += read();
	}
	return sum / times;
}

double get_value(uint8_t times) {
	return read_average(times) - Offset;
}

float get_units(uint8_t times) {
	return get_value(times) / Scale;
}

void tare(uint8_t times) { //tarar la balanza mediante el promedio de una cierta cantidad de lecturas
	double sum = read_average(times);
	set_offset(sum);
}

void set_scale(float scale) {
	Scale = scale;
}

float get_scale() {
	return Scale;
}

void set_offset(long offset) {
	Offset = offset;
}

long get_offset() {
	return Offset;
}

void power_down() {
	if(Port=='a'){
		GPIOA->ODR&=~(1<<PD_SCK);
		GPIOA->ODR|=(1<<PD_SCK);
	}
	else if(Port=='b'){
		GPIOB->ODR&=~(1<<PD_SCK);
		GPIOB->ODR|=(1<<PD_SCK);
	}
}

void power_up() {
	if(Port=='a'){
		GPIOA->ODR&=~(1<<PD_SCK);
	}
	else if(Port=='b'){
		GPIOB->ODR&=~(1<<PD_SCK);
	}
}


