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
    Serial.println("Init ICM_20948 Sensor");
    Wire.begin();
    Wire.setClock(400000);

    ICM.begin(Wire, AD0_VAL);

    Serial.print("Initialization of the sensor returned: ");
    Serial.println(ICM.statusString());
    if (ICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Sensor Init Failed ...");
      while(1)
      {}
    }

    Serial.println("Doing SW reset ...");
    ICM.swReset();
    if (ICM.status != ICM_20948_Stat_Ok)
    {
      Serial.print("Software Reset returned: ");
      Serial.println(ICM.statusString());
    }
    delay(250);

    Serial.println("Wake up sensor ...");
    ICM.sleep(false);
    ICM.lowPower(false);

    Serial.println("Set sample mode of Gyro and Accelerometer ...");
    ICM.setSampleMode((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), ICM_20948_Sample_Mode_Continuous);
    if (ICM.status != ICM_20948_Stat_Ok)
    {
      Serial.print("setSampleMode returned: ");
      Serial.println(ICM.statusString());
    }

    Serial.println("Set full scale ranges for both acc and gyr ...");
    ICM_20948_fss_t myFSS;
    myFSS.a = gpm2;
    myFSS.g = dps250;

    ICM.setFullScale((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myFSS);
    if (ICM.status != ICM_20948_Stat_Ok)
    {
      Serial.print("setFullScale returned: ");
      Serial.println(ICM.statusString());
    }

    Serial.println("Set up Digital Low-Pass Filter configuration ...");
    ICM_20948_dlpcfg_t myDLPcfg;
    myDLPcfg.a = acc_d473bw_n499bw; 
    myDLPcfg.g = gyr_d361bw4_n376bw5;

    ICM.setDLPFcfg((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myDLPcfg);
    if (ICM.status != ICM_20948_Stat_Ok)
    {
      Serial.print("setDLPcfg returned: ");
      Serial.println(ICM.statusString());
    }

    Serial.println("Choose whether or not to use DLPF ...");
    ICM_20948_Status_e accDLPEnableStat = ICM.enableDLPF(ICM_20948_Internal_Acc, false);
    ICM_20948_Status_e gyrDLPEnableStat = ICM.enableDLPF(ICM_20948_Internal_Gyr, false);
    Serial.print("Enable DLPF for Accelerometer returned: ");
    Serial.println(ICM.statusString(accDLPEnableStat));
    Serial.print("Enable DLPF for Gyroscope returned: ");
    Serial.println(ICM.statusString(gyrDLPEnableStat));

    Serial.println("Choose whether or not to start the magnetometer ...");
    ICM.startupMagnetometer();
    if (ICM.status != ICM_20948_Stat_Ok)
    {
      Serial.print("startupMagnetometer returned: ");
      Serial.println(ICM.statusString());
    }

    Serial.println("Init Sensor ... Done");
}

void sensor_Read_All(s_sensor_t *DataStruct)
{
  if (ICM.dataReady())
  {
    ICM.getAGMT();

#ifdef FREE_IMU_CAL
    DataStruct->Ax = ICM.accX();
    DataStruct->Ay = ICM.accY();
    DataStruct->Az = ICM.accZ();
#else
    /* Calibrating from acc.txt
    * Offsets:
    * [-149.3554817662568, -164.25065584365404, -213.05733970182368]
    * Scales:
    * [856.1995468434739, nan, nan]
    */
    DataStruct->Ax = (ICM.accX() - (-149.3554817662568)) / 856.1995468434739;
    DataStruct->Ay = (ICM.accY() - (-164.25065584365404)) / 1.0;
    DataStruct->Az = (ICM.accZ() - (-213.05733970182368)) / 1.0;
#endif

    DataStruct->Gx = ICM.gyrX();
    DataStruct->Gy = ICM.gyrY();
    DataStruct->Gz = ICM.gyrZ();

#ifdef FREE_IMU_CAL
    DataStruct->Mx = ICM.magX();
    DataStruct->My = ICM.magY();
    DataStruct->Mz = ICM.magZ();
#else
    /* Calibrating from magn.txt
    * Offsets:
    * [0.2697052658330848, 41.382911220553474, -6.751119894511462]
    * Scales:
    * [49.337575641173665, 51.36571121729221, 53.01166375673847]
    */
    DataStruct->Mx = (ICM.magX() - 0.2697052658330848) / 49.337575641173665;
    DataStruct->My = (ICM.magY() - 41.382911220553474) / 51.36571121729221;
    DataStruct->Mz = (ICM.magZ() - (-6.751119894511462)) / 53.01166375673847;
#endif
  }
}
#endif