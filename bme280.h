#ifndef BME280_H
#define BME280_H

#include "gpio.h"



class Bme280 
{

public: 

Bme280(I2C *dev); 

void init();
int get_humidity(); 

private: 

I2C *_dev;

int Temperature , Humidty , Presure; 
int calT1,calT2,calT3;
int calP1, calP2, calP3, calP4, calP5, calP6, calP7, calP8, calP9;
int calH1, calH2, calH3, calH4, calH5, calH6;

};


#endif