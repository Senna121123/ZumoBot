#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
void initClckBuzz();
void buzzer(uint8_t);
void initDrive();
void wheels(int, int, int);

void initClckBuzz(){//Init function for Buzzer
    DDRD |= (1<<PORTD7);//Make PortD7 output for PWM
    TCCR4B |= (1<<CS43);//Prescaler 128x
    TCCR4C |= (1<<COM4D1) | (1<<COM4D0);//Comparator mode Set on comparematch
    OCR4C = 166;//Set top of fastPWM
    OCR4D = 83;//Comparator value
}

void buzzer(uint8_t iteraties){//function for Buzzer
    int i;
    for (i = 0; i < iteraties; i++){
        TCCR4C |= (1<<PWM4D);//Zet de buzzer aan op fastPWM
        _delay_ms(500);
        TCCR4C &= ~(1<<PWM4D);//Zet de buzzer uit fastPWM
        _delay_ms(500);
    }
}

void initDrive(){
    //Timer 1 Configuration
    //Prescaler: clock I/O / 1
    //Output enabled
    //Phase-correct PWM
    //top of 400
    //
    //PWM frequency calculation
    //16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20KHz

    TCCR1A |= (1<<COM1A1 | 1 <<COM1B1);//Set OCnA/OCnB/OCnC on compare match when upcounting, clear on compare match when downcounting
    TCCR1B |= (1<<CS12);//Prescaler 256
    TCCR1B |= (1<<WGM13);//PWM Phase- and Frequency Correct
    ICR1 = 400;//TOP of PWM
    /*PORTB1: Right motor direction  |  PORTB2: Left motor direction  |  PORTB5: Right motor PWM  |  PORTB6: Left motor PWM*/
    DDRB |= (1 << PORTB1 | 1 << PORTB2 | 1 << PORTB5 | 1 << PORTB6);//Set ports of motor to output
    OCR1A = 0;//OutputCompareRegister --> Right tire still
    OCR1B = 0;//OutputCompareRegister --> Left tire still
    PORTB= ~(1 << PORTB1) | ~(1 << PORTB2);//Default direction drive forward
}

void wheels(int speedLeft, int speedRight, int time){
    int checkDriveBack = 0;//Checks if robot drives backwards
    int loopDriveBack = 0;//Variable used for buzzer on driving backwards

    if(speedLeft < 0 && speedRight < 0){//Zumo drives backwards
        checkDriveBack = 1;//Variable used to tell if zumo drives backwards
    }

    if(speedLeft > 0){/*forward left tire*/
        PORTB &= ~(1<<PORTB2);
    }else{/*backward left tire*/
        PORTB |= (1<<PORTB2);
        speedLeft = speedLeft * -1;
    }
    if(speedRight > 0){/*forward right tire*/
        PORTB &= ~(1<<PORTB1);
    }else{/*backward right tire*/
        PORTB |= (1<<PORTB1);
        speedRight = speedRight * -1;
    }

    /*setting speed*/
    OCR1B = speedLeft * 4;//Set the speed of the Left tire
    OCR1A = speedRight * 4;//Set the speed of the Right tire
    if (time != 0){//If zumo did not get the operation of driving without a time limit
        if (checkDriveBack == 1 && time >= 1000){//Check if robot drives backwards and the time 
            loopDriveBack = time / 1000;//Calculate the number of seconds
            buzzer(loopDriveBack);//Buzzer function for driving backwards
            _delay_ms(time - (1000*loopDriveBack));//Wait left over seconds before driving
        }else{
            _delay_ms(time);//Drive forward for certain amount of time 
        }
        
        OCR1B = 0;//Set the speed of the Left tire to 0
        OCR1A = 0;//Set the speed of the Right tire to 0
    }
}



int main(){
    initDrive();
    initClckBuzz();
    
    while(1){
        wheels(-50, -50, 2000);
        wheels(80, 80, 2000);
        wheels(-40, 40, 2000);
        wheels(80, 80, 2000);
        wheels(-50, -50, 2000);
    }
}
