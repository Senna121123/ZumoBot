#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <float.h>

volatile char newChar, oldChar;
char direction;
double userSpeed;


ISR(USART1_RX_vect) {
     newChar = UDR1;
}

void initClckBuzz(){                            //Init function for Buzzer
    DDRD |= (1<<PORTD7);                        //Make PortD7 output for PWM
    TCCR4B |= (1<<CS43);                        //Prescaler 128x
    TCCR4C |= (1<<COM4D1) | (1<<COM4D0);        //Comparator mode Set on comparematch
    OCR4C = 166;                                //Set top of fastPWM
    OCR4D = 83;                                 //Comparator value
}

void buzzer(uint8_t iterations){        //function for Buzzer
    int i;                              //Integer for going through the loop
    for (i = 0; i < iterations; i++){   //Loop for times the buzzer is calls
        TCCR4C |= (1<<PWM4D);           //Zet de buzzer aan op fastPWM
        _delay_ms(500);                 //Delay of 0.5 seconds
        TCCR4C &= ~(1<<PWM4D);          //Zet de buzzer uit fastPWM
        _delay_ms(500);                 //Delay of 0.5 seconds
    }
}

void initUsart(){
    UCSR1A = 0;                                             //clear the USART Control and Status Register 
    UCSR1B |= (1 << TXEN1) | (1 << RXEN1) |(1 << RXCIE1);   //enable the USART Transmitter and Receiver and enable interrupt on the RXCn Flag 
    UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);                //set the USART Character Size to 8-bit
    UBRR1H = 0;                                             //USART1 Baud Rate Register High Byte
    UBRR1L = 103;                                           //USART1 Baud Rate Register Low Byte for 9600Hz
}

uint8_t readChar(){
    while(!(UCSR1A & (1 << RXC1))); //wait for a moment where no key is pressed
        return UDR1;                //returns the received char
}

void writeChar(char x) {
    while (!(UCSR1A & (1<<UDRE1))); // Wait for empty transmit buffer
    UDR1 = x;                       // Put data into buffer, sends the data
}

void writeString(char st[]) {
    uint8_t i = 0;                  //Variable to loop through an array
    for(i = 0 ; st[i] != 0 ; i++) { //Loop for each char in the array
        writeChar(st[i]);           //Write the char
    }
}

void writeInt(uint16_t i) {
    char buffer[16];        //char Array of 16 chars
    itoa(i,buffer,10);      //Convert the integers i into chars and storge in the buffer
    writeString(buffer);    //Write all chars as a string
}

void writeFloat(float fl){
    float t = fl * 10;      //variable used for decimal numbers
	float i = (int)fl * 10; //variable for whole numbers without decimal numbers
	int b = ((int)t-(int)i);//Get the decimal numbers
	writeInt((int)fl);      //Print the whole numbers
	writeChar(',');         //Add a comma
	writeInt(abs(b));       //Print the decimal digits
}

void initDrive(){
                                                //Timer 1 Configuration
                                                //Prescaler: clock I/O / 1
                                                //Output enabled
                                                //Phase-correct PWM
                                                //top of 400
                                                //PWM frequency calculation
                                                //16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20KHz

    TCCR1A |= (1<<COM1A1 | 1 <<COM1B1);         //Set OCnA/OCnB/OCnC on compare match when upcounting, clear on compare match when downcounting
    TCCR1B |= (1<<CS12);                        //Prescaler 256
    TCCR1B |= (1<<WGM13);                       //PWM Phase- and Frequency Correct
    ICR1 = 400;                                 //TOP of PWM
    DDRB |= (1 << PORTB1 | 1 << PORTB2);        //Set motordirections as output Right motor direction  |  PORTB2: Left motor direction
    DDRB |= (1 << PORTB5 | 1 << PORTB6);        //Set motors PWM as output PORTB5: Right motor PWM  |  PORTB6: Left motor PWM
    OCR1A = 0;                                  //OutputCompareRegister --> Right tire still
    OCR1B = 0;                                  //OutputCompareRegister --> Left tire still
    PORTB |= ~(1 << PORTB1) | ~(1 << PORTB6);   //Default direction drive forward
}

void wheels(double speedLeft, double speedRight, int time){
    int checkDriveBack = 0;                         //Checks if robot drives backwards
    int loopDriveBack = 0;                          //Variable used for buzzer on driving backwards

    if(speedLeft < 0 && speedRight < 0){            //Zumo drives backwards
        checkDriveBack = 1;                         //Variable used to tell if zumo drives backwards
    }

    if(speedLeft > 0){                              //forward left tire
        PORTB &= ~(1<<PORTB2);                      //set direction of left tire forward
    }else{                                          //backward left tire
        PORTB |= (1<<PORTB2);                       //set direction of left tire backward
        speedLeft = speedLeft * -1;                 //set speed of left tire negative
    }
    if(speedRight > 0){                             //forward right tire
        PORTB &= ~(1<<PORTB1);                      //set direction of right tire forward
    }else{                                          //backward right tire
        PORTB |= (1<<PORTB1);                       //set direction of right tire backward
        speedRight = speedRight * -1;               //set speed of right tire negative
    }

                                                    //setting speed
    OCR1B = speedLeft * 4;                          //Set the speed of the Left tire
    OCR1A = speedRight * 4;                         //Set the speed of the Right tire
    if (time != 0){                                 //If zumo did not get the operation of driving without a time limit
        if (checkDriveBack == 1 && time >= 1000){   //Check if robot drives backwards and the time 
            loopDriveBack = time / 1000;            //Calculate the number of seconds
            buzzer(loopDriveBack);                  //Buzzer function for driving backwards
            _delay_ms(time - (1000*loopDriveBack)); //Wait left over seconds before driving
        }else{                                      //If zumo gets a time delay with a specific time
            _delay_ms(time);                        //Drive forward for certain amount of time 
        }
        
        OCR1B = 0;                                  //Set the speed of the Left tire to 0
        OCR1A = 0;                                  //Set the speed of the Right tire to 0
    }
}
    
void driveDirection(char z, int time){
    switch(z){                                                  //z is used for the direction the zumo is going
     		case 'w':                                           //if direction pressed is W
                wheels(userSpeed, userSpeed, time);             //Let zumo drive forward
                direction = 'w';                                //change current direction to W
                break;                                          //end of switch case
					
			case 'd':                                           //if direction pressed is D
                wheels(userSpeed, -1 * userSpeed, time);        //Let zumo turn right on its position
                direction = 'd';                                //change current direction to D
                break;                                          //end of switch case
					
			case 'a':                                           //if direction pressed is A
                wheels(-1 * userSpeed, userSpeed, time);        //Let zumo turn left on its position
                direction = 'a';                                //change current direction to A
                break;                                          //end of switch case
					
			case 's':                                           //if direction pressed is S
				wheels(-1 * userSpeed, -1 * userSpeed, time);   //Let zumo drive backward
                direction = 's';                                //change current direction to S
                break;                                          //end of switch case

            case 'q':                                           //if direction pressed is Q
                wheels(userSpeed*0.5 , userSpeed, time);        //Let zumo take a wide turn left forward
                direction = 'q';                                //change current direction to Q
                break;                                          //end of switch case

            case 'e':                                           //if direction pressed is E
                wheels(userSpeed, userSpeed * 0.5, time);       //Let zumo take a wide turn right forward
                direction = 'e';                                //change current direction to E
                break;                                          //end of switch case

            case 'c':                                           //if direction pressed is C
                wheels(-1 * userSpeed, -1 * userSpeed/2, time); //Let zumo take a wide turn right backward
                direction = 'c';                                //change current direction to C
                break;                                          //end of switch case

            case 'z':                                           //if direction pressed is Z
                wheels(-1 * userSpeed/2 , -1 * userSpeed, time);//Let zumo take a wide turn left backward
                direction = 'z';                                //change current direction to Z
                break;                                          //end of switch case
    }
}

void driveControl(int time){
    switch(newChar){                        //newChar is used for the direction the zumo is going
        case '1':                           //If speed 1 is pressed
            userSpeed = 10;                 //set current speed to 10%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case '2':                           //If speed 2 is pressed
            userSpeed = 20;                 //set current speed to 20%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case '3':                           //If speed 3 is pressed
            userSpeed = 30;                 //set current speed to 30%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case '4':                           //If speed 4 is pressed
            userSpeed = 40;                 //set current speed to 40%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case '5':                           //If speed 5 is pressed
            userSpeed = 50;                 //set current speed to 50%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case '6':                           //If speed 6 is pressed
            userSpeed = 60;                 //set current speed to 60%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case '7':                           //If speed 7 is pressed
            userSpeed = 70;                 //set current speed to 70%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case '8':                           //If speed 8 is pressed
            userSpeed = 80;                 //set current speed to 80%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case '9':                           //If speed 9 is pressed
            userSpeed = 90;                 //set current speed to 90%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case '0':                           //If speed 0 is pressed
            userSpeed = 0;                  //set current speed to 0%
            driveDirection(direction,time); //Call function to move the zumo
            break;                          //end of switch case
        case 'x':                           //Toeter
            buzzer(1);                      //Activate the buzzer
            break;                          //end of switch case
        default:                            //If none of the above cases is called
            driveDirection(newChar,time);   //Call function to move the zumo
    }
}

int main(){
    USBCON = 0;                 //USB interupt clear
    initClckBuzz();             //Initialise the buzzer
    initDrive();                //Initialise motors
    initUsart();                //initialise usart
    sei();                      //enable interupt

    while(1){                   //Besturing
        if (newChar != oldChar){//checks if a new key is pressed
            oldChar = newChar;  //change the old char to the current new char
            writeChar(newChar); //Prints out the new char
            driveControl(0);    //control the motors speed and direction
        }
    }
}
