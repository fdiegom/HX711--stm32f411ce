#include "stm32f411xe.h"
#include <stdbool.h>

  uint8_t shiftInSlow(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);
	
	void HX711();
	
	void begin(uint8_t dout, uint8_t pd_sck, uint8_t gain,char p);
	
	bool is_ready();
	
	void wait_ready();
	
	bool wait_ready_retry(int retries , unsigned long delay_ms );
	
	bool wait_ready_timeout(unsigned long timeout , unsigned long delay_ms);
	
	void set_gain(uint8_t gain);
	
	long read();
	
	long read_average(uint8_t times);
	
	double get_value(uint8_t times);
	
	float get_units(uint8_t times);
	
	void tare(uint8_t times);
	
	void set_scale(float scale);
	
	float get_scale();
	
	void set_offset(long offset );
	
	long get_offset();
	
	void power_down();
	
	void power_up();