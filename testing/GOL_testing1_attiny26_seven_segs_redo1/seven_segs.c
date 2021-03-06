

//the functions

#include "seven_segs.h"
//#include <stm32f10x.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

//inline init_digit_pins(void);

uint8_t digit_bits[] PROGMEM = { DIG_0, DIG_1, DIG_2 };
//const uint8_t  num_digits = sizeof(digit_bits)/2;
const uint8_t num_digits = 3;

//const 
uint8_t number_seg_bytes[]  PROGMEM = {
//       unconfigured
//ABCDEFG^
0b11111101,//0
0b01100000,//1
0b11011011,//2
0b11110011,//3
0b01100110,//4
0b10110111,//5
0b10111111,//6
0b11100001,//7
0b11111111,//8
0b11100111,//9
0b10011111, //'E' for error
};

//uint8_t SPI_out_byte;
//uint16_t digits_out;
//uint8_t out_byte;

void init_digit_pins(void){
    
    //setup bit 0 in DDRA for output for digit 3
    //DDRA |= DIG_3;
    //setup bits 0-2 in DDRB for output for digits 0-2
    DDRB |= (DIG_0|DIG_1|DIG_2);
    
}

void init_segment_pins(void){
    //setup all segs as output
    SEGMENT_DDR |= ALL_SEGS;
    SEGMENT_DDR |= 1<<0;
}


void write_digit(int8_t num, uint8_t dig){
	uint8_t k;
    uint8_t out_byte;
    
    if((num < 10) && (num >= 0)){
    //out_byte = number_seg_bytes[num];
    out_byte = pgm_read_byte(&number_seg_bytes[num]);
    } else {
    
    //out_byte = number_seg_bytes[10];
    out_byte = pgm_read_byte(&number_seg_bytes[10]);
    }
    
    //SEGMENT_PORT = 0x00;
    SEGMENT_PORT = (out_byte>>1);
    //write_segs(out_byte);
    //out_byte=PORTB;
	for( k = 0; k < num_digits; k++){
        
        if ( k == dig ){
                //PORTB |= digit_bits[k];
                PORTB |= pgm_read_byte(&digit_bits[k]);
                //PORTB |= (1<<k);
		} else {
                //PORTB &= ~(digit_bits[k]);
                PORTB &= ~(pgm_read_byte(&digit_bits[k]));
                //PORTB &= ~(1<<k);
        }
	}
    //PORTB |= out_byte;
	_delay_ms(1);
    
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

void write_segs(uint8_t byte){
    uint8_t o;
    for(o=0;o<8;o++){
    if(byte & (1<<o)){
    SEGMENT_PORT |= (1<<o);
    }
    else{
    SEGMENT_PORT &= ~(1<<o);
    }
    }
}
