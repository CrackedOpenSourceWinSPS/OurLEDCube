#define F_CPU 8000000UL
#define UpdateFrame 5

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
//Library für 32 bit integers
#include <stdint.h>

#define PatternCount 3
#define PatternLength 8

//Formatting
/*
    int 32 bit
	
	27 used: 9 bit segments
	
	first 9 bits lowest row, next second, next third
	
	 xxxxx 000000000 000000000 000000000

    Muster des LED cubes wird in einen einzigen uint32 gespeichert, die ersten 27 bit beschreiben
    den Zustand der LEDs, der letzte bit lässt das Muster abbrechen. Der erste bit entspricht also 
    der LED in der niedersten ecke, während der 27. der LED in der obersten Ecke entspricht.
*/
//Muster werden in diesem 2D array gespeichert
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

//Aktuellen LED zustand
uint32_t led = 0;
//Aktuelles muster im array
int muster = 0;
//Wird zum reseten des Knopfes benutzt
char s = '0';

//Da die LEDs immer geupdated werden müssen, wird diese methode verwendet um eine bestimmte zeit lang
die LEDs mit dem gleichen muster zu updaten. Delay beschreibt wie langr geupdated wird.
void Update(float delay);
void Update (float delay) {
	//Aktuelle zeit
	float current = 0;
	
        //Solange updaten bis die aktuelle zeit größer als totale zeit ist
	while (current < delay) 
	{		
                //LED port reseten
		PORTB = 0;
                //Auf erste reihe schreiben
		PORTD = (1<<PD3);

		//3 reihen
		for (uint32_t c = 0; c < 3; c++)
		{
			PORTB = 0;
			
                        //Zustand des musters in die LEDs schreiben
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
			
                        //Überprüfen ob schalter gedrückt
			if ((PIND & (1 << PD2)) != 0 && s != '1')
			{
                                //Verhinden das schalter mehrmals muster in einem klick ändert
				s = '1';
				
				muster++;
				
                                //Falls muster größer als array zu muster 0 zurückwechseln
				if (muster >= PatternCount)
					muster = 0;
				
				_delay_ms(10);
			}
			
			if (s == '1' && (PIND & (1 << PD2)) == 0)
				s = '0';
			//Kruz warten dann nächste reihe anzeigen
			_delay_ms((float)UpdateFrame / 3.0f);
			
			if (c == 0)
				PORTD = (1<<PD5);
			else if (c == 1)
				PORTD = (1<<PD4);
		}
		
                //Aktuellen zeitwert überschreiben
		current += UpdateFrame;
	}
	
}

int main(void)
{
        //Aus und eingänge setzten
	DDRB = 0xFF;
	DDRD = ~((1<<PD2) | (1<<PD1) | (1<<PD0));

	muster = 0;
	s = '0';

    while (1) 
    {
                //Muster schreiben
		for (int z = 0; z < PatternLength; z++)
		{
                        //Zustand aus flash lesen
			led = pgm_read_dword(&Muster[muster][z]);
				
                        Falls letzte bit aktiv ist das muster zu ende
			if (led & (uint32_t)0b10000000000000000000000000000000)
				break;
				
                        //Muster frame 0.3 sekunden lang anzeigen
			Update(300.0f);
		}
		
                //Led zustand reseten
		led = 0;
    }
}

