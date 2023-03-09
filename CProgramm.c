#define F_CPU 8000000UL
#define UpdateFrame 5

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>

#define PatternCount 3
#define PatternLength 8

//Formatting
/*
    int 32 bit
	
	27 used: 9 bit segments
	
	first 9 bits lowest row, next second, next third
	
	 xxxxx 000000000 000000000 000000000
*/
const uint32_t PROGMEM Muster [3][8] =
{
	{
		(uint32_t)0b00000101010101010101010101010101,
		(uint32_t)0b00000010101010101010101010101010,
		//stops the pattern
		(uint32_t)0b10000000000000000000000000000000,
		(uint32_t)0b10000000000000000000000000000000,
		(uint32_t)0,
		(uint32_t)0,
		(uint32_t)0,
		(uint32_t)0
	},
	{
		(uint32_t)0b00000000000111000000111000000111,
		(uint32_t)0b00000000111000000111000000111000,
		(uint32_t)0b00000111000000111000000111000000,
		(uint32_t)0b10000000000000000000000000000000,
		(uint32_t)0b10000000000000000000000000000000,
		(uint32_t)0,
		(uint32_t)0,
		(uint32_t)0
	},
	{
		(uint32_t)0b00000000000001000000001000000001,
		(uint32_t)0b00000000001010000001010000001010,
		(uint32_t)0b00000001010100001010100001010100,
		(uint32_t)0b00000010100000010100000010100000,
		(uint32_t)0b00000100000000100000000100000000,
		(uint32_t)0b00000010100000010100000010100000,
		(uint32_t)0b00000001010100001010100001010100,
		(uint32_t)0b00000000001010000001010000001010
	}
};

uint32_t led = 0;
int muster = 0;
char s = '0';

void Update(float delay);
void Update (float delay) {
		
	float current = 0;
	
	while (current < delay) 
	{		
		PORTB = 0;
		PORTD = (1<<PD3);
		
		for (uint32_t c = 0; c < 3; c++)
		{
			PORTB = 0;
			
			for (uint32_t i = 0; i < 9; i++)
			{
				switch (i)
				{
					case 0:
					if (led & (uint32_t)((uint32_t)0x0001<<(i + (uint32_t)9 * c)))
						PORTB |= (1<<PB7);
					break;
					case 1:
					if (led & (uint32_t)((uint32_t)0x0001<<(i + (uint32_t)9 * c)))
						PORTB |= (1<<PB6);
					break;
					case 2:
					if (led & (uint32_t)((uint32_t)0x0001<<(i + (uint32_t)9 * c)))
						PORTB |= (1<<PB5);
					break;
					case 3:
					if (led & (uint32_t)((uint32_t)0x0001<<(i + (uint32_t)9 * c)))
						PORTB |= (1<<PB4);
					break;
					case 4:
					if (led & (uint32_t)((uint32_t)0x0001<<(i + (uint32_t)9 * c)))
						PORTB |= (1<<PB2);
					break;
					case 5:
					if (led & (uint32_t)((uint32_t)0x0001<<(i + (uint32_t)9 * c)))
						PORTB |= (1<<PB3);
					break;
					case 6:
					if (led & (uint32_t)((uint32_t)0x0001<<(i + (uint32_t)9 * c)))
						PORTB |= (1<<PB1);
					break;
					case 7:
					if (led & (uint32_t)((uint32_t)0x0001<<(i + (uint32_t)9 * c)))
						PORTB |= (1<<PB0);
					break;
					case 8:						
					if (led & (uint32_t)((uint32_t)0x0001<<(i + (uint32_t)9 * c)))
						PORTD |= (1<<PD6);
					break;
				}
			}
			
			if ((PIND & (1 << PD2)) != 0 && s != '1')
			{
				s = '1';
				
				muster++;
				
				if (muster >= PatternCount)
					muster = 0;
				
				_delay_ms(10);
			}
			
			if (s == '1' && (PIND & (1 << PD2)) == 0)
				s = '0';
			
			_delay_ms((float)UpdateFrame / 3.0f);
			
			if (c == 0)
				PORTD = (1<<PD5);
			else if (c == 1)
				PORTD = (1<<PD4);
		}
		
		current += UpdateFrame;
	}
	
}

int main(void)
{
	DDRB = 0xFF;
	DDRD = ~((1<<PD2) | (1<<PD1) | (1<<PD0));

	muster = 0;
	s = '0';

    while (1) 
    {
		for (int z = 0; z < PatternLength; z++)
		{
			led = pgm_read_dword(&Muster[muster][z]);
				
			if (led & (uint32_t)0b10000000000000000000000000000000)
				break;
				
			Update(300.0f);
		}
		
		led = 0;
    }
}

