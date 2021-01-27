#include "gpio.h"
#include "bme280.h"
#include "oled.h"
#include <sched.h>


#define ON               1
#define OFF              0
#define Automatic        1
#define Manuel           0
#define Led              16
#define LedBef           5
#define Taster           4

#define TasterONOFF      15
#define TasterEntfuschte 0
#define TasterBefuschte  1

#define Entfeuschte      1
#define Befeuschte       2 
 
using namespace std; 

pthread_mutex_t HumidtyMutex;
pthread_mutex_t mutexLed;
pthread_mutex_t mutexONOFF;



I2C *_dev = new I2C(1);
int humidty; 
int humdity2;
int modi; 
int modi2;
int lastMotorInDisplay;
int timing;
int sec2;
int zahler;
int vectorLastSize;
int statues; 
static int compter;
int Motor;


vector<int> SecVector;


pthread_t thread1; 
pthread_t thread2;
pthread_t thread3; 
pthread_t thread4;


static Bme280 sensor(_dev);
static Oled Display(_dev);
static RaspiGpio led(Led , OUT);
static RaspiGpio ledBef(LedBef,OUT);
static RaspiGpio taster(Taster,IN);
static RaspiGpio ONOFFTatser(TasterONOFF,IN);
static RaspiGpio EntfTatser(TasterEntfuschte,IN);
static RaspiGpio BefTatser(TasterBefuschte,IN);






void *threadStatues(void *arg)
{
    
  while(1)
  {
      pthread_mutex_lock(&mutexONOFF);
      if(ONOFFTatser.digitalReadPin()==0)
        {
            while(ONOFFTatser.digitalReadPin()==0){RaspiGpio::milisecondDelay(1);};
            compter += 1;
        }

            if(compter % 2 == 0)
            statues = OFF;
        else 
            statues = ON;   
        pthread_mutex_unlock(&mutexONOFF);
        
  }


}



void *threadDisplay(void *arg)
{
 

   char character[3]="0";
    char bufferEntBef[14]="Nothing To Do";
    char buff3[10] ="Automatic";
    char buff[3]="0";
    int sec= 0;
    int secbefore;
    vectorLastSize = 0;
    char buffer10[10]; 

    int prevstatues = OFF;
    lastMotorInDisplay = 0;

while(1)
{

  int statues2; 
  pthread_mutex_lock(&mutexONOFF); 
  statues2 = statues; 
  pthread_mutex_unlock(&mutexONOFF); 
  RaspiGpio::milisecondDelay(5);

   
  if(statues2 == ON)
  {
  
    pthread_mutex_lock(&HumidtyMutex);

     if(statues2 != prevstatues)
     {
         prevstatues = statues2; 
         snprintf(buffer10,8,"Welcome");
         Display.text(50,32,buffer10);
         Display.display();
         RaspiGpio::milisecondDelay(2000);
         Display.clear();
         Display.display();
     }
 

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


       
        if(lastMotorInDisplay != Motor)
         {
          lastMotorInDisplay = Motor; 
          if(lastMotorInDisplay == Entfeuschte)
          {
              snprintf(bufferEntBef ,13,"Dehumidifier");
              Display.clearLoc(4,55,127);
              Display.clearLoc(4 ,56,127);
              Display.clearLoc(4 ,57,127);
              Display.clearLoc(4 ,57,127);
              Display.clearLoc(4 ,57,127);
              Display.clearLoc(4 ,57,127);
              Display.display();
          }


          else if(lastMotorInDisplay == Befeuschte)
          {
              snprintf(bufferEntBef ,11,"Humidifier");
              Display.clearLoc(4,55,127);
              Display.clearLoc(4 ,56,127);
              Display.clearLoc(4 ,57,127);
              Display.clearLoc(4 ,57,127);
              Display.clearLoc(4 ,57,127);
              Display.clearLoc(4 ,57,127);
              Display.display();
          }  

          else if (lastMotorInDisplay == 0)
          {
              snprintf(bufferEntBef,14,"Nothing To Do");
              Display.clearLoc(4,55,127);
              Display.clearLoc(4 ,56,127);
              Display.clearLoc(4 ,57,127);
              Display.clearLoc(4 ,57,127);
              Display.clearLoc(4 ,57,127);
              Display.clearLoc(4 ,57,127);
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
        Display.text(4,55,bufferEntBef);
        Display.display();
        pthread_mutex_unlock(&HumidtyMutex);
        RaspiGpio::milisecondDelay(200);
}

else 
{
    prevstatues = OFF;
    pthread_mutex_lock(&HumidtyMutex);
    Display.clear();
    Display.display();
    pthread_mutex_unlock(&HumidtyMutex);
}
}
pthread_exit(NULL);
}





void *threadSensor(void *arg)
{


   
	while(1)
	{

  int statues2; 
  pthread_mutex_lock(&mutexONOFF); 
  statues2 = statues; 
  pthread_mutex_unlock(&mutexONOFF); 
  RaspiGpio::milisecondDelay(5);

         if(statues2 == ON)
    {
        pthread_mutex_lock(&HumidtyMutex);
		humidty = sensor.get_humidity(); 
        pthread_mutex_unlock(&HumidtyMutex);

        if(humidty > 40 && modi == Automatic)
        {
            pthread_mutex_lock(&mutexLed);
           
           led.digitalWritePin(HIGH);
            while(humidty > 40)
            {
                 pthread_mutex_lock(&HumidtyMutex);
                 humidty = sensor.get_humidity();
                 pthread_mutex_unlock(&HumidtyMutex);
                 RaspiGpio::milisecondDelay(50);
               
            }


                led.digitalWritePin(LOW);
        }

        else if (humidty < 30 && modi == Automatic)
        {
            ledBef.digitalWritePin(HIGH);
           while(humidty < 30)
           {
               pthread_mutex_lock(&HumidtyMutex);
               humidty = sensor.get_humidity();
               pthread_mutex_unlock(&HumidtyMutex);
               RaspiGpio::milisecondDelay(50);
           }
        
         ledBef.digitalWritePin(LOW);


        }


             pthread_mutex_unlock(&mutexLed);

             
        
       
        RaspiGpio::milisecondDelay(100);
	}


    }
    pthread_exit(NULL);
}









void  *threadLedTaster(void *arg)
{


int prevstatues = OFF;
while(1)
{


 int statues2; 
 //pthread_mutex_lock(&mutexONOFF); 
  statues2 = statues; 
  //pthread_mutex_unlock(&mutexONOFF); 
 //RaspiGpio::milisecondDelay(10);



if(statues2 == ON)
{

if (prevstatues != statues2)
{
    prevstatues = statues2;
    led.digitalWritePin(HIGH);
    ledBef.digitalWritePin(HIGH);
    RaspiGpio::milisecondDelay(500);
    led.digitalWritePin(LOW);
    ledBef.digitalWritePin(LOW);
    RaspiGpio::milisecondDelay(500);

    led.digitalWritePin(HIGH);
    ledBef.digitalWritePin(HIGH);
    RaspiGpio::milisecondDelay(500);
    led.digitalWritePin(LOW);
    ledBef.digitalWritePin(LOW);
    RaspiGpio::milisecondDelay(500);
       led.digitalWritePin(HIGH);
    ledBef.digitalWritePin(HIGH);
    RaspiGpio::milisecondDelay(500);
    led.digitalWritePin(LOW);
    ledBef.digitalWritePin(LOW);
}

int compter=0;

zahler = 0;
compter = 0;

if(taster.digitalReadPin()==0)
{
zahler +=1;  
timing = 600; 
modi = Manuel;
//led.digitalWritePin(HIGH);
sec2 = ((zahler * 600)-compter) / 10;
SecVector.push_back(sec2);

while(timing > 0 )
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

    if(statues == OFF)
    {
        led.digitalWritePin(LOW);
        break;
    }   

    if(EntfTatser.digitalReadPin()==0 )
        {led.digitalWritePin(HIGH);
         Motor = Entfeuschte;
         ledBef.digitalWritePin(LOW);
         }

    if(BefTatser.digitalReadPin()==0)
        {ledBef.digitalWritePin(HIGH);
          Motor = Befeuschte;
          led.digitalWritePin(LOW);
          }        
}



  led.digitalWritePin(LOW);
  ledBef.digitalWritePin(LOW);
  pthread_mutex_lock(&HumidtyMutex);
  humidty = sensor.get_humidity();
   if(humidty > 40)
   led.digitalWritePin(HIGH);
    if(humidty < 30)
   ledBef.digitalWritePin(HIGH);
   pthread_mutex_unlock(&HumidtyMutex);

modi = Automatic;
Motor = 0;
}

}

else {led.digitalWritePin(LOW);
       ledBef.digitalWritePin(LOW);
       prevstatues = OFF;}
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
  compter = 0;
  modi = Automatic;
  modi2 = Automatic;
  statues = 0;
  Motor = 0;

  taster.ExportPin(); 
  taster.DirectionPin(IN);
   
  led.ExportPin();
  led.DirectionPin(OUT);
  
  ledBef.ExportPin();
  ledBef.DirectionPin(OUT);


   
  BefTatser.ExportPin();
  BefTatser.DirectionPin(IN);

  EntfTatser.ExportPin();
  EntfTatser.DirectionPin(IN);

  ONOFFTatser.ExportPin(); 
  ONOFFTatser.DirectionPin(IN);

  


  pthread_mutex_init(&HumidtyMutex,0);
  pthread_mutex_init(&mutexLed,0);
  pthread_mutex_init(&mutexONOFF,0);



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
   pthread_create(&thread4,NULL,threadStatues,NULL);

   struct sched_param params;
   params.sched_priority = sched_get_priority_max(SCHED_FIFO);
   pthread_setschedparam(thread4,SCHED_FIFO,&params);


   
   




   while(1)
   {
  int statues2; 
  pthread_mutex_lock(&mutexONOFF); 
  statues2 = statues; 
  pthread_mutex_unlock(&mutexONOFF); 
  cout <<"Motor = " <<Motor <<endl; 
  if(statues2 ==ON)
    {cout<<"ON"<<endl;
    cout <<"compteur ON OFF " << compter <<endl;}

  else 
    {cout <<"OFF"<<endl;
    cout <<"compteur ON OFF " << compter <<endl;}  
   RaspiGpio::milisecondDelay(10);
       if (statues2 == ON )
       {
      pthread_mutex_lock(&HumidtyMutex);
      std::cout <<"Humidty = " << humidty<<endl;
      std::cout <<"Timing = " << sec2<<endl;
      pthread_mutex_unlock(&HumidtyMutex);
      RaspiGpio::milisecondDelay(200);
       }

   }

  atexit(&DestroyMutex);

    return 0;
}



