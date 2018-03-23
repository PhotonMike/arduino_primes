#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Thread.h>
#include <math.h>
#include <SD.h>

#define ledpin 12

LiquidCrystal_I2C lcd(0x3F,16,3);
Thread scr_thread = Thread();

unsigned long int cur = 0;
unsigned long int last = 0;
unsigned long int ciklus = 0;
bool sdAvail = false;

bool is_prime(const unsigned long int num)
{
	unsigned long int b = 2;
	if (num == 1L) {
		return false;
	}
	else if (num == 2L) {
		return true;
	}
	bool i = true;
	while (i)
	{
		if (num%b == 0L)
		{
			i = false;
		}
		else
		{
			if (b >= sqrt(num))
			{
				return true;
			}
			else
			{
				b++;
			}
		}
	}
	return false;
}

void sd_save(){
    String sdOut = String(cur);
    SD.remove("primes.txt");
    File dataFile = SD.open("primes.txt", FILE_WRITE);
    lcd.setCursor(0,3);
    if(dataFile) {
        dataFile.print(sdOut);
        dataFile.close();
        lcd.print("SD: " + sdOut);
        digitalWrite(ledpin, HIGH);
        delay(20);
        digitalWrite(ledpin, LOW);
    }
    else {
        lcd.print("SD hiba");
        sdAvail = false;
    }
}

void scr_prnt() {
    lcd.setCursor(0, 2);
    lcd.print(cur);
    if ((cur - last > 1000) && sdAvail) {
        sd_save();
        last = cur;
    }
}

void sdRead() {
    File read = SD.open("primes.txt");
    String out = "";
    lcd.setCursor(0,3);
    while(read.available()) {
        out += (char)read.read();
    }
    ciklus = out.toInt();
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(ledpin, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    //Set up a pin to power the SD card
    pinMode(48, OUTPUT);
    digitalWrite(48, HIGH);

    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Primszam kereso");
    lcd.setCursor(0, 1);
    lcd.print("Jelenlegi prim: ");
    scr_thread.enabled = true;
    scr_thread.onRun(scr_prnt);
    lcd.setCursor(0,3);
    if (!SD.begin(53)){
        lcd.print("SD hiba");
    }
    else {
        sdAvail = true;
        lcd.print("SD");
        sdRead();
    }

    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
    if (is_prime(ciklus)){
        cur = ciklus;
        scr_thread.run();
    }
    ciklus++;
}