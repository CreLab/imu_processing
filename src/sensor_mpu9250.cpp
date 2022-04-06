#include "sensor.h"

#ifdef USE_MPU9250_DRV
#include "MPU9250.h"		// https://github.com/bolderflight/MPU9250

MPU9250 IMU(Wire,0x68);

void sensor_Initialize()
{
  Serial.println("Init Sensor");
  // start communication with IMU
  int32_t status = IMU.begin();
  if (status < 0)
  {
    while (1)
    {
      Serial.print("IMU initialization unsuccessful\n");
      Serial.print("Check IMU wiring or try cycling power\n");
      Serial.print("Status: ");
      Serial.println(status);
    }
  }
  Serial.println("Init Sensor ... Done");
}

void sensor_Read_All(s_sensor_t *DataStruct)
{
  IMU.readSensor();

  DataStruct->Ax = IMU.getAccelX_mss();
  DataStruct->Ay = IMU.getAccelY_mss();
  DataStruct->Az = IMU.getAccelZ_mss();
  DataStruct->Gx = IMU.getGyroX_rads();
  DataStruct->Gy = IMU.getGyroY_rads();
  DataStruct->Gz = IMU.getGyroZ_rads();
  DataStruct->Mx = IMU.getMagX_uT();
  DataStruct->My = IMU.getMagY_uT();
  DataStruct->Mz = IMU.getMagZ_uT();
}
#endif