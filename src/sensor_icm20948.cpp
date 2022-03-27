#include "sensor.h"

#ifdef USE_ICM20948_DRV
#include "ICM_20948.h"  // http://librarymanager/All#SparkFun_ICM_20948_IMU

/* The value of the last bit of the I2C address.        
   On the SparkFun 9DoF IMU breakout the default is 1, and when
   the ADR jumper is closed the value becomes 0 */
#define AD0_VAL 0

ICM_20948_I2C ICM;

void sensor_Initialize()
{
    Serial.println("Init Sensor");
    // init chip
    Wire.begin();
    Wire.setClock(400000);

    ICM.begin(Wire, AD0_VAL);

    Serial.print(F("Initialization of the sensor returned: "));
    Serial.println(ICM.statusString());
    if (ICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Sensor Init Failed ...");
      while(1)
      {}
    }
    Serial.println("Init Sensor ... Done");
}

void sensor_Read_All(s_sensor_t *DataStruct)
{
  if (ICM.dataReady())
  {
    ICM.getAGMT();

    DataStruct->Ax = ICM.accX();
    DataStruct->Ay = ICM.accY();
    DataStruct->Az = ICM.accZ();
    DataStruct->Gx = ICM.gyrX();
    DataStruct->Gy = ICM.gyrY();
    DataStruct->Gz = ICM.gyrZ();
    DataStruct->Mx = ICM.magX();
    DataStruct->My = ICM.magY();
    DataStruct->Mz = ICM.magZ();
  }
}
#endif