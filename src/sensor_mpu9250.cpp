#include "sensor.h"

#ifdef USE_MPU9250_DRV

#if 1
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

  DataStruct->Mx = (IMU.getMagX_uT() - 62.99259669269141) / 49.25142271505528;
  DataStruct->My = (IMU.getMagY_uT() - 37.000401835511646) / 50.04079664168832;
  DataStruct->Mz = (IMU.getMagZ_uT() - (-47.245231720315054)) / 49.93739253676479;
#else
  xyzFloat accRaw = IMU.getAccRawValues();
  xyzFloat accCorrRaw = IMU.getCorrectedAccRawValues();
  xyzFloat gValue = IMU.getGyrValues();
  xyzFloat magValue = IMU.getMagValues();

  DataStruct->Ax = (accRaw.x + 829.9776683737704) / 11835.967506520801;
  DataStruct->Ay = (accRaw.y + 468.9941072105943) / 23235.787109756428;
  DataStruct->Az = (accRaw.z + 3399.704448082534) / 23063.453548005557;

  DataStruct->Gx = gValue.x;
  DataStruct->Gy = gValue.y;
  DataStruct->Gz = gValue.z;

  DataStruct->Mx = (magValue.x - 61.52574705754689) / 24.019885729336412;
  DataStruct->My = (magValue.y - 40.54995781046352) / 1.0;
  DataStruct->Mz = (magValue.z + 107.24235657273655) / 162.25630772708948;
#endif
}
#endif