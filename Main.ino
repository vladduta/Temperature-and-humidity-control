#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#define LED_CLEAR_PD 0b00000011
#define LED_CLEAR_PB 0b11000000
#define LED_CLEAR_PC 0b11110011
#define LED_OFF_TEMP 0b11111101
#define LED_ON_TEMP 0b00000010
#define VENTILATOR_ON 0b00000001
#define VENTILATOR_OFF 0b11111110
#define TEMP 0
#define HUMID 1
#define NONE -1

byte nr_pd[11]; 
byte nr_pb[11];

byte nrz_pb[11]; //nr zecilor
byte nrz_pc[11];

int contor_timer = 0;
int milisecunde = 0;
int timp_flicker = 200;
int timp_setare = 1;
int secunde_setare = 0;
int secunde = 0;

int temp_setata = 0;
int humid_setata = 0;

int temp_curenta = 20;
int humid_curenta= 50;

int mod_setare = NONE;
int mod_afisare = TEMP;

bool stins = false;
bool is_receiving = false;

unsigned char slave_addr = 0x90;


void setup()
{
  nr_pd[0] = 0b11111100;
  nr_pb[0] = 0b00000000;
  nr_pd[1] = 0b00011000;
  nr_pb[1] = 0b00000000;
  nr_pd[2] = 0b01101100;
  nr_pb[2] = 0b00000001;
  nr_pd[3] = 0b00111100;
  nr_pb[3] = 0b00000001;
  nr_pd[4] = 0b10011000;
  nr_pb[4] = 0b00000001;
  nr_pd[5] = 0b10110100;
  nr_pb[5] = 0b00000001;
  nr_pd[6] = 0b11110100;
  nr_pb[6] = 0b00000001;
  nr_pd[7] = 0b00011100;
  nr_pb[7] = 0b00000000;
  nr_pd[8] = 0b11111100;
  nr_pb[8] = 0b00000001;
  nr_pd[9] = 0b10111100;
  nr_pb[9] = 0b00000001;
  nr_pd[10]= 0b00000000;
  nr_pb[10]= 0b00000001;
        
  nrz_pb[0] = 0b00111110;
  nrz_pc[0] = 0b00001000;
  nrz_pb[1] = 0b00001100;
  nrz_pc[1] = 0b00000000;
  nrz_pb[2] = 0b00110110;
  nrz_pc[2] = 0b00000100;
  nrz_pb[3] = 0b00011110;
  nrz_pc[3] = 0b00000100;
  nrz_pb[4] = 0b00001100;
  nrz_pc[4] = 0b00001100;
  nrz_pb[5] = 0b00011010;
  nrz_pc[5] = 0b00001100;
  nrz_pb[6] = 0b00111010;
  nrz_pc[6] = 0b00001100;
  nrz_pb[7] = 0b00001110;
  nrz_pc[7] = 0b00000000;
  nrz_pb[8] = 0b00111110;
  nrz_pc[8] = 0b00001100;
  nrz_pb[9] = 0b00011110;
  nrz_pc[9] = 0b00001100;
  nrz_pb[10]= 0b00000000;
  nrz_pc[10]= 0b00000100;
  
  DDRD = 0b11111111;
  DDRB = 0b00111111;
  DDRC = 0b00111101;
  
  PORTC = 0b00110010;
        
  USART_Init(MYUBRR); //initializare TIMER
  
  TCCR0A = (1<<WGM01)|(0<<WGM00); //seteaza mod de functionare CTC
  OCR0A = 0xF9; //valoarea de TOP=250
  TIMSK0 |= (1<<OCIE0A); //activeaza intreruperea de comparare
  TCCR0B = (1<<CS02 )|(0<<CS01)|(0<<CS00 ); //setare prescalar=256
  SREG |= (1<<SREG_I);
  
  // Init I2C
  i2c_init();
}


void loop()
{  
  // Pornire comunicatie
  i2c_start();
  
  i2c_send_request();
  
  // Citire date primite
  humid_curenta = i2c_receive();
  
  i2c_stop();
  
  i2c_start();
  
  i2c_send_request();
  
  // Citire date primite
  temp_curenta = i2c_receive() - 9;
  
   // Oprire comunicatie
  i2c_stop();
  
  char mesaj[3];
  int messageIndex = 0;
  
  char x = USART_Recieve();
  if (x != ' ')//verific daca am inceput sa primim date 
    while(x != '\0')
  	{
    	mesaj[messageIndex] = x; 
    	messageIndex++;
      	if (messageIndex >=3) 
        {
          break;
        }
    	x = USART_Recieve(); 
        while (x == ' ') //asteptam pana se trimite urmatorul caracter
          x = USART_Recieve();
  	}
  
  
  if (messageIndex != 0) //daca am primit mesaj
  {
	mesaj[messageIndex] = '\0'; 
    procesare_mesaj(mesaj); 
  }
  
  
  switch(mod_setare)
  {
  	case(TEMP):
      	display_nr(temp_setata);
    	return;
    case(HUMID):
        display_nr(humid_setata);
    	return;
  }
   
  
  if (temp_setata < temp_curenta || humid_setata < humid_curenta)
    PORTC |= VENTILATOR_ON;
  else
    PORTC &= VENTILATOR_OFF;
    
  
  switch(mod_afisare)
  {
    case(TEMP):
      	display_nr(temp_curenta);
    	break;
    case(HUMID):
    	display_nr(humid_curenta);
    	break;
  }
}


void procesare_mesaj(char *msg)
{
  char mode = msg[0];
  char t1 = msg[1];
  char t2 = msg[2];
  int value;
  
  if (msg[1] != '-')
  	value = (msg[1]-'0')*10 + msg[2] - '0';
  else
    value = - (msg[2] - '0');
  
  if (mode == 't')
  {
    mod_setare = TEMP;
    temp_setata = value;
    return;
  }
  
  if (mode == 'h')
  {
    mod_setare = HUMID;
    humid_setata = value;
  }
  
  secunde_setare = 0;
}


unsigned char USART_Recieve(void)
{
  if (!(UCSR0A & (1 << RXC0)))//daca nu primim mesaj 
  {
    return ' ';
  }
  
  return UDR0;
}


void USART_Init(unsigned int ubrr)
{
  UBRR0H = (unsigned char)(ubrr>>8); //setare baud rate 
  UBRR0L = (unsigned char)ubrr; 
  
  UCSR0B |= (1<<RXEN0); // activare receptor
  UCSR0C = (1<<USBS0)|(3<<UCSZ00); // setare 8 biti de date si 2 biti de stop
}

void display_nr(int n)
{
  int p0;
  int p1;
  int nr = n;
  if (nr < 0)
    nr *= -1;
  
  p0 = nr % 10;
  p1 = nr / 10;
  
  PORTD &= LED_CLEAR_PD; //stingem leduri de la 7 segmente
  PORTB &= LED_CLEAR_PB;
  PORTC &= LED_CLEAR_PC;
  
  if (stins) return;
  
  PORTD |= nr_pd[p0];
  PORTB |= nr_pb[p0];
  
  if (n >= 0)
  {
    PORTB |= nrz_pb[p1];
  	PORTC |= nrz_pc[p1];
  }
  else
  {
    PORTB |= nrz_pb[10]; //aprindem "-"
  	PORTC |= nrz_pc[10];
  }    
}


ISR(TIMER0_COMPA_vect)
{
  SREG &= ~(1<<SREG_I); 
  ++contor_timer;

  if(contor_timer >= 250)
  {
	++secunde;
    
    if (mod_setare > NONE)
    {
      ++secunde_setare;
      if (secunde_setare >= timp_setare)
      {
        secunde_setare = 0;
        mod_afisare = mod_setare;
        secunde = 0;
        mod_setare = NONE;
        stins = false;
        if (mod_afisare == TEMP)
        {
          PORTC |= LED_ON_TEMP;
        }
      	else
          PORTC &= LED_OFF_TEMP;
      }
    }
    
	contor_timer = 0;
    
    if (mod_setare == NONE && secunde >= 2)
  	{
    	secunde = 0;
      	mod_afisare++;
      	if (mod_afisare > 1)
        {
          mod_afisare = 0;
          PORTC |= LED_ON_TEMP;
        }
      	else
          PORTC &= LED_OFF_TEMP;
  	}
  }
  
  
  if (mod_setare > NONE)
  {
    milisecunde += 4;
    if (milisecunde > timp_flicker)
    {
      milisecunde = 0;
      stins = !stins;
    }
  }
  
  SREG |= (1 << SREG_I);
}


void i2c_init() {
  // Se seteaza registrul de stare pe 0
  TWSR = 0x00; 
  
  // setare SCL la 400 Khz 
  TWBR = 0x0c; 
}


void i2c_start() {
  //configurare i2c
  TWCR= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); 
  
  // Asteptam pana cand conditia de start a fost transmisa
  while(!(TWCR & (1<<TWINT))); 
  
  // Asteptam confirmare
  while((TWSR & 0xF8)!= 0x08); 
}


void i2c_send_request() { 
  // Vom trimite adresa și instrucțiunea de citire
  TWDR = slave_addr + 1; 
            
  // Dezactivam intrerupere, activam TWI
  TWCR=(1<<TWINT)|(1<<TWEN); 
  
  // Asteptare pana se transmite request-ul
  while (!(TWCR & (1<<TWINT))); 
  
  // Asteptam confirmare
  while((TWSR & 0xF8)!= 0x40); 
}


int i2c_receive() { 
  // Dezactivare intrerupere,activare TWI
  TWCR=(1<<TWINT)|(1<<TWEN); 
  
  // Asteptare transmitere date
  while (!(TWCR & (1<<TWINT))); 
  
  // Asteptam confirmare
  while((TWSR & 0xF8) != 0x58); 

  // Returnam datele primite
  return (int)TWDR;
}


void i2c_stop() {
  //Stergere intrerupere, conditionare si activare TWI
  TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); 
  
  // Asteptam pana conditia de stop este transmisa
  while(!(TWCR & (1<<TWSTO)));  
}


