//program to test a library I found for the ht1632c
//just makes a line travel across the display

#include <avr/io.h>
//#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "ht1632c.h"
#include <avr/interrupt.h>
#include "seven_segs.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#define X_AXIS_LEN 32
#define Y_AXIS_LEN 8

#define BUTTON_BIT 6
#define BUTTON_DDR DDRB
#define BUTTON_PORT PORTB
#define BUTTON_PIN PINB

#define DO_YOU_WANT_BUTTON_INT0 1

#define DO_YOU_WANT_DEBUG 0
#define DEBUG_DDR DDRA
#define DEBUG_PORT PORTA
#define DEBUG_BIT_LEN 7 //starting with 0 end with this bit

#define LOW_DIFF_THRESHOLD 42
#define MED_DIFF_THRESHOLD 196

//static uint8_t bright = 0;  /* current LED brightness */
uint8_t fb[X_AXIS_LEN];      /* framebuffer */
//static char buf[8]; /* general purpose */

uint8_t state_storage[32]; //area to store pixel states

volatile uint8_t update_gen_flag = 0;

void clear_fb(void);

void push_fb(void);

void get_new_states(void);
uint8_t get_new_pixel_state(uint8_t in_states[], int8_t x, int8_t y);
uint8_t get_current_pixel_state(uint8_t in[], int8_t x,int8_t y); 

uint8_t get_difference(uint8_t a[],uint8_t b[]);

uint8_t low_diff_count=0;
uint8_t old_low_diff_count=0;

uint16_t med_diff_count=0;
uint16_t old_med_diff_count=0;

volatile uint16_t generation_count=0;

volatile uint8_t timer_overflow_count=0;

#define INIT_BUTTON BUTTON_DDR &= ~(1<<BUTTON_BIT);BUTTON_PORT |= (1<<BUTTON_BIT);

//const uint8_t a_num = 123;

/*inline init_button(void);
inline init_button(void){

    BUTTON_DDR &= ~(1<<BUTTON_BIT);
    //enable pullup
    BUTTON_PORT |= (1<<BUTTON_BIT);
    

}*/

void init_timer0(void);
void init_timer1(void);

void reset_grid(void);

int main(void)
{
    //init stuff
    ht1632c_init();
    
    #if DO_YOU_WANT_BUTTON_INT0==0
    //init_button();
    INIT_BUTTON;
    
    /*BUTTON_DDR &= ~(1<<BUTTON_BIT);
    //enable pullup
    BUTTON_PORT |= (1<<BUTTON_BIT);
    */
    #endif
    
    reset_grid();
    
    //init_timer0();
    init_timer1();
    
    #if DO_YOU_WANT_DEBUG==0
    init_digit_pins();
    init_segment_pins();
    #endif
    
    //init segment stuff
    /*
    //setup bit 0 in DDRA for output for digit 3
    DDRA |= DIG_3;
    //setup bits 0-2 in DDRB for output for digits 0-2
    DDRB |= (DIG_0|DIG_1|DIG_2);
     //setup all segs as output
    SEGMENT_DDR |= ALL_SEGS;
    */
    /*
    #if DO_YOU_WANT_DEBUG
    //for debug
    int8_t l = DEBUG_BIT_LEN;
    while(l>=0){
    DEBUG_DDR |= (1<<l);
    l--;
    }//while(l>=0);
    #endif
    */
    #if DO_YOU_WANT_BUTTON_INT0
    //if you want a button to use INT0 for button on PB6
        
        //setup INT0 to trigger on falling edge
        MCUCR |= (1<<ISC01);
        
        //enable the INT0 external interrupt
        GIMSK |= (1<<INT0);
        
    #endif
    //test glider
    //fb[29] = 0b00100000;
    //fb[30] = 0b00101000;
    //fb[31] = 0b00110000;
    
    uint16_t g_count;
    
    //enable global interrupts
    sei();
    
    while(1){
        
        //uint16_t g_count;
        //if(update_gen_flag){
        //g_count = generation_count;
        //update_gen_flag=0;
        //}
        #if DO_YOU_WANT_DEBUG==0
        //write_number(g_count);
        write_number(generation_count);
        #endif
        //write_number(234);
        
        /*#if DO_YOU_WANT_DEBUG==1
        DEBUG_PORT = ~g_count;
        #endif*/
    }
}

void clear_fb(void){
    uint8_t count;
    for(count=0;count<32;count++){
        fb[count]=0;
    }
}

void push_fb(void){
    uint8_t i=0;
    uint8_t j=0;
    for(i=0;i<32;i++){
        ht1632c_data8(j,fb[i]);
        j+=2;
    }
}
/*
static inline init_button(void){

    BUTTON_DDR &= ~(1<<BUTTON_BIT);
    //enable pullup
    BUTTON_PORT |= (1<<BUTTON_BIT);
    

}*/

void reset_grid(void){

    uint8_t k;
    for(k=0;k<X_AXIS_LEN;k++){
        //int rand=89;
        //uint8_t thing = (uint8_t)rand();
        //rand=(rand*109+89)%251; 
        //fb[k] = ((uint8_t)rand & 0x8f);
        fb[k] = ((uint8_t)rand() & 0xff);
    }
    
    generation_count=0;
}

uint8_t get_current_pixel_state(uint8_t in[], int8_t x,int8_t y){
    
    //for wrapping
    if(x < 0){ x = (X_AXIS_LEN - 1);}
    if(x == X_AXIS_LEN) {x = 0;}
    if(y < 0){ y = (Y_AXIS_LEN-1);}
    if(y == Y_AXIS_LEN) {y = 0;}
    
    return (in[x] & (1<<y));
}

uint8_t get_new_pixel_state(uint8_t in_states[], int8_t x,int8_t y){
    
    uint8_t n=0;
    uint8_t state_out=0;
    //uint8_t p;
    //check on neighbors
    //for(p=y-1;p<y+1;y++){
    //if(get_current_pixel_state(in_states, x-1,p)){n++;}
    //}
    if(get_current_pixel_state(in_states, x-1,y)){n++;}
    if(get_current_pixel_state(in_states, x-1,y+1)){n++;}
    if(get_current_pixel_state(in_states, x-1,y-1)){n++;}
    
    if(get_current_pixel_state(in_states, x,y-1)){n++;}
    if(get_current_pixel_state(in_states, x,y+1)){n++;}
    
    if(get_current_pixel_state(in_states, x+1,y)){n++;}
    if(get_current_pixel_state(in_states, x+1,y+1)){n++;}
    if(get_current_pixel_state(in_states, x+1,y-1)){n++;}
    
    //now determine if dead or alive by neighbors
    if((n<2)){state_out=0;}
    else if ((n<=3) && get_current_pixel_state(in_states, x, y)){state_out=1;}
    else if ((n==3)){state_out=1;}
    else if ((n>3)){state_out=0;}
    
    return state_out;
}

void get_new_states(void){
//find all the new states and put them in the buffer
    
    //copy the current stuff into storage
    int8_t x=X_AXIS_LEN;
    //for(x=0;x<X_AXIS_LEN;x++){
    //    state_storage[x] = fb[x];
    //}
    
    //now get all new states
    //for(x=0;x<X_AXIS_LEN;x++){
    while(x--){
        int8_t y=Y_AXIS_LEN;
        //for(y=0;y<Y_AXIS_LEN;y++){
        while(y--){
            
            if(get_new_pixel_state(fb, x, y)==1){
                //fb[x] |= (1<<y);
                state_storage[x] |= (1<<y);
            } else {
                //fb[x] &= ~(1<<y);
                state_storage[x] &= ~(1<<y);
            }
        }
    }
    uint8_t diff_val= get_difference(state_storage,fb);
    //PORTC = get_difference(state_storage,fb);
    
    /*#if DO_YOU_WANT_DEBUG
    //for debug
    DEBUG_PORT = diff_val;
    #endif*/
    
    //if((get_difference(state_storage,fb)<10)){
    //        reset_grid();
    //}
    
    //if(bit_is_clear(PIND, BUTTON_BIT)|| (diff_val < 2)){
    //    reset_grid();
    //}
    if((diff_val <= 4)){
        low_diff_count++;
    }
    else if((diff_val<=8)){
        med_diff_count++;
    }
    else{
        if(low_diff_count > 0){
        low_diff_count--;
        }
        if(med_diff_count >0){
        med_diff_count--;
        }
    }
    
    #if DO_YOU_WANT_BUTTON_INT0==0
    if(bit_is_clear(BUTTON_PIN, BUTTON_BIT)){
        reset_grid();
    } 
    else 
    #endif
    if(low_diff_count > LOW_DIFF_THRESHOLD){
        low_diff_count=0;
        reset_grid();
    }
    else if(med_diff_count > MED_DIFF_THRESHOLD){
        med_diff_count=0;
        reset_grid();
    }
    
    
    else{
    
    //int8_t x=0;
    //x=X_AXIS_LEN;
    for(x=0;x<X_AXIS_LEN;x++){
    //while(x--){
        //state_storage[x] = fb[x];
        fb[x] = state_storage[x];
    }
    
    //generation_count++;
    }
}

//from other code
uint8_t get_difference(uint8_t a[],uint8_t b[])
{
	uint8_t x_v,y_v,diff=0;

	for(x_v=0; x_v < X_AXIS_LEN; x_v++)
	//while(x_v--)
    {
		for(y_v=0; y_v < Y_AXIS_LEN; y_v++)
		//while(y_v--)
        {
			if(( (get_current_pixel_state(a,x_v,y_v)==1) && (get_current_pixel_state(b,x_v,y_v) == 0)) 
            || ((get_current_pixel_state(a,x_v,y_v)==0) && (get_current_pixel_state(b,x_v,y_v)==1)))
            {
            diff++;
            }
		}
	}
	return diff;
}

void init_timer0(void){
    
    //setup prescaler to CK/1024
    TCCR0 |= ( (1<<CS02) | (1<<CS00) );
    //enable timer0 overflow interrupt
    TIMSK |= (1<<TOIE0);
    
}


void init_timer1(void){
    
    //set prescaler to CK/16384
    TCCR1B |= ((1<<CS13)|(1<<CS12)|(1<<CS11)|(1<<CS10));
    
    //enable timer1 overflow interrupt
    TIMSK |= (1<<TOIE1);
    
}

//----ISRs-----


ISR(TIMER1_OVF1_vect){
    //timer overflow
    /*
    //counts times, so it nows when to actually update the display.
    //because it can't every time it overflows because it would be
    //around 30FPS, and the game of life would go way too fast.
    if(timer_overflow_count<15){
        timer_overflow_count++;
    } */
    //else{
        //timer_overflow_count=0;//reset the count
        generation_count++;
        //push framebuffer to the display
        //push_fb();
        //uint8_t i=0;
        uint8_t i;
        for(i=0;i<32;i++){
            ht1632c_data8((i*2),fb[i]);
        }
        
        //generation_count++;
        //get the new states and add them to the framebuffer,
        //or reset the display if there isn't enough action
        get_new_states();
        update_gen_flag=1;
    //}
    //update_gen_flag=1;
}


#if DO_YOU_WANT_BUTTON_INT0
//if you want a button to use INT0 for button on PB6

ISR(INT0_vect){
//INT0 ISR
    
    //reset and randomize
    reset_grid();
}

#endif
