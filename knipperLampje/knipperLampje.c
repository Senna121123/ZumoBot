#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
void initKnipperLampje();
void knipperLampje(uint8_t);

void initKnipperLampje(){
    DDRB |= (1<<PORTB0);//RED LED Port output
    DDRC |= (1<<PORTC7);//Yellow LED Port output
    DDRD |= (1<<PORTD5);//Green LED Port output
}

void knipperLampje(uint8_t iteraties){
    int i;
    for (i = 0; i < iteraties*2; i++){
        PORTC ^= (1 << PORTC7); // Toggle Yellow LED
        _delay_ms(200);
        PORTD ^= (1 << PORTD5); // Toggle Green LED
        _delay_ms(200);
        PORTB ^= (1 << PORTB0); // Toggle Red LED
        _delay_ms(200);
    }
}

int main(){
    initKnipperLampje();
    while(1){
        knipperLampje(4);//Toggle knipperLampje(4x times)
    }
}
