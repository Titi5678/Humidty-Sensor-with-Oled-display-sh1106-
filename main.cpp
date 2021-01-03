#include "gpio.h"
#include "bme280.h"
#include "oled.h"

#define Automatic 1
#define Manuel    0
#define Led       16
#define Taster    4

using namespace std; 

pthread_mutex_t HumidtyMutex;
pthread_mutex_t mutexLed;

I2C *_dev = new I2C(1);
int humidty; 
int humdity2;
int modi; 
int modi2;
int timing;
int sec2;
int zahler;
int vectorLastSize;


vector<int> SecVector;


pthread_t thread1; 
pthread_t thread2;
pthread_t thread3; 
pthread_t thread4;


static Bme280 sensor(_dev);
static Oled Display(_dev);
static RaspiGpio led(Led , OUT);
static RaspiGpio taster(Taster,IN);





void *threadDisplay(void *arg)
{
    char character[3]="0";
    char buff3[10] ="Automatic";
    char buff[3]="0";
    int sec= 0;
    int secbefore;
    vectorLastSize = 0;

while(1)
{

       pthread_mutex_lock(&HumidtyMutex);
       if(humidty != humdity2)
       {
        humdity2 = humidty; 
        snprintf(character,3,"%d",humdity2);
        Display.clearLoc(90,2,12);
        Display.display();
       }

      if(SecVector.size()==0)
        snprintf(buff,3,"%d",sec); 
     
     else 
     {
   
     
     if(SecVector.size() != vectorLastSize)
     {

         for(int x = vectorLastSize ; x < SecVector.size(); x++)
         {
             if((SecVector[x] % 10 == 0))
                sec = SecVector[x]; 
         }
     

      vectorLastSize = SecVector.size();
         
      if(sec != secbefore)
      {
      
      secbefore = sec;
      
       if(sec < 100 )
       {
        snprintf(buff,3,"%d",sec); 
        Display.clearLoc(90,35 ,24);
        Display.clearLoc(90,36 ,24);
        Display.clearLoc(90,37,24);
        Display.clearLoc(90,38 ,24);
        Display.clearLoc(90,39 ,24);
        Display.clearLoc(90,40 ,24);
        Display.display();
       }
       
       else if (sec < 1000 )
       {
        snprintf(buff,4,"%d",sec); 
        Display.clearLoc(90,35 ,24);
        Display.clearLoc(90,36 ,24);
        Display.clearLoc(90,37,24);
        Display.clearLoc(90,38 ,24);
        Display.clearLoc(90,39 ,24);
        Display.clearLoc(90,40 ,24);
        Display.display(); 
       }


       else if (sec < 10000)
       {
        snprintf(buff,5,"%d",sec); 
        Display.clearLoc(90,35 ,24);
        Display.clearLoc(90,36 ,24);
        Display.clearLoc(90,37,24);
        Display.clearLoc(90,38 ,24);
        Display.clearLoc(90,39 ,24);
        Display.clearLoc(90,40 ,24);
        Display.display(); 
       }
     }
     }
     }
       if(modi != modi2)
       {
           modi2 = modi; 
         

           if(modi2 == Manuel)
           {snprintf(buff3,7,"Manuel");
           Display.clearLoc(50,20 ,70);
           Display.clearLoc(50,21 ,70);
           Display.clearLoc(50,22 ,70);
           Display.clearLoc(50,23 ,70);
           Display.clearLoc(50,24 ,70);
           Display.clearLoc(50,25,70);
           Display.display();
           }   

           else 
           {
          snprintf(buff3,10,"Automatic");
          Display.clearLoc(50,20 ,70);
          Display.clearLoc(50,21 ,70);
          Display.clearLoc(50,22 ,70);
          Display.clearLoc(50,23 ,70);
          Display.clearLoc(50,24 ,70);
          Display.clearLoc(50,25,70);
            
            

          Display.display();  
           }
       }

        Display.text(4,2,"Humidity");
        Display.text(90,2,character);
        Display.text(118,2,"%");
        Display.text(4,20,"Mode"); 
        Display.text(60,20,buff3);
        Display.text(4,35,"Time");
        Display.text(90,35,buff);
        Display.text(118,35,"s"); 
        Display.text(4,55,"Ent Beleuschte");
        Display.display();
        pthread_mutex_unlock(&HumidtyMutex);
        RaspiGpio::milisecondDelay(200);
}

pthread_exit(NULL);
}





void *threadSensor(void *arg)
{
    
	while(1)
	{
        pthread_mutex_lock(&HumidtyMutex);
		humidty = sensor.get_humidity(); 
        if(humidty > 40 && modi == Automatic)
        {
            pthread_mutex_lock(&mutexLed);
            while(humidty > 40)
            {
                led.digitalWritePin(HIGH);
                humidty = sensor.get_humidity();
            }

             pthread_mutex_unlock(&mutexLed);
        }
        pthread_mutex_unlock(&HumidtyMutex);
        RaspiGpio::milisecondDelay(100);
	}
    pthread_exit(NULL);
}









void  *threadLedTaster(void *arg)
{

int compter=0;

while(1)
{

zahler = 0;
compter = 0;

if(taster.digitalReadPin()==0)
{
zahler +=1;  
timing = 600; 
modi = Manuel;
led.digitalWritePin(HIGH);
sec2 = ((zahler * 600)-compter) / 10;
SecVector.push_back(sec2);

while(timing > 0)
{
  
    timing -= 1; 
    compter +=1;
    RaspiGpio::milisecondDelay(100);
    sec2 = timing / 10;
    SecVector.push_back(sec2);
    if(taster.digitalReadPin()==0 &&  timing < ((zahler * 600) -10))
       { 
           timing +=600;
           zahler +=1; 
           sec2 = ((zahler * 600)-compter) / 10;
           SecVector.push_back(sec2); 
       }
}

}

led.digitalWritePin(LOW);
modi = Automatic;
}
pthread_exit(NULL);
}



void DestroyMutex()
{
    
    pthread_mutex_destroy(&HumidtyMutex);
}





int main()
{
     
  humdity2 = 0;
  timing = 0;
  modi = Automatic;
  modi2 = Automatic;

  taster.ExportPin(); 
  taster.DirectionPin(IN);
   
  led.ExportPin();
  led.DirectionPin(OUT);

  pthread_mutex_init(&HumidtyMutex,0);
  pthread_mutex_init(&mutexLed,0);

   std::cout <<"begin programm" <<endl;
   std::cout <<"Display init" <<endl;
   Display.init();
   
   RaspiGpio::milisecondDelay(2000);

   std::cout <<"begin Sensor init " <<endl;
   sensor.init();
   RaspiGpio::milisecondDelay(2000);

   pthread_create(&thread1 , NULL ,threadDisplay,NULL );
   pthread_create(&thread2,NULL,threadSensor,NULL);
   pthread_create(&thread3,NULL,threadLedTaster,NULL);
   while(1)
   {
       pthread_mutex_lock(&HumidtyMutex);
      std::cout <<"Humidty = " << humidty<<endl;
      std::cout <<"Timing = " << sec2<<endl;
      pthread_mutex_unlock(&HumidtyMutex);
      RaspiGpio::milisecondDelay(200);
   }

  atexit(&DestroyMutex);

    return 0;
}



