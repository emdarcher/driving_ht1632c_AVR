//header file with seven segment stuff

#ifndef SEVEN_SEGS
#define SEVEN_SEGS

#include <stdint.h>

/*
segments
 ---A---
|       |
F       B
|___G___|
|       |
E       C
|___D___| O dp
*/

/*
 * pinout of an individual display
 *      |------|
 * GND--|  __  |--  A
 * F  --| |  | |--  B
 * G  --| |--| |--  C
 * E  --| |__| |-- dp
 * D  --|_____O|--GND
 * 
 * 
 * the display is common cathode
 */

//these are the bits of the shift register
//that go to particular segments
#define SEG_A (1<<7)
#define SEG_B (1<<6)
#define SEG_C (1<<5)
#define SEG_D (1<<4)
#define SEG_E (1<<3)
#define SEG_F (1<<2)
#define SEG_G (1<<1)

#define ALL_SEGS ( SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G )

//make sure to handle digit selection in software

#define DIG_0 (1<<0)
#define DIG_1 (1<<1)
#define DIG_2 (1<<2)
#define DIG_3 (1<<3)

//remember to add any newly defines digits here
#define ALL_DIGS ( DIG_0 | DIG_1 | DIG_2 | DIG_3 )

extern const uint16_t digit_bits[];
extern const uint8_t  num_digits;

extern const uint8_t number_seg_bytes[];

//extern uint8_t SPI_out_byte;
extern uint16_t digits_out;
extern uint8_t out_byte;

//void init_SPI1(void);
void init_digit_pins(void);
//void write_SPI1(uint8_t out_byte);
void msg_error(void);

void write_number(int16_t number);
void write_digit(int8_t num, uint8_t dig);

void write_segs(uint8_t byte);

#endif
