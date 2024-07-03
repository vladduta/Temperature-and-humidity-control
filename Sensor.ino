#define t_min -9
#define t_max 99

unsigned int sensorValue;
int umiditate_sensor;

// Adresa dispozitivului slave
unsigned char slave_addr = 0x90;
int cont = 1;


void setup()
{
  initializareADC();
  i2c_init();
}


void loop()
{
  i2c_wait_request();
  
  switch (cont)
  {
    case(1):
  		sensorValue = citesteADC(0);
  		umiditate_sensor = sensorValue/10.33;
  
  		i2c_send(umiditate_sensor);
    	break;
    case(2):
    	sensorValue = citesteADC(1);
  		int temp = (sensorValue-20.0)*(165.0/339.0) - 40.0;
  		//int temp = (sensorValue* 0.0048828125-0.5)*100;
  		if (temp > t_max)	temp = t_max;
  		if (temp < t_min)   temp = t_min;
  
  		i2c_send(temp+9);
    	cont = 0;
    	break;
  }
  
  cont++;
}


void initializareADC(){
  //dezactivare buffer intrare digitala
 DIDR0 = (0<<ADC5D) | (0<<ADC4D) | (0<<ADC3D) | (0<<ADC2D) | (0<<ADC1D) | (0<<ADC0D);
  //configurare registru de control si stare
 ADCSRA = (1<<ADEN) | (0<<ADSC) | (1<<ADATE) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);
}


unsigned int citesteADC(int pin){
  //configurare multiplexor ADC
  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR);
  
  if(pin==1){
    ADMUX |= (1 << MUX0);
  }

  ADCSRA |= (1<<ADSC); //se porneste conversie
  
  while((ADCSRA & (1<<ADIF))==0){} //asteptam finalizarea conversiei
  ADCSRA |= (1<<ADIF);//intrerupere ADC 
  
  return ADCW; 
}


void i2c_init() {
  TWAR = slave_addr;
}


void i2c_wait_request() {
  // Asteptam informatia corecta
  while((TWSR & 0xF8)!= 0xA8) {
     // Primeste informatia si se dezactiveaza intreruperea
	TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT); 
    
    // Asteapta pana cand flag-ul este setat
	while (!(TWCR & (1<<TWINT)));   
  }
}


void i2c_send(int value) {
  // Se introduc datele ce urmeaza a fi trimise
  TWDR = value;

  // Pornire trimitere date
  TWCR = (1<<TWEN)|(1<<TWINT);
  
  // Asteptare confirmare
  while((TWSR & 0xF8) != 0xC0);
}

