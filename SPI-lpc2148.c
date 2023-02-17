#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void delay_ms(uint16_t j) /* Function for delay in milliseconds  */
{
  uint16_t x,i;
	for(i=0;i<j;i++)
	{
    for(x=0; x<7000; x++);    /* loop to generate 1 millisecond delay with Cclk = 60MHz */
	}
}
void SPIPutDot (int x, int y, char bright) {
	IOSET0 = (1 << 15);
	// SPDR - SPI Data Register, holds the data to be transmitted/received
	// SPSR - SPI0 Status Register
	if(bright == 1){ //set burning
		S0SPDR = ~(1 << (y - 1)); // Write data in SPDR register
		while ((S0SPSR & (1 << 7)) == 0);	// Waite until SPI transfer not complete, Bit 7 – SPIF
		S0SPDR = ~(1 << (8 - x));
		while ((S0SPSR & (1 << 7)) == 0);	
	}	
	else{ // set clear
		S0SPDR = (1 << (y - 1)); // Write data in SPDR register
		while ((S0SPSR & (1 << 7)) == 0);	// Waite until SPI transfer not complete, Bit 7 – SPIF
		S0SPDR = (1 << (8 - x));
		while ((S0SPSR & (1 << 7)) == 0);	
	}
	IOCLR0 = (1 << 15);	
}

int main(void)
{
	uint32_t result;
	float voltage;
	float step = 3.3 / 8;
	float voltage_values[8];
	for(int i = 1; i < 9; i++){
		voltage_values[i - 1] = step * i;
	}
	
	PINSEL1 = 0x01000000; // P0.28 as AD0.1
	// 00000000001000000000010000000010
	// 4 in CLKDIV CLKDIV = ARM BUS / 4 + 1 = 4.5 Mhz
	// 10 bit = 0 - 1023 variations
	AD0CR = 0x00200402; // 21 bit is ADC operational, 10-bits, 11 clocks for conversion and AD0.1 chanel choose, 
	while(1)
	{
		AD0CR = AD0CR | (1<<24); /* Start Conversion */
		while ( !(AD0DR1 & 0x80000000) ); // wait operations done ADC1 Data Registers
		result = AD0DR1;
		result = (result>>6); // hide reserverd pins
		result = (result & 0x000003FF); // save reserved data
		voltage = ( (result/1023.0) * 3.3 ); // find normalized value, result / max value * max voltage

		for(int i = 0; i < 8; i++){
			if(voltage < voltage_values[i]){
				for (int y = 1; y < i + 2; y++){
					for (int x = 1; x < 9; x++){
						SPIPutDot(x, y, 1);
					}
				}
			}
		}
	}
}
