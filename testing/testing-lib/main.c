//program to test a library I found for the ht1632c


#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "ht1632c.h"

static uint8_t bright = 0;  /* current LED brightness */
static uint8_t fb[32];      /* framebuffer */
static char buf[8]; /* general purpose */


int main(void)
{
    //init stuff
    ht1632c_init();
    
    
    while(1){
        ht1632c_clear_fb(fb);
        ht1632c_data8(0x04, 0xff);
        //_delay_ms(10);
        ht1632c_flush_fb(fb);
        //_delay_ms(10);
    }
}

