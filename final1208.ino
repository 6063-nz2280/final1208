// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 2  // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 16  // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500  // Time (in milliseconds) to pause between pixels

int brightness = 255;
int buttonPin = 3;

bool isPlaying = true;
bool isEnabled = false;

// Variables will change:
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers



void setup() {
  pinMode(buttonPin, INPUT);
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  if (Serial.available() > 0) {
    String strIn = Serial.readStringUntil('\n');
    Serial.println(strIn);
    if (strIn == "on") {
      Serial.println("turn on the light");
      isEnabled = HIGH;
    } else if (strIn == "off") {
      Serial.println("turn off the light");
      isEnabled = LOW;
    } else {
      Serial.println("something else");
    }

    detectButtonClick();
    pixels.clear();  // Set all pixel colors to 'off'

    // The first NeoPixel in a strand is #0, second is 1, all the way up
    // to the count of pixels minus one.
    if (isPlaying && isEnabled) {
      displayDesign0();
      displayDesign1();
      displayDesign2();

      pixels.setBrightness(brightness);
      delay(1);
      pixels.show();

      brightness -= 50;
      brightness = constrain(brightness, 0, 255);
      if (brightness == 0) {
        brightness = 255;
      }
    } else {
      pixels.clear();
      pixels.show();
    }
    delay(50);
  }
}


void displayDesign0() {
  for (int i = 0; i < NUMPIXELS / 2; i++) {  // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    detectButtonClick();
    if (!isPlaying || !isEnabled) break;
    pixels.setPixelColor(i, pixels.Color(100, 150, 100));
    pixels.setPixelColor(NUMPIXELS - i - 1, pixels.Color(100, 150, 100));

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(DELAYVAL);  // Pause before next pass through loop
  }
}

void displayDesign1() {
  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
    detectButtonClick();
    if (!isPlaying || !isEnabled) break;
    int hue = 0 + i * 65536L / NUMPIXELS;
    uint32_t color = pixels.gamma32(pixels.ColorHSV(hue));  // hue -> RGB
    pixels.setPixelColor(i, color);

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(DELAYVAL);  // Pause before next pass through loop
  }
}
void displayDesign2() {
  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
    detectButtonClick();
    if (!isPlaying || !isEnabled) break;
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if (i % 2 == 0) {
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 150));
    }

    // Send the updated pixel colors to the hardware.
    delay(5);
  }
}

void detectButtonClick() {

  int reading = digitalRead(buttonPin);
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        isPlaying = !isPlaying;
        // Serial.println(isPlaying);
      }
    }
  }


  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  Serial.print("is playing:");
  Serial.println(isPlaying);
}