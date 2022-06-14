#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
void initClckBuzz();
void buzzer();

void initClckBuzz(){//Init function for Buzzer
    TCCR4B |= (1<<CS43);//Prescaler 128x
    TCCR4C |= (1<<COM4D1) | (1<<COM4D0);//Comparator mode Set on comparematch
    OCR4C = 166;//Set top of fastPWM
    OCR4D = 83;//Comparator value
}

void buzzer(uint8_t iteraties){//function for Buzzer
    int i;
    for (i = 0; i <= iteraties; i++){
        TCCR4C |= (1<<PWM4D);//Zet de buzzer aan op fastPWM
        _delay_ms(500);
        TCCR4C &= ~(1<<PWM4D);//Zet de buzzer uit fastPWM
        _delay_ms(500);
    }
}

int main(){
    initPort();
    buzzer(1);
    while(1){
    }
}
