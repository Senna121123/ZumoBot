#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
void initPort();
void wheels(int, int, int);

void initPort(){
    DDRC |= (1 << PORTC7); // Make pin 13 be an output.
    DDRB |= (1 << PORTB1 | 1 << PORTB2 | 1 << PORTB5 | 1 << PORTB6);
    DDRD |= (1<<PORTD7);
    PORTB= ~(1 << PORTB1) | ~(1 << PORTB6);
}

void wheels(int speedLeft, int speedRight, int time){
    int checkDriveBack = 0;

    if(speedLeft < 0 && speedRight < 0){//achteruit 
        checkDriveBack = 1;
    }

    if(speedLeft > 0){/*vooruit*/
        PORTB &= ~(1<<PORTB2);
    }else{/*achteruit*/
        PORTB |= (1<<PORTB2);
        speedLeft = speedLeft * -1;
    }
    if(speedRight > 0){/*vooruit*/
        PORTB &= ~(1<<PORTB1);
    }else{/*achteruit*/
        PORTB |= (1<<PORTB1);
        speedRight = speedRight * -1;
    }

    /*setting speed*/
    OCR1B = speedLeft * 4;
    OCR1A = speedRight * 4;
}



int main(){
    initPort();
    initClckDrive();
    
    while(1){
        wheels(-50, -50, 8000);
        wheels(80, 80, 4000);
        wheels(-40, 40, 2700);
        wheels(80, 80, 4000);
        wheels(-50, -50, 4000);
    }
}
