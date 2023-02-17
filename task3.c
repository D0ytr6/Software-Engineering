#include <lpc214x.h>
#include <stdint.h>
unsigned char flag = 0;
unsigned char bright_flag = 0;

__irq void eint1(void) {
	flag = 1;
	EXTINT |= 0x02; // clear eint1 interupt, control flag register
	VICVectAddr = 0; // End of interrupt execution
}

void init_eint1(void) {
	PINSEL0 |= 0x20000000; // Set 0.14 pin as eint1 interupt
	VICVectCntl0 = 0x0000002f;
	VICVectAddr0 = (unsigned)&eint1;
	VICIntEnable = 0x00008000; 	
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
void show_chest(void){
		for (int y = 1; y < 9; y++){
		for (int x = 1; x < 9; x++){
			if(y % 2 == 0){
				if(x % 2 != 0){
					SPIPutDot(x, y, 1);
				}
			}
			else{
				if(x % 2 == 0){
					SPIPutDot(x, y, 1);
				}
			}
		}
	}
}
void clear_chest(void){
		for (int y = 1; y < 9; y++){
		for (int x = 0; x < 8; x++){
			SPIPutDot(x, y, 0);
		}
	}
}
void delay_ms(uint16_t j) /* Function for delay in milliseconds  */
{
  uint16_t x,i;
	for(i=0;i<j;i++)
	{
    for(x=0; x<6000; x++);    /* loop to generate 1 millisecond delay with Cclk = 60MHz */
	}
}
int main(void)
{
	IODIR0 |= 0x0000FF00;
	IOSET0 |= 0x0000FF00;
	init_eint1();
	while(1) {
		if (flag == 1) {
			IOCLR0 |= 0x0000FF00;
			delay_ms(200);
			
			if(bright_flag == 0){
				show_chest();
				bright_flag = 1;
			}
			else if(bright_flag == 1){
				clear_chest();
				bright_flag = 0;
			}
			IOSET0 |= 0x0000FF00;
			flag = 0;
		}
		
	}

	return 0;
}
