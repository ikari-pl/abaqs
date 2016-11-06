/**
* Arduino Bluetooth Air Quality Station
* (ABAQS)
*
* ⓒ2014 Cezar „ikari” Pokorski
*
* USED ON:
*  Arduino Leonardo +
*  GP2Y1010AU0F dust sensor +
*  bluetooth module
*
* Dust sensor: https://www.sparkfun.com/datasheets/Sensors/gp2y1010au_e.pdf
* Bluetooth: http://www.lightinthebox.com/new-edition-hc06-bluetooth-wireless-module-2-pcs_p1235994.html
*
*
* CREDITS:
*
* This blog post and it's not so clear picture helped me wire the stuff together:
* http://sensorapp.net/sharp-dust-sensor-and-arduino/
*
* And then I found this almost identical project:
* https://gist.github.com/nfjinjing/8d63012c18feea3ed04e
* which I would like to credit for the AQI formula. Later I have stolen also the buffer code,
* because what the heck, it was good.
*
* And finally, the bluetooth setup that finally explained me,
* why the heck it doesn't JUST WORK:
* https://learn.sparkfun.com/tutorials/using-the-bluesmirf/example-code-using-command-mode
*/

#include <SoftwareSerial.h>

unsigned int DUST_PIN = 0;

unsigned int BLUETOOTH_TX_PIN = 3;  // TX-O pin of bluetooth module, Arduino D2
unsigned int BLUETOOTH_RX_PIN = 4;  // RX-I pin of bluetooth module, Arduino D3

unsigned int DUST_LED_PIN = 2;
// all the delays below are in MICROseconds
unsigned int DELAY_BEFORE_READ = 280;
unsigned int DELAY_AFTER_READ = 40;
unsigned int DELAY_MAX_DENSITY = 9680; // let the led "rest" and the capacitor charge(?)

unsigned int DELAY_UNTIL_LOOP = 990; // ms left before next second

char voltageStr[8];
char ppmStr[8];
char avgStr[8];
char buff[64];
const int bufferLength = 10;
float dustDensityBuffer[bufferLength];

SoftwareSerial bluetooth(BLUETOOTH_TX_PIN, BLUETOOTH_RX_PIN);

void setup() {
    Serial.begin(9600);
    pinMode(DUST_LED_PIN, OUTPUT);
    pinMode(4, OUTPUT);

    bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
    bluetooth.print("$");  // Print three times individually
    bluetooth.print("$");
    bluetooth.print("$");  // Enter command mode
    delay(100);  // Short delay, wait for the Mate to send back CMD
    bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
    // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
    bluetooth.begin(9600);  // Start bluetooth serial at 9600

    // Sharp Dust Sensor
    for (int i = 0; i < bufferLength; i++) {
        dustDensityBuffer[i] = 0;
    }

}


// AQI formula: https://en.wikipedia.org/wiki/Air_Quality_Index#United_States
float AQI(float I_high, float I_low, float C_high, float C_low, float C) {
    return (I_high - I_low) * (C - C_low) / (C_high - C_low) + I_low;
}

float dustDensityToAQI(float density) {
    int d10 = (int) (density * 10);

    if (d10 <= 0) {return 0;}
    else if (d10 <= 120) {return AQI(50, 0, 120, 0, d10);}
    else if (d10 <= 354) {return AQI(100, 51, 354, 121, d10);}
    else if (d10 <= 554) {return AQI(150, 101, 554, 355, d10);}
    else if (d10 <= 1504) {return AQI(200, 151, 1504, 555, d10);}
    else if (d10 <= 2504) {return AQI(300, 201, 2504, 1505, d10);}
    else if (d10 <= 3504) {return AQI(400, 301, 3504, 2505, d10);}
    else if (d10 <= 5004) {return AQI(500, 401, 5004, 3505, d10);}
    else if (d10 <= 10000) {return AQI(1000, 501, 10000, 5005, d10);}
    else {return 1001;}
}


void pushBuffer(float x) {
    int len = bufferLength;
    for (int i = 0; i < len - 1; i++) {
        dustDensityBuffer[i] = dustDensityBuffer[i + 1];
    }
    dustDensityBuffer[len - 1] = x;
}

float averageDensity() {
    float sum = 0;
    for (int i = 0; i < bufferLength; i++) {
        sum += dustDensityBuffer[i];
    }

    return sum / bufferLength;
}

int readRawValue() {
    int rawVal;
    // DUST_LED_PIN is any digital pin on the arduino connected to Pin 3 on the sensor
    digitalWrite(DUST_LED_PIN, LOW); // power on the LED
    delayMicroseconds(DELAY_BEFORE_READ);
    rawVal = analogRead(DUST_PIN); // read the dust value via pin 5 on the sensor
    delayMicroseconds(DELAY_AFTER_READ);
    digitalWrite(DUST_LED_PIN, HIGH); // turn the LED off
    delayMicroseconds(DELAY_MAX_DENSITY);
    return rawVal;
}

float rawToVoltage(int raw) {
    return ((float)raw) * 0.0049f;
}

void loop() {
    int dustVal = readRawValue();
    float voltage = rawToVoltage(dustVal);
    float particles = (voltage-0.54f)*166.6666667f; // *1000/6 (mili)

    pushBuffer(particles);
    float averageDustDensity = averageDensity();
    int AQI = (int) dustDensityToAQI(averageDustDensity);

    dtostrf(voltage, 2, 2, voltageStr);
    dtostrf(particles, 2, 3, ppmStr);
    dtostrf(averageDustDensity, 2, 3, avgStr);
    sprintf(buff, "%d => %s V => %s ug/m^3 => avg %s ug/m^3 => AQI: %d", dustVal, voltageStr, ppmStr, avgStr, AQI);

    if (Serial1) {
        Serial.println(buff);
    }

    bluetooth.println(buff);

    while (Serial1 && Serial.available() > 0) {
        // read the oldest byte in the serial buffer:
        byte incomingByte = Serial.read();
        if (incomingByte == 'P') {
            Serial.println("PONG");
        }
    }

    while (bluetooth.available())  // If the bluetooth sent any characters
    {
        // Send any characters the bluetooth prints to the serial monitor
        Serial.print((char) bluetooth.read());
    }

    delay(DELAY_UNTIL_LOOP);
}

