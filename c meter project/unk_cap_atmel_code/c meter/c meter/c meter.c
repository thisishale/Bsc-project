/*
 * c_meter.c
 *
 * Created: 12/30/2015 03:01:26 ب.ظ
 *  Author: hale
 */ 
#define F_CPU 11059200UL

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

float b=0; 
float c=0;
unsigned long int x; 
unsigned int i;
unsigned int n;
unsigned int m;
unsigned char str[10];
unsigned char str2[10];
void lcd_float(unsigned int m);
  int main(void)
  {	 
	    lcd_init(LCD_DISP_ON);	        	//initialize lcd, display on, cursor on 
		_delay_ms(500);
		lcd_puts("started");
	    _delay_ms(500);
		DDRD=0x60;							//int0 & int1 DDR
		DDRB |=0x01;
		PORTD &= 0b00011111;
		TIMSK=((1<<TOIE0));		//timer0 overflow enable
		TCNT0=0x00;							//starts from 0
		TCNT1=0x00;                        
		GICR=((1<<INT0)|(1<<INT1));
		MCUCR=((1<<ISC01)|(0<<ISC00)|(1<<ISC11)|(0<<ISC10));
		PORTD |= 0b00100000;                //discharges the cap UF
		PORTB |= 0b00000001;                //discharges the cap NF
		_delay_ms(500);
		lcd_clrscr();
		sei();								//enables global interrupts					
       while(1)
      { 
	

		  /********************************UF PART***************************/
			if (!(PIND & 0b00000001))             //PIND.0 push button for uf
			{  
			_delay_ms(50);
				  if ((PIND & 0b00000001))
					{
                    PORTD &= 0b11011111;          //stops the discharging uf
					TCCR0 |= ((1 << CS01));       // Set up timer at 8 prescale,start timer
					PORTD |= 0b01000000;          //PORTD.6 is connected to a transistor	
					}			
			}
			/****************************************************************/
			
			/********************************NF PART*************************/
				  if (!(PIND & 0b00010000))       //PIND.4 FOR nf
			{  
			_delay_ms(50);
				  if ((PIND & 0b00010000))
					{
                    PORTB &= 0b11111110;         //stops the discharge nf
					TCCR1B= ((1 << CS21)|(1 << CS20));      // Set up timer1 at 64 prescale,start timer
					//bit_set(PORTD,BIT(6));
					PORTD |= 0b10000000;        //PIND.7 is connected to a transistor	
					}			
			}

			/****************************************************************/	
			
					
			if(!(PIND & 0b00000010))        //clear everything
			{

	              b=0;
				  x=0;			
				  n=0;
				  m=0;
				  c=0;
                  TCNT0=0x00;           //reset tcnt0
				  TCNT1=0x00;            
				  TCCR0 &= (0xf8);      //stops timer0
				  TCCR1B &= (0xf8);      //stops timer1
				  lcd_clrscr();
				  
				  PORTD &= 0b10111111;  //stops charging uf cap
				  PORTD &= 0b01111111;  //stops charging nf cap
				  PORTD |= 0b00100000;  //discharge both caps
				  PORTB |= 0b00000001;  //discharge both caps
				  lcd_gotoxy(0,0);
				  
			      				  
			}	   
	    }
		
		

  }
  
  
  		 
		void lcd_float(unsigned int m)
		{


			 lcd_gotoxy(0,1);
			 sprintf(str,"%d",m);
			 i=strlen(str)-1;
			 str[i+1]=str[i];  
			 str[i]='.';
			lcd_puts(str);
			lcd_puts("uf");
		

			}			
  
  
  
  		ISR(TIMER0_OVF_vect)
	     {

	      	b=b+1;
	       
	  
         }
		 
		 ISR(INT0_vect)
         {   
			 TCCR0 &= (0xf8);    //stop timer0
			 b/=7;
			 b=b/10;
			 n=b*100;
            
				if((n<10)||(n>5000))
				{
				  lcd_puts("out of uf range ");
			
				}
				else
				{	
                 lcd_float(n);
				}				
		
		 }
		 
		 
		
		 ISR(INT1_vect)
		 {
			TCCR1B &= (0xf8);    //stop timer1
			
            x=TCNT1;
			x=x/116.09;
				if((x<1)||(x>600))
				{
				lcd_puts("out of nf range ");
			
				}
				else
				{
                sprintf(str2,"%d",x);
				lcd_puts(str2);	
				lcd_puts("nf");
				}				
			
			
			
		 }