#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
void initClckBuzz();
void initPort();

void initClckBuzz(){
    TCCR4B |= (1<<CS43);
    TCCR4C |= (1<<COM4D1) | (1<<COM4D0);
    OCR4C = 166;
    OCR4D = 83;
}

void initPort(){
    DDRC |= (1 << PORTC7); // Make pin 13 be an output.
    DDRB |= (1 << PORTB1 | 1 << PORTB2 | 1 << PORTB5 | 1 << PORTB6);
    DDRD |= (1<<PORTD7);
    PORTB= ~(1 << PORTB1) | ~(1 << PORTB6);
}

int main(){
    initPort();
    initClckBuzz();
    
    while(1){
        TCCR4C |= (1<<PWM4D);//Zet de buzzer aan
        _delay_ms(500);
        TCCR4C &= ~(1<<PWM4D);//Zet de buzzer uit
        _delay_ms(500);
    }
}
