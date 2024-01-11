#include "kb.h"
void (*userF)();  //спасибо! https://alexgyver.ru/lessons/library-writing/
void SetAnalogPinMode(uint8_t port, uint8_t mode){ //УСТАНОВКА РЕЖИМА ПОРТА
	if(mode==0){ //УСТАНОВКА ПИНА В РЕЖИМ OUTPUT
		if(port>-1&&port<6){
			DDRC=DDRC|1<<port;
		}
	}
	if(mode==1){ //УСТАНОВКА ПИНА В РЕЖИМ INPUT
		if(port>-1&&port<6){
			DDRC = DDRC &~( 1<<port);
		}
	}
}
void SetDigitalPinMode(uint8_t port, bool mode){ //УСТАНОВКА РЕЖИМА ПОРТА 
	if(mode==0){ //УСТАНОВКА ПИНА В РЕЖИМ OUTPUT
		if(port>7&&port<14){
			DDRB=DDRB|1<<port-8;
		}
		if(port>-1&&port<8){
			DDRD=DDRD|1<<port;
		}
	}
	if(mode==1){ //УСТАНОВКА ПИНА В РЕЖИМ INPUT
		if(port>8&&port<14){
			DDRB = DDRB &~(1<<port-8);
		}
		if(port>-1&&port<8){
			DDRD = DDRD &~( 1<<port);
		}
	}
}
uint8_t PinRead(uint8_t port){ //ЧТЕНИЕ ДАННЫХ С ПОРТА
	if(port<8){
		uint8_t val=PIND >> port & 1<<port >> port; //читаем данные спомощью PIND(0-7 digital pins) умножаем номер пина на бит сдвинутый на номер пина и сдвигаем его на значение порта.
		return val;
	} else{
		uint8_t val=PINB >> port-8 & 1<<port-8 >> port-8; //читаем данные спомощью PINB(0-7 digital pins) умножаем номер пина на бит сдвинутый на номер пина и сдвигаем его на значение порта.
		return val;
	}
}
uint8_t AnalogRead(uint8_t port){ //повторение кода выше, только с Analog pins
	uint8_t val=DDRC>>port&1<<port>>port;
	return val;
}

void digitalPinWrite(uint8_t port,uint8_t e){
	
	if(port<8){
		switch(e){
		case 0: PORTD=~(1<<port); break; //УСТАНОВКА ПОРТА В LOW
		case 1: PORTD=1<<port; break;//УСТАНОВКА ПОРТА В HIGH
		}
	}else{
		switch(e){
		case 0: PORTB=~(1<<port); break; //УСТАНОВКА ПОРТА В LOW
		case 1: PORTB=1<<port; break;//УСТАНОВКА ПОРТА В HIGH
		}
	}
}

void digitalInterrupt(uint8_t port,void (*function)(),uint8_t mode){ //ФУНКЦИЯ ДЛЯ ПРЕРЫВАНИЙ 
	userF = *function; //Использование ссылки на функцию (ALEX GYVER СПАСИБО)!
	if(port==2){ //Если порт 2 то 
		DDRD &= ~(1<<2);  //Задаем D2 в качестве input
		PORTD=1<<port; //Подаем питание на D2
		if(mode==1){
			EICRA &=~(1<<ISC00); //0  на порте (0000)
		}
		if(mode==2){
			EICRA |=(1<<ISC00); //любое изменение (0001)
		}
		if(mode==3){
			EICRA |=(1<<ISC01); //нисходящее (0001)
		}
		if(mode==3){
			EICRA |=(1<<ISC01); EICRA |=(1<<ISC00); //восходящее (0010)
		}
		EIMSK |= (1<<INT0); //Внешнее прирывание INT0
	}
	if(port==3){ //Если порт 3 то 
		DDRD &= ~(1<<3);  //Задаем D2 в качестве input
		PORTD=1<<port; //Подаем питание на D3
		if(mode==1){
			EICRA &=~(1<<ISC10); //0  на порте (0000)
		}
		if(mode==2){
			EICRA |=(1<<ISC10); //любое изменение (0100)
		}
		if(mode==3){
			EICRA |=(1<<ISC11); //нисходящее (1000)
		}
		if(mode==3){
			EICRA |=(1<<ISC11); EICRA |=(1<<ISC10); //восходящее  (1100)
		}
		EIMSK |= (1<<INT1); //Внешнее прирывание INT1
	}
	sei(); //БЕЗ ЭТОГО РАБОТАТЬ НЕ БУДЕТ!
	////////ISC11 и ISC10 ДЛЯ INT 1 ///////// ISC00 и ISC10 ДЛЯ INT 0////////////////////
}
ISR(INT0_vect) //Обработчик прирываний INT0
{
  (*userF)();
}
ISR(INT1_vect) //Обработчик прирываний INT1
{
  (*userF)();
}
void SerialPort(){ //https://dzen.ru/a/Yh73sJWrDF5Zmwt1
	// Предделитель частоты 1 
	CLKPR=(1<<CLKPCE);
	CLKPR=0;
	
	UCSR0A = 0;						// Асинхронный режим, 1 стоповый бит, без проверки четности
	UCSR0B = (1<<TXEN0);					// Включаем передатчик
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);	// 8 бит данных
	UBRR0H=0;
	UBRR0L=103;
}
void send_byte(uint8_t byte){
	while(!( UCSR0A & (1 << UDRE0))) ;	// ожидаем, пока UDR0 не будет нулевым (не закончится передача)
	UDR0 = byte;				// записываем байт в порт
}
/* Функция для передачи строки без прерываний */
void send_buffer(char * buffer)
{
	for (uint8_t i = 0; i < sizeof(buffer)/sizeof(int); ++i)
		send_byte(buffer[i]);
}
