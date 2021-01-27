#include "gpio.h"
#include <iostream> 

#define Led       16
#define Taster    1
#define TasterONOFF 15


using namespace std; 

int main()
{

  RaspiGpio led(Led , OUT);
  RaspiGpio Taster1(Taster,IN);
  RaspiGpio TasterONOFF1(TasterONOFF,IN);

  led.ExportPin();
  TasterONOFF1.ExportPin();
  Taster1.ExportPin();


  led.DirectionPin(OUT);
  TasterONOFF1.DirectionPin(IN);
  Taster1.DirectionPin(IN);

  while(1)
  {
      if(TasterONOFF1.digitalReadPin()==0)
         {led.digitalWritePin(HIGH);
          cout <<"LED ON"<<endl;
          RaspiGpio::milisecondDelay(500);
          led.digitalWritePin(LOW);
          RaspiGpio::milisecondDelay(500);

          led.digitalWritePin(HIGH);
          cout <<"LED ON"<<endl;
          RaspiGpio::milisecondDelay(500);
          led.digitalWritePin(LOW);
          RaspiGpio::milisecondDelay(500);
         }

  }


    return 0;
}