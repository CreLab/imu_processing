#ifndef __defines_H
#define __defines_H

#include <stdint.h>
#include <Arduino.h>
//#include "Streaming.h" 		// needed for the Serial output https://github.com/geneReeves/ArduinoStreaming

#if 1
    #define USE_MPU9250_DRV
#else
    #define USE_ICM20948_DRV
#endif

#if 1
    #define MADGWICK_AHRS_ALGO
#else
    #define MAHONY_AHRS_ALGO
#endif

#if (defined(MAHONY_AHRS_ALGO) && defined(MADGWICK_AHRS_ALGO))
  #error "Please select MAHONY_AHRS_ALGO or MADGWICK_AHRS_ALGO"
#endif

#if 1
    #define QUATERNION_ALGO
#endif

typedef struct
{
    uint32_t id;
    
    float Ax;
    float Ay;
    float Az;

    float Gx;
    float Gy;
    float Gz;

  	float My;
  	float Mx;
  	float Mz;

    float roll;
  	float pitch;
  	float yaw;
} s_sensor_t;

#endif /* __MAIN_H */