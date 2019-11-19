#define F_CPU 8000000UL
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd.h"

int lcd_putchar(char c, FILE *unused){
	
	(void) unused;
	
	lcd_putc(c);
	
	return 0;
}


//Setting up ADC0 pin to input for LM32 sensor and converting the input to ADC
int get_LM32_adc(void){
	
	ADMUX &= ~(1 << MUX0);					//Setting pin ADC0 input
	
	ADCSRA |= (1 << ADEN);					//Turning on ADC
	
	ADCSRA |= (1 << ADSC);					//Start conversion from analog to digital
	while (!(ADCSRA & (1 << ADIF)));		//Wait until conversion is complete
	
	ADCSRA &= ~(1 << ADSC);					//Don't try convert more values once first conversion is complete
	
	return ADC;	
}


//Converting the input adc voltage to measured temperature
int LM32_temp(int LM32_adc){
	
	int temp;
	
	temp = (LM32_adc*0.10742);				// conversion factor from : 100/(1024/1.1), where 1.1 is the reference voltage
	
	return temp;
}


//Setting up ADC1 pin to input for diode sensor and converting the input to ADC
int get_diode_adc(void){
	
	ADMUX |= (1 << MUX0);					//Setting to ADC1 input
	
	ADCSRA |= (1 << ADEN);					//Turning on ADC
	
	ADCSRA |= (1 << ADSC);					//Start conversion from analog to digital
	
	while (!(ADCSRA & (1 << ADIF)));		//Wait until conversion is complete
	
	ADCSRA &= ~(1 << ADSC);					//Don't try convert more values once first conversion is complete
	
	return ADC;
}


//Converting the input ADC voltage to measured temperature 
int diode_temp(int diode_adc){
	
	int temp;
	
	temp = -0.6017*diode_adc + 301.6;		//-0.6017, 301.6 are constants calculated experimentally to be comparable to LM32 Temp sensor
	
	return temp;
	
}


//Printing temperatures to the 16x2 LCD
int print_to_lcd(int LM32_adc,int diode_adc ){
	
	printf("LM35 Temp: %d\n",LM32_adc, LM32_temp(LM32_adc));		//Printing temperature sensor data to LCD
	
	_delay_ms(10);
	
	printf("Diode Temp: %d\n", diode_adc, diode_temp(diode_adc));	//Printing diode temperature data to LCD
	
}


FILE lcd_stream = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);



int main(void){
	
	int LM32_adc;
	
	int diode_adc;
	
	stdout = &lcd_stream;					//Changing IO output to display to LCD
	
	lcd_init(LCD_DISP_ON);					//Turning on LCD
	
	lcd_clrscr();							//Clearing LCD screen
	
	ADMUX |= (1 << REFS1) | (1<<REFS0);		//Setting 1.1V reference
	
	while(true){
		
		LM32_adc = get_LM32_adc();			//Returning the input voltage for LM32 sensor
		
		diode_adc = get_diode_adc();		//Returning the input voltage for diode 
		
		print_to_lcd(LM32_adc,diode_adc);	//Printing to LCD
		
		_delay_ms(100);
		
	}
	
	return 0;
}

