#include "sensor.h"
#include "algo.h"
#include "Streaming.h" 		// needed for the Serial output https://github.com/geneReeves/ArduinoStreaming

static s_sensor_t SensorDataStruct;
static void prepareSensorDataPackage(uint8_t* pSendDataStruct, s_sensor_t* pSensorDataStruct);

static void swap(void* input, void* output, uint8_t size);

void setup()
{
  // serial to display data
  Serial.begin(115200);
  while (!Serial)
  {}

  Serial.println("Start System");

  sensor_Initialize();
  SensorDataStruct.id = 0xA0FFFFB1;

  Serial.println("Start System ... Done");
}

void loop()
{
    uint8_t SendDataStruct[52] = {0};
    
    sensor_Read_All(&SensorDataStruct);

#ifdef QUATERNION_ALGO
    QuaternionUpdate(SensorDataStruct.Gx, SensorDataStruct.Gy, SensorDataStruct.Gz, 
                    SensorDataStruct.Ax, SensorDataStruct.Ay, SensorDataStruct.Az, 
                    SensorDataStruct.Mx, SensorDataStruct.My, SensorDataStruct.Mz);
#else
    AHRSupdate(SensorDataStruct.Gx, SensorDataStruct.Gy, SensorDataStruct.Gz, 
              SensorDataStruct.Ax, SensorDataStruct.Ay, SensorDataStruct.Az, 
              SensorDataStruct.Mx, SensorDataStruct.My, SensorDataStruct.Mz);
#endif

    getQValues(&SensorDataStruct.q0);
    float heading = getFilteredHeading(SensorDataStruct.Mx, SensorDataStruct.My, SensorDataStruct.Mz);

#ifdef FREE_IMU_CAL
    Serial.print(SensorDataStruct.Ax, 8);
    Serial.write(",");
    Serial.print(SensorDataStruct.Ay, 8);
    Serial.print(",");
    Serial.print(SensorDataStruct.Az, 8);
    Serial.write(",");
    Serial.print(SensorDataStruct.Mx, 8);
    Serial.write(",");
    Serial.print(SensorDataStruct.My, 8);
    Serial.print(",");
    Serial.print(SensorDataStruct.Mz, 8);
    Serial.println("");

    delay(100);
#else
    Serial.print("65535");
    Serial.write(",");
    Serial.print(SensorDataStruct.Mx);
    Serial.write(",");
    Serial.print(SensorDataStruct.My);
    Serial.print(",");
    Serial.print(SensorDataStruct.Mz);
    Serial.write(",");
    Serial.print(SensorDataStruct.q0);
    Serial.write(",");
    Serial.print(SensorDataStruct.q1);
    Serial.print(",");
    Serial.print(SensorDataStruct.q2);
    Serial.print(",");
    Serial.print(SensorDataStruct.q3);
    Serial.write(",");
    Serial.print(SensorDataStruct.pitch);
    Serial.write(",");
    Serial.print(SensorDataStruct.roll);
    Serial.print(",");
    Serial.print(SensorDataStruct.yaw);
    Serial.write(",");
    Serial.print(SensorDataStruct.Ax);
    Serial.write(",");
    Serial.print(SensorDataStruct.Ay);
    Serial.print(",");
    Serial.print(SensorDataStruct.Az);
    Serial.write(",");
    Serial.print(SensorDataStruct.Gx);
    Serial.write(",");
    Serial.print(SensorDataStruct.Gy);
    Serial.print(",");
    Serial.print(SensorDataStruct.Gz);
    Serial.print(",");
    Serial.println(heading);
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
