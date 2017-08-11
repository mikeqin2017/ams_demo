/**
 * @file arduino.ino
 *
 * @brief AMS(Arduino Measurement Shield) sample code
 *
 * This program configures a Arduino as a 4-wire SPI Single Master and
 * acquire data from AMS at various data rate. The labview in PC acquire the
 * signal data from Arduino using a simple protocol via UART communication.
 * 
 * Target:         Arduino Duo
 * Tool chain:     Arduino IDE 1.8.2
 * Command Line:   None
 *
 * @author  shuwen (shuwen.ou@thermofisher.com)
 * @author  mikeqin (mike.qin@thermofisher.com)
 * @version 1.0
 * @date    2017-08-04
 * @bug     
 *
 * Revision history:
 * $Log:$
 *
 */

#include <SPI.h>

const int chipSelectPin = 10;   ///< set pin 10 as slave select for digital pot
byte rx_data_bytes[3] = {0};    ///< for received serial data frame bytes

void read_data_frame(void);
void AD_init(void);

/**
 * @brief setup
 *
 * put your setup code here, to run once.
 *
 * @return void
 */
void setup() {
    Serial.begin(9600);     
    pinMode(chipSelectPin, OUTPUT);
    SPI.begin();
    SPI.setDataMode(SPI_MODE3);
    SPI.setClockDivider(SPI_CLOCK_DIV64);
    AD_init();
}

/**
 * @brief loop
 * 
 * put your main code here, to run repeatedly
 *
 * @return void
 */
void loop() {
    byte dat1 = 0x01;
    byte dat2 = 0x02;
    byte dat3 = 0x02;
    static long adc_data;

    // read the incoming frame bytes in the serial buffer
    digitalWrite(chipSelectPin, LOW); 
    delayMicroseconds(20);
    if (digitalRead(MISO) == LOW) {
        digitalWrite(chipSelectPin,LOW); 
        SPI.transfer(0x44);
        dat1 = SPI.transfer(0);
        dat2 = SPI.transfer(0);
        dat3  =SPI.transfer(0);
        digitalWrite(chipSelectPin,HIGH);

        adc_data = dat1 * 65536 + dat2 * 256 + dat3;
    }

    if (Serial.available() > 0) {
        // read the received frame bytes in serial buffer
        read_data_frame();

        if ((rx_data_bytes[0] == 0x55) && (rx_data_bytes[1] == 0xAA) &&
                (rx_data_bytes[2] == 0x10)) {
            Serial.print(adc_data, DEC);
        }
    }
}

/**
 * @brief ADC init
 *
 * @param none
 * @return void
 */
void AD_init()
{
    digitalWrite(chipSelectPin,LOW);   
    SPI.transfer(0x01);
    SPI.transfer(0xA0);
    SPI.transfer(0);  // Continous mode and enable filter data rate control
    digitalWrite(chipSelectPin,HIGH);

    digitalWrite(chipSelectPin,LOW);   
    SPI.transfer(0x28);
    SPI.transfer(0x00);
    //SPI.transfer(0x0b); //500Hz
    //SPI.transfer(0x14); //5Hz
    //SPI.transfer(0x03); //50khz
    SPI.transfer(0x09); //2500Hz
    //SPI.transfer(0x00); //25000Hz
    digitalWrite(chipSelectPin,HIGH);

    digitalWrite(chipSelectPin,LOW);   
    SPI.transfer(0x20);
    SPI.transfer(0x10); //default value error
    SPI.transfer(0x20); //internal ref
    //SPI.transfer(0x00); //external ref
    digitalWrite(chipSelectPin,HIGH);

    digitalWrite(chipSelectPin,LOW);   
    SPI.transfer(0x10);
    SPI.transfer(0x80);
    SPI.transfer(0x01);
    digitalWrite(chipSelectPin,HIGH);
}

/**
 * @brief read data frame
 *
 * @return void
 */
void read_data_frame(void) {
    // read the incoming frame bytes in the serial buffer
    for (int i = 0; i < 3; ++i) {
        rx_data_bytes[i] = Serial.read();

        // delay 2 milliseconds before next reading otherwise the data may lost
        delay(2);                                               
    }
}
