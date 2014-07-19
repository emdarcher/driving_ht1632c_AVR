//program to test a library I found for the ht1632c
//just makes a line travel across the display

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "ht1632c.h"

static uint8_t bright = 0;  /* current LED brightness */
static uint8_t fb[32];      /* framebuffer */
static char buf[8]; /* general purpose */

void clear_fb(void);

void push_fb(void);

int main(void)
{
    //init stuff
    ht1632c_init();
    
    
    while(1){
        char c;
        for(c = 0;c<32;c+=1){
            clear_fb();
            fb[c]=0xff;
            //ht1632c_clear_fb(fb);
            push_fb();
            
            _delay_ms(100);
            //ht1632c_flush_fb(fb);
            //_delay_ms(10);
        
        }
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
