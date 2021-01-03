#include "bme280.h"

 


unsigned char ucTemp[8];
unsigned char ucCal[36];


Bme280::Bme280(I2C *dev):_dev(dev)
{

     Temperature = 0; 
     Humidty = 0;
     Presure = 0; 
    
}


void Bme280::init()
{
_dev->WriteOneByte_ReadBuff(0x88 , ucCal ,24 ,0x76);
RaspiGpio::milisecondDelay(30);

_dev->WriteOneByte_ReadBuff(0xa1,&ucCal[24] ,1 ,0x76);
RaspiGpio::milisecondDelay(10);

_dev->WriteOneByte_ReadBuff(0xe1 , &ucCal[25],7 ,0x76);

	// Prepare temperature calibration data
	calT1 = ucCal[0] + (ucCal[1] << 8);
	calT2 = ucCal[2] + (ucCal[3] << 8);
	if (calT2 > 32767) calT2 -= 65536; // negative value
	calT3 = ucCal[4] + (ucCal[5] << 8);
	if (calT3 > 32767) calT3 -= 65536;
	
	// Prepare pressure calibration data
	calP1 = ucCal[6] + (ucCal[7] << 8);
	calP2 = ucCal[8] + (ucCal[9] << 8);
	if (calP2 > 32767) calP2 -= 65536; // signed short
	calP3 = ucCal[10] + (ucCal[11] << 8);
	if (calP3 > 32767) calP3 -= 65536;
	calP4 = ucCal[12] + (ucCal[13] << 8);
	if (calP4 > 32767) calP4 -= 65536;
	calP5 = ucCal[14] + (ucCal[15] << 8);
	if (calP5 > 32767) calP5 -= 65536;
	calP6 = ucCal[16] + (ucCal[17] << 8);
	if (calP6 > 32767) calP6 -= 65536;
	calP7 = ucCal[18] + (ucCal[19] << 8);
	if (calP7 > 32767) calP7 -= 65536;
	calP8 = ucCal[20] + (ucCal[21] << 8);
	if (calP8 > 32767) calP8 -= 65536;
	calP9 = ucCal[22] + (ucCal[23] << 8);
	if (calP9 > 32767) calP9 -= 65536;

	// Prepare humidity calibration data
	calH1 = ucCal[24];
	calH2 = ucCal[25] + (ucCal[26] << 8);
	if (calH2 > 32767) calH2 -= 65536;
	calH3 = ucCal[27];
	calH4 = (ucCal[28] << 4) + (ucCal[29] & 0xf);
	if (calH4 > 2047) calH4 -= 4096; // signed 12-bit
	calH5 = (ucCal[30] << 4) + (ucCal[29] >> 4);
	if (calH5 > 2047) calH5 -= 4096;
	calH6 = ucCal[31];
	if (calH6 > 127) calH6 -= 256; // signed char

    uint8_t buff[2] = {0xf2,0x01}; //Control Humidty register , Humidty over sampling Rate
    _dev->Write_Bytes(buff,2 ,0x76);   
    
    buff[0] = 0xf4; //control measurment register
    buff[1] = 0x27; //normal mode Temp
    _dev->Write_Bytes(buff ,2 ,0x76); 

    buff[0] = 0xf5; 
    buff[1] = 0xa0;
    _dev->Write_Bytes(buff ,2 ,0x76);
 
}




int Bme280::get_humidity()
{
int i,rc;
int t, p, h; // raw sensor values
int var1,var2,t_fine;


_dev->WriteOneByte_ReadBuff(0xf7 , ucTemp , 8 ,0x76);

p = (ucTemp[0] << 12) + (ucTemp[1] << 4) + (ucTemp[2] >> 4);
t = (ucTemp[3] << 12) + (ucTemp[4] << 4) + (ucTemp[5] >> 4);
h = (ucTemp[6] << 8) + ucTemp[7];

var1 = ((((t >> 3) - (calT1 <<1))) * (calT2)) >> 11;
var2 = (((((t >> 4) - (calT1)) * ((t>>4) - (calT1))) >> 12) * (calT3)) >> 14;

t_fine = var1 + var2;


Temperature = (t_fine * 5 + 128) >> 8;

var1 = (t_fine - 76800);
var1 = (((((h << 14) - ((calH4) << 20) - ((calH5) * var1)) + 
(16384)) >> 15) * (((((((var1 * (calH6)) >> 10) * (((var1 * (calH3)) >> 11) + (32768))) >> 10) + (2097152)) * (calH2) + 8192) >> 14));
var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * (calH1)) >> 4));
var1 = (var1 < 0? 0 : var1);
var1 = (var1 > 419430400 ? 419430400 : var1);
Humidty = var1 >> 12;

return Humidty /1024.0;

}











/*

int main()
{
	I2C *_dev = new I2C(1);
	Bme280 sensor(_dev);
	std::cout <<"begin" <<std::endl;
	sensor.init();
	RaspiGpio::milisecondDelay(500);

	while (1)
	{
		
		
    std::cout<<"Humidty = " <<sensor.get_humidity()<<" %."<<std::endl;
	}
	
	
	
	
	return 0;
}

*/
