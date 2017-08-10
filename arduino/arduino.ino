#includ <SPI.h>
#define ss 10   // define the CS pin mapping

byte rx_data_bytes;

void read_data_frame(void);

void AD_init()
{
  digitalWrite(ss,LOW);   
  SPI.transfer(0x01);
  SPI.transfer(0xA0);
  SPI.transfer(0);  // Continous mode and enable filter data rate control
  digitalWrite(ss,HIGH);
  
  digitalWrite(ss,LOW);   
  SPI.transfer(0x28);
  SPI.transfer(0x00);
  //SPI.transfer(0x0b);//500
 //SPI.transfer(0x14);  //5
 // SPI.transfer(0x03);//50khz
 SPI.transfer(0x09);  //2500
  //SPI.transfer(0x00);  //25000
  digitalWrite(ss,HIGH);

  digitalWrite(ss,LOW);   
  SPI.transfer(0x20);
  SPI.transfer(0x10);//default value error
  SPI.transfer(0x20); //intern
  //SPI.transfer(0x00); //extern
  digitalWrite(ss,HIGH);

  digitalWrite(ss,LOW);   
  SPI.transfer(0x10);
  SPI.transfer(0x80);
  SPI.transfer(0x01);
  digitalWrite(ss,HIGH);
}
void setup() {
  Serial.begin(9600);         // opens serial port at 9600 bps
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  
  AD_init();
}

void loop() {
   byte dat1=0x01;
   byte dat2=0x02;
   byte dat3=0x02;
  byte rx_data_bytes[3];
   static long temp;
  // read the incoming frame bytes in the serial buffer
   digitalWrite(ss,LOW); 
   delayMicroseconds(20);
   if(digitalRead(MISO)==LOW)
   {
        digitalWrite(ss,LOW); 
        SPI.transfer(0x44);
        dat1=SPI.transfer(0);
        dat2=SPI.transfer(0);
        dat3=SPI.transfer(0);
        digitalWrite(ss,HIGH);

        temp=dat1*65536+dat2*256+dat3;
       
   }
  if(Serial.available()>0)
  { 
     for (int i = 0; i < 3; ++i) {
     rx_data_bytes[i] = Serial.read();

    // delay 2 milliseconds before next reading otherwise the data may lost
     delay(2);                                               
   }
    if ((rx_data_bytes[0] == 0x55) && (rx_data_bytes[1] == 0xAA)&&(rx_data_bytes[2] == 0x10))
   { 
        Serial.print(temp,DEC);
   }

  }
}



