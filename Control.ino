#define btn_ON_OFF 4
#define btn_ON 5
#define LED_R 3
#define LED_V 2
#define btn_SCHIMB 4
#define btn_incr 2
#define btn_decr 3

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

int contor_schimbare = 0; //par pt temperatura, impar pt umiditate
int temperatura = 20;
int umiditate = 50;
const int tmax = 99;
const int tmin = -9;
const int umax = 99;
const int umin =  0;
int btn_set;


void setup()
{
  USART_Init(MYUBRR);

  EICRA=(1<<ISC11)|(0<<ISC10)|(1<<ISC01)|(0<<ISC00);
  EIMSK=(1<<INT1)|(1<<INT0);
  EIFR=(0<<INTF1)|(0<<INTF0);
  SREG|=(1<<SREG_I); 
  
  DDRD = 0b11100011;
  DDRB |= (1 << LED_R)|(1 << LED_V)|(0<<btn_SCHIMB);
  PORTB = (1<<LED_R);
}


void loop()
{
  if(buton_schimbare(btn_SCHIMB))
  {
    contor_schimbare++;
    btn_set = 0;
    set_temp_humid();
  }  
}

  
ISR(INT0_vect) // incrementare(2)
{
  SREG &=~ (1 << SREG_I);
  
  btn_set = 1;
  set_temp_humid();
  
  SREG |= (1 << SREG_I);
}


ISR(INT1_vect) // decrementare (3)
{
  SREG &=~ (1 << SREG_I);
  
  btn_set = -1;
  set_temp_humid();
  
  SREG |= (1 << SREG_I);
}


bool buton_schimbare(byte pin)
{
  if((PIND & (1<<pin))==0){
    while((PIND & (1<<pin))==0){
      
    }
    return true;
  }
  return false;
}


void aprindere_led(int led)
{
  PORTB = (1<<led);
}


void set_temp_humid()
{ 
  int set_temp = contor_schimbare % 2;
  char mesaj_t_h[3];
  
  if (!set_temp)
  {
    aprindere_led(LED_R);
    temperatura+=btn_set;
    temperatura=clamp(temperatura, tmin, tmax);
    if(temperatura <= 0)
    {
   	  mesaj_t_h[0] = 't';
      mesaj_t_h[1] = 45;
      mesaj_t_h[2] = 48+(temperatura*(-1));
    }
    else
   	{
   	  mesaj_t_h[0] = 't';
      mesaj_t_h[1] = temperatura/10 + '0';
      mesaj_t_h[2] = temperatura%10 + '0';
    }
    SendString(mesaj_t_h);
  }
  else
  { 
    aprindere_led(LED_V);
  	umiditate+=btn_set;
  	umiditate = clamp(umiditate, umin, umax);
    mesaj_t_h[0] = 'h';
    mesaj_t_h[1] = 48+umiditate/10;
    mesaj_t_h[2] = 48+umiditate%10;
    SendString(mesaj_t_h);
  }
}


int clamp(int x, int mi, int ma)
{
  if (x < mi){
    return mi;
  }
  if (x > ma){
    return ma;
  }
  return x;
}


// seriala
void USART_Init(unsigned int ubrr)
{
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  
  UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
  UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}


void USART_Transmit(unsigned char data){
	while(!(UCSR0A & (1<<UDRE0)));
  	UDR0=data;
}


void SendString(char *StringPtr){
  while(*StringPtr!=0x00){
  USART_Transmit(*StringPtr);
    StringPtr++;
  }
  USART_Transmit('\0');
}
  
