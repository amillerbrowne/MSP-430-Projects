#include <msp430g2553.h>

/*

H // 4 short flashes
E // 1 short flash
L // 1 short - 3 long - 2 short
L // 1 short - 3 long - 2 short
O // 3 long - 3 long - 3 long

A // 1 short - 3 long
L // 1 short - 3 long - 2 short
E // 1 short
X // 3 long - 2 short - 3 long

*/
#define RED 0x01 //Red LED P1.0
#define GREEN 0x40 // Green LED P1.6
#define BUTTON 0x08 // Button P1.3

volatile unsigned int dot;
volatile unsigned int dash;
volatile unsigned int longBlink;
volatile unsigned int blink_counter;
volatile unsigned int morse_counter = 0; //initializes the morse code counter that goes through the array

volatile unsigned char redOrGreen = 0;
volatile unsigned char changeLED = 0; // is set to 1 if the user requests to switch LEDs (i.e. if he presses the button)


//int morse_code[22] = {2,0,2,0,1,3,
//1,0,1,0,1,0,1,3,
//1,0,1,0,1,0,1,4};
// this array holds the message "HELLO ALEX"
int morse_code[55] = {1,0,1,0,1,0,1,3, 1,3, 1,0,2,0,1,0,1,3, 1,0,2,0,1,0,1,3, 2,0,2,0,2,3, 4, 1,0,2,3, 1,0,2,0,1,0,1,3, 1,3, 2,0,1,0,1,0,2,4}
//0 = short pause(dots/dashes); 1 = dot; 2 = dash  3 = long pause(between letters) 4 = extra long pause(words)

volatile unsigned char count = 0; //selects if red (0) or green(1)
unsigned char mask[] = {RED, GREEN}; // bit masks for the two LED pins P1.0 and P1.6


int main(void) {
    WDTCTL = (WDTPW + WDTTMSEL + WDTCNTCL + 0 + 1);

    IE1 |= WDTIE;

    P1DIR &= ~BUTTON; //sets the button in the input direction
    P1OUT |= BUTTON;
    P1REN |= BUTTON; //turns on the resistor of the button


    morse_counter = 0;      //initializes the morse code counter that goes through the array
    dot = 20;               //1 time unit
    dash = 60;              //3 time units
    longBlink = 140;        //7 time units
    blink_counter = dot;    //sets it to 1 time unit

    _bis_SR_register(GIE+LPM0_bits);
}

interrupt void WDT_interval_handler(){
    unsigned char b = (P1IN & BUTTON);                  //reads the button state

    if((redOrGreen == BUTTON) && (b == 0)){             //if it the button goes from high to low
        changeLED = 1;                                  //changes the LED from red to green or vice versa
        P1DIR |= mask[count^1];                         //sets the other LED to the output direction
        P1OUT |= mask[count^1];                         //outputs to either the green or red LED
    }


    if (--blink_counter == 0){
        if (morse_code[morse_counter] == 0){
            P1OUT &= ~1;                                //checks the state that the output is off
            blink_counter = dot;                        //sets it to the dot state for a short pause (1 time unit)
        }
        else if (morse_code[morse_counter] == 1){
            P1OUT |= 1;                                 //turns on the led/steady output
            blink_counter = dot;                        //sets to the dot flashing
        }
        else if (morse_code[morse_counter] == 2){
            P1OUT |= 1;                                 
            blink_counter = dash;                       //sets to the dash interval in time
        }
        else if (morse_code[morse_counter] == 3){
            P1OUT &= ~1;
            blink_counter = dash;                       //sets to the dash (3 time units) so that it is a pause between letters
        }
        else if (morse_code[morse_counter] == 4){
            P1OUT &= ~1;
            blink_counter = longBlink;                  //sets to the long pause (7 time units)
        }

        ++morse_counter;                                //steps the counter through the array
        if (morse_counter > 55) {
            morse_counter = 0;
        }
    }

   redOrGreen = b;

}

ISR_VECTOR(WDT_interval_handler, ".int10")