

//the functions

#include "seven_segs.h"
//#include <stm32f10x.h>
#include <avr/io.h>
#include <util/delay.h>

//inline init_digit_pins(void);

const uint16_t digit_bits[] = { DIG_0, DIG_1, DIG_2, DIG_3 };
//const uint8_t  num_digits = sizeof(digit_bits)/2;
const uint8_t num_digits = 3;

const uint8_t number_seg_bytes[] = {
//       unconfigured
//ABCDEFG^
0b11111100,//0
0b01100000,//1
0b11011010,//2
0b11110010,//3
0b01100110,//4
0b10110110,//5
0b10111110,//6
0b11100000,//7
0b11111110,//8
0b11100110,//9
0b10011110, //'E' for error
};

//uint8_t SPI_out_byte;
uint16_t digits_out;
uint8_t out_byte;

void init_digit_pins(void){
    
    //setup bit 0 in DDRA for output for digit 3
    //DDRA |= DIG_3;
    //setup bits 0-2 in DDRB for output for digits 0-2
    DDRB |= (DIG_0|DIG_1|DIG_2);
    
}

void init_segment_pins(void){
    //setup all segs as output
    SEGMENT_DDR |= ALL_SEGS;
}


void write_digit(int8_t num, uint8_t dig){
	uint8_t k;
    
    //put SS/CS low
    //GPIOA->BRR |= (1<<4);
	if((num < 10) && (num >= 0)){
    out_byte = number_seg_bytes[num];
    } else {
    out_byte = number_seg_bytes[10];
    }
    
    SEGMENT_PORT = out_byte;
    //write_segs(out_byte);
    
	//write_SPI1(SPI_out_byte);
    //SPI1->DR = SPI_out_byte;
	for( k = 0; k < num_digits; k++){
		//uint8_t digit_port = (dig==3) ? PORTA : PORTB;
        
        if ( k == dig ){
			//if(dig==3)
           // {
            //    PORTA |= digit_bits[k];
            //}else{
                PORTB |= digit_bits[k];
            //}
            //digit_port |= digit_bits[k];
		} else {
			//digits_out &= ~(digit_bits[k]);
            //GPIOC->BRR |= (digit_bits[k]);
            //digit_port &= ~(digit_bits[k]);
            //if(dig==3)
            //{
            //    PORTA &= ~digit_bits[k];
            //}else{
                PORTB &= ~digit_bits[k];
            //}
        }
	}
    //GPIOA->BSRR |= (1<<4); //put SS/CS high again to latch shift register
	//_delay_ms(1);
    //Delay(1);
}

void msg_error(void){
	write_digit(10, 0);
}

void write_number(int16_t number){
		uint8_t h;
		int16_t format_num = number;
		//check if number is too big ot not
		if ((number < 1000) && (number >= 0)){
			//formats number based on digits to correct digits on display
			for(h=0;h < num_digits;h++){
				write_digit(format_num % 10, h);
				format_num /= 10;
			}         
		} else {
			msg_error();
		}
}

/*void write_segs(uint8_t byte){
    
    SEGMENT_PORT = byte;
    
}*/
