#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
void initPort();

void initPort(){
    DDRC |= (1 << PORTC7); // Make pin 13 be an output.
    DDRB |= (1 << PORTB1 | 1 << PORTB2 | 1 << PORTB5 | 1 << PORTB6);
    DDRD |= (1<<PORTD7);
    PORTB= ~(1 << PORTB1) | ~(1 << PORTB6);
}

void knipperLampje(){
    int i = 0;
    while(i < 5){
        PORTC |= (1 << PORTC7); // Turn the LED on.
        _delay_ms(500);
        PORTC &= ~(1 << PORTC7); // Turn the LED off.
        _delay_ms(500);
        i = i + 1;
    }
}

int main(){
    initPort();
    knipperLampje();//Toggle knipperLampje(5x times)
    
    while(1){
    }
}
