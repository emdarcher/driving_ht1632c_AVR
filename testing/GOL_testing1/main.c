//program to test a library I found for the ht1632c
//just makes a line travel across the display

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "ht1632c.h"
#include <avr/interrupt.h>

#define X_AXIS_LEN 32
#define Y_AXIS_LEN 8

#define BUTTON_BIT 2
#define BUTTON_DDR DDRD
#define BUTTON_PORT PORTD

#define DIFF_DEBUG_DDR DDRC
#define DIFF_DEBUG_PORT PORTC
#define DIFF DEBUG_BIT_LEN 6

#define LOW_DIFF_THRESHOLD 20

static uint8_t bright = 0;  /* current LED brightness */
static uint8_t fb[X_AXIS_LEN];      /* framebuffer */
static char buf[8]; /* general purpose */

static uint8_t state_storage[32]; //area to store pixel states

void clear_fb(void);

void push_fb(void);

void get_new_states(void);
uint8_t get_new_pixel_state(uint8_t in_states[], int8_t x, int8_t y);
uint8_t get_current_pixel_state(uint8_t in[], int8_t x,int8_t y); 

uint8_t get_difference(uint8_t a[],uint8_t b[]);

uint8_t low_diff_count=0;
uint8_t old_low_diff_count=0;

void init_button(void);

void reset_grid(void);

int main(void)
{
    //init stuff
    ht1632c_init();
    
    //put some stuff on it
    /*uint8_t k;
    for(k=0;k<X_AXIS_LEN;k++){
        int rand=89;
        uint8_t thing = (uint8_t)rand();
        rand=(rand*109+89)%251; 
        fb[k] = ((uint8_t)rand & 0x8f);
        
    }*/
    init_button();
    reset_grid();
    
    
    DDRC = 0xff;
    
    
    //test glider
    //fb[29] = 0b00100000;
    //fb[30] = 0b00101000;
    //fb[31] = 0b00110000;
    
    while(1){
        /*char c;
        for(c = 0;c<32;c+=1){
            clear_fb();
            fb[c]=0xff;
            //ht1632c_clear_fb(fb);
            push_fb();
            
            _delay_ms(100);
            //ht1632c_flush_fb(fb);
            //_delay_ms(10);
        
        }*/
        push_fb();
        //if(bit_is_set(PIND, BUTTON_BIT) || (get_difference(fb,state_storage)<2)){
        //    reset_grid();
        //}
        
        get_new_states();
        
        _delay_ms(500);
        
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

void init_button(void){

    BUTTON_DDR &= ~(1<<BUTTON_BIT);
    //enable pullup
    BUTTON_PORT |= (1<<BUTTON_BIT);
    

}

void reset_grid(void){

    uint8_t k;
    for(k=0;k<X_AXIS_LEN;k++){
        //int rand=89;
        uint8_t thing = (uint8_t)rand();
        //rand=(rand*109+89)%251; 
        //fb[k] = ((uint8_t)rand & 0x8f);
        fb[k] = ((uint8_t)rand() & 0xff);
    }


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
    
    //check on neighbors
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
    int8_t x=0;
    //for(x=0;x<X_AXIS_LEN;x++){
    //    state_storage[x] = fb[x];
    //}
    
    //now get all new states
    for(x=0;x<X_AXIS_LEN;x++){
        int8_t y=0;
        for(y=0;y<Y_AXIS_LEN;y++){
            
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
    PORTC = diff_val;
    
    //if((get_difference(state_storage,fb)<10)){
    //        reset_grid();
    //}
    //if(bit_is_clear(PIND, BUTTON_BIT)|| (diff_val < 2)){
    //    reset_grid();
    //}
    if((diff_val <= 4)){
        low_diff_count++;
    }else{
        if(low_diff_count > 0){
        low_diff_count--;
        }
    }
    
    if(bit_is_clear(PIND, BUTTON_BIT)){
        reset_grid();
    } 
    else if(low_diff_count > LOW_DIFF_THRESHOLD){
        low_diff_count=0;
        reset_grid();
    }
    
    else{
    
    //int8_t x=0;
    for(x=0;x<X_AXIS_LEN;x++){
        //state_storage[x] = fb[x];
        fb[x] = state_storage[x];
    }
    }
}

//from other code
uint8_t get_difference(uint8_t a[],uint8_t b[])
{
	uint8_t x_v,y_v,diff=0;

	for(x_v=0; x_v < X_AXIS_LEN; x_v++)
	{
		for(y_v=0; y_v < Y_AXIS_LEN; y_v++)
		{
			if(( (get_current_pixel_state(a,x_v,y_v)==1) && (get_current_pixel_state(b,x_v,y_v) == 0)) 
            || ((get_current_pixel_state(a,x_v,y_v)==0) && (get_current_pixel_state(b,x_v,y_v)==1))){
            
            diff++;}
		}
	}
	return diff;
}
