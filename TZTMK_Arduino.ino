#include "Keyboard.h"
#include "Mouse.h"
#include <SoftwareSerial.h>

SoftwareSerial serialPort(10, 11); // RX, TX

uint8_t bytesLeftInTransmission = 0;

// zmienna odpowiadająca za typ transmisji; używana dla transmisji dłuższej niż jeden bajt
// 1 - klawisz w dół
// 2 - klawisz w górę
// 3 - ruch myszki
uint8_t transmissionType = 0;

int moveX = 0;
int moveY = 0;

void setup() { 
  // prędkość transmisji portu szeregowego
  serialPort.begin(19200);
  // inicjalizacja emulacji urządzeń HID
  Mouse.begin();
  Keyboard.begin();
}

void loop() {
  if (serialPort.available()) {
    byte inChar = serialPort.read();
    // sprawdzamy czy jest już jakaś transmisja aktywna
    if(bytesLeftInTransmission > 0)
    {
      // jest jakaś transmisja aktywna - stosowane tylko przy transmisji skladającej się z więcej niż jednego bajta
      switch(transmissionType){
        case 1:
          // aktywna transmisja nacisnięcia klawisza
          Keyboard.press(inChar);
          transmissionType = 0;
          break;
        case 2:
          // aktywna transmisja zwolnienia klawisza
          Keyboard.release(inChar);
          transmissionType = 0;
          break;
        case 3:
          // aktywna transmisja ruchu myszka
          if(bytesLeftInTransmission == 2){
            moveX = inChar;
          }
          if(bytesLeftInTransmission == 1){
            moveY = inChar;
            int moveXtransformed = moveX - 128;
            int moveYtransformed = moveY - 128;
            Mouse.move(moveXtransformed,moveYtransformed,0);
            transmissionType = 0;
          }
          break;
        default:
          break;
      }

      // zdejmujemy bajt z transmisji
      bytesLeftInTransmission--;
    }
    else
    {
      // nie ma żadnej transmisji aktywnej, czyli jak coś przyszło to początek nowej transmisji
      switch(inChar){
        case 'K':
          // nacisnięcie klawisza
          // nowa transmisja z jednym bajtem nagłówka i jednym bajtem danych
          transmissionType = 1;
          bytesLeftInTransmission = 1;
          break;
        case 'k':
          // zwolnienie klawisza
          transmissionType = 2;
          bytesLeftInTransmission = 1;
          break;
        case 'P': 
          // ruch myszki
          // nowa transmisja z jednym bajtem nagłówka i dwoma bajtami danych
          transmissionType = 3;
          bytesLeftInTransmission = 2;
          break;
        case 'L':
          // nowa transmisja z jednym bajtem nagłówka bez bajtow danych
          // nie otwieramy nowej transmisji (nie ustawiamy transmissionType) ponieważ nie ma bajtów danych i od razu wykonamy obsługę -  w tym wypadku naciśnięcie przycisku myszki
          Mouse.press(MOUSE_LEFT);
          break;
        case 'l':
          Mouse.release(MOUSE_LEFT);
          break;
        case 'R':
          Mouse.press(MOUSE_RIGHT);
          break;
        case 'r':
          Mouse.release(MOUSE_RIGHT);
          break;
        case 'M':
          Mouse.press(MOUSE_MIDDLE);
          break;
        case 'm':
          Mouse.release(MOUSE_MIDDLE);
          break;
        case 'S':
          Mouse.move(0,0,-1);
          break;
        case 's':
          Mouse.move(0,0,1);
          break;
        default:
          break;
      }
    }
  }
}
