#include "sensor.h"

#ifdef USE_MPU9250_DRV

#if 0
  #define MPU9250_BOLDERFLIGHT_LIB
  #include "MPU9250.h"		// https://github.com/bolderflight/MPU9250
#else
  #include "MPU9250_WE.h"		// https://github.com/wollewald/MPU9250_WE
#endif

#ifdef MPU9250_BOLDERFLIGHT_LIB
  MPU9250 IMU(Wire,0x68);
#else
  MPU9250_WE IMU = MPU9250_WE(0x68);
#endif

void sensor_Initialize()
{
  Serial.println("Init MPU9250 Sensor");
  // start communication with IMU
  #ifdef MPU9250_BOLDERFLIGHT_LIB
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
  #else
  Wire.begin();
  if(!IMU.init()){
    Serial.println("MPU9250 does not respond");
  }
  else{
    Serial.println("MPU9250 is connected");
  }

  if(!IMU.initMagnetometer())
  {
    Serial.println("Magnetometer does not respond");
  }
  else
  {
    Serial.println("Magnetometer is connected");
  }

  IMU.setMagOpMode(AK8963_CONT_MODE_100HZ);
  delay(200);

  Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
  delay(1000);
  IMU.autoOffsets();
  Serial.println("Done!");

  IMU.setSampleRateDivider(5);
  IMU.setAccRange(MPU9250_ACC_RANGE_2G);
  IMU.enableAccDLPF(true);
  IMU.setAccDLPF(MPU9250_DLPF_6);

  IMU.setGyrOffsets(157.0f,-114.0f,-129.0f);
  #endif
  Serial.println("Init Sensor ... Done");
}

void sensor_Read_All(s_sensor_t *DataStruct)
{
#ifdef MPU9250_BOLDERFLIGHT_LIB
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
#else
  xyzFloat accRaw = IMU.getAccRawValues();
  xyzFloat accCorrRaw = IMU.getCorrectedAccRawValues();
  xyzFloat gValue = IMU.getGyrValues();
  xyzFloat magValue = IMU.getMagValues();

  DataStruct->Ax = accRaw.x;
  DataStruct->Ay = accRaw.y;
  DataStruct->Az = accRaw.z;
  DataStruct->Gx = gValue.x;
  DataStruct->Gy = gValue.y;
  DataStruct->Gz = gValue.z;
  DataStruct->Mx = magValue.x;
  DataStruct->My = magValue.y;
  DataStruct->Mz = magValue.z;
#endif
}
#endif