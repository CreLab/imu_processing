#include "sensor.h"
#include "algo.h"
#include "Streaming.h" 		// needed for the Serial output https://github.com/geneReeves/ArduinoStreaming

#ifndef DUMMY_DATA
static s_sensor_t SensorDataStruct;
static void prepareSensorDataPackage(uint8_t* pSendDataStruct, s_sensor_t* pSensorDataStruct);
#else
static void prepareDummyDataPackage(uint8_t* SendDataStruct);
#endif

static void swap(void* input, void* output, uint8_t size);

void setup()
{
  // serial to display data
  Serial.begin(115200);
  while (!Serial)
  {}

  Serial.println("Start System");
  #ifndef DUMMY_DATA
  sensor_Initialize();
  SensorDataStruct.id = 0xA0FFFFB1;
  #endif  

  Serial.println("Start System ... Done");
}

void loop()
{
    uint8_t SendDataStruct[52] = {0};

#ifndef DUMMY_DATA
      sensor_Read_All(&SensorDataStruct);

#ifdef PROCESSING_ACTIVE
  #ifdef QUATERNION_ALGO
      QuaternionUpdate(SensorDataStruct.Gx, SensorDataStruct.Gy, SensorDataStruct.Gz, 
                      SensorDataStruct.Ax, SensorDataStruct.Ay, SensorDataStruct.Az, 
                      SensorDataStruct.Mx, SensorDataStruct.My, SensorDataStruct.Mz);
  #else
      AHRSupdate(SensorDataStruct.Gx, SensorDataStruct.Gy, SensorDataStruct.Gz, 
                SensorDataStruct.Ax, SensorDataStruct.Ay, SensorDataStruct.Az, 
                SensorDataStruct.Mx, SensorDataStruct.My, SensorDataStruct.Mz);
  #endif
      
      if (EulerUpdate(&SensorDataStruct.roll, &SensorDataStruct.pitch, &SensorDataStruct.yaw))
      {
        prepareSensorDataPackage(&SendDataStruct[0], &SensorDataStruct);
        Serial.write(&SendDataStruct[0], sizeof(SendDataStruct));
      }
#else
      prepareSensorDataPackage(&SendDataStruct[0], &SensorDataStruct);
      Serial.write(&SendDataStruct[0], sizeof(SendDataStruct));
#endif
#else
      prepareDummyDataPackage(SendDataStruct);
      Serial.write(&SendDataStruct[0], sizeof(SendDataStruct));
#endif
}

static void swap(void* input, void* output, uint8_t size)
{
	uint8_t* data = (uint8_t*)input;
  uint8_t* tmp = (uint8_t*)output;

  for (uint8_t cnt = 0; cnt < size; cnt++)
  { 
    tmp[cnt] = data[size - (cnt + 1)];
  }
}

#ifndef DUMMY_DATA
static void prepareSensorDataPackage(uint8_t* pSendDataStruct, s_sensor_t* pSensorDataStruct)
{
      swap(&pSensorDataStruct->id, &pSendDataStruct[0],sizeof(uint32_t));
      
      swap(&pSensorDataStruct->Ax,&pSendDataStruct[4],sizeof(float));
      swap(&pSensorDataStruct->Ay,&pSendDataStruct[8],sizeof(float));
      swap(&pSensorDataStruct->Az,&pSendDataStruct[12],sizeof(float));

      swap(&pSensorDataStruct->Gx,&pSendDataStruct[16],sizeof(float)); 
      swap(&pSensorDataStruct->Gy,&pSendDataStruct[20],sizeof(float)); 
      swap(&pSensorDataStruct->Gz,&pSendDataStruct[24],sizeof(float)); 

      swap(&pSensorDataStruct->My,&pSendDataStruct[28],sizeof(float)); 
      swap(&pSensorDataStruct->Mx,&pSendDataStruct[32],sizeof(float)); 
      swap(&pSensorDataStruct->Mz,&pSendDataStruct[36],sizeof(float));

      swap(&pSensorDataStruct->roll,&pSendDataStruct[40],sizeof(float)); 
      swap(&pSensorDataStruct->pitch,&pSendDataStruct[44],sizeof(float)); 
      swap(&pSensorDataStruct->yaw,&pSendDataStruct[48],sizeof(float));
}
#else
static void prepareDummyDataPackage(uint8_t* pSendDataStruct)
{
      uint32_t id = 0xA0FFFFB1;
      int32_t fourByte = 0x01234567;

      swap(&id, &pSendDataStruct[0],sizeof(uint32_t));
      swap(&fourByte, &pSendDataStruct[4],sizeof(float));
      swap(&fourByte, &pSendDataStruct[8],sizeof(float));
      swap(&fourByte, &pSendDataStruct[12],sizeof(float));

      swap(&fourByte, &pSendDataStruct[16],sizeof(float)); 
      swap(&fourByte, &pSendDataStruct[20],sizeof(float)); 
      swap(&fourByte, &pSendDataStruct[24],sizeof(float)); 

      swap(&fourByte, &pSendDataStruct[28],sizeof(float)); 
      swap(&fourByte, &pSendDataStruct[32],sizeof(float)); 
      swap(&fourByte, &pSendDataStruct[36],sizeof(float));

      swap(&fourByte,&pSendDataStruct[40],sizeof(float)); 
      swap(&fourByte,&pSendDataStruct[44],sizeof(float)); 
      swap(&fourByte,&pSendDataStruct[48],sizeof(float));
}
#endif
