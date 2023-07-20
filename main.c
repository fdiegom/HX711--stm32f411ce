#include "stm32f411xe.h"
#include "clock_driver.h"
#include "systick_timer.h"
#include "hx711.h"

int main(){
	begin(12,13,128,'b');
	
	double peso;
	long v;
	set_scale(226.173);
	tare(20);
	long off;
	while(1){
		peso=get_units(10);
		v=read();
		
	}
	
	
}