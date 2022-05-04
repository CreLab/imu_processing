#include "algo.h"
#include "quaternionFilters.h"
#include "MadgwickAHRS.h"
#include "MahonyAHRS.h"

float deltat = 0.0f;
float time_previous = 0.0f;

float deltat2 = 0.0f;
float time_previous2 = 0.0f;

void AHRSupdate(float gx, float gy, float gz,
                float ax, float ay, float az,
                float mx, float my, float mz)
{
#ifdef MADGWICK_AHRS_ALGO
  MadgwickAHRSupdate(gx*DEG_TO_RAD, gy*DEG_TO_RAD, gz*DEG_TO_RAD, ax, ay, az, mx, my, mz);
#else
  MahonyAHRSupdate(gx*DEG_TO_RAD, gy*DEG_TO_RAD, gz*DEG_TO_RAD, ax, ay, az, mx, my, mz);
#endif
}

void AHRSupdateIMU(float gx, float gy, float gz,
                   float ax, float ay, float az)
{
#ifdef MADGWICK_AHRS_ALGO
  MadgwickAHRSupdateIMU(gx*DEG_TO_RAD, gy*DEG_TO_RAD, gz*DEG_TO_RAD, ax, ay, az);
#else
  MahonyAHRSupdateIMU(gx*DEG_TO_RAD, gy*DEG_TO_RAD, gz*DEG_TO_RAD, ax, ay, az);
#endif
}

void QuaternionUpdate(float gx, float gy, float gz,
                      float ax, float ay, float az,
                      float mx, float my, float mz)
{
  float time_now = (float)millis();
  deltat = time_now - time_previous;
  time_previous = time_now;

#ifdef MADGWICK_AHRS_ALGO
  MadgwickQuaternionUpdate(ax, ay, az, gx*DEG_TO_RAD, gy*DEG_TO_RAD, gz*DEG_TO_RAD, mx, my, mz, deltat);
#else
  MahonyQuaternionUpdate(ax, ay, az, gx*DEG_TO_RAD, gy*DEG_TO_RAD, gz*DEG_TO_RAD, mx, my, mz, deltat);
#endif
}

int EulerUpdate(float* roll, float* pitch, float* yaw)
{
  float r = 0.0f;
  float p = 0.0f;
  float y = 0.0f;

  float pQ[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  // Serial print and/or display at 0.5 s rate independent of data rates
  float time_now = (float)millis();
  deltat2 = time_now - time_previous2;

  // update LCD once per half-second independent of read rate
  if (deltat2 > 500)
  {
      // Define output variables from updated quaternion---these are Tait-Bryan
      // angles, commonly used in aircraft orientation. In this coordinate system,
      // the positive z-axis is down toward Earth. Yaw is the angle between Sensor
      // x-axis and Earth magnetic North (or true North if corrected for local
      // declination, looking down on the sensor positive yaw is counterclockwise.
      // Pitch is angle between sensor x-axis and Earth ground plane, toward the
      // Earth is positive, up toward the sky is negative. Roll is angle between
      // sensor y-axis and Earth ground plane, y-axis up is positive roll. These
      // arise from the definition of the homogeneous rotation matrix constructed
      // from quaternions. Tait-Bryan angles as well as Euler angles are
      // non-commutative; that is, the get the correct orientation the rotations
      // must be applied in the correct order which for this configuration is yaw,
      // pitch, and then roll.
      // For more see
      // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
      // which has additional links.

      getQValues(pQ);

      r  = atan2(2.0f * (pQ[0] * pQ[1] + pQ[2] * pQ[3]), pQ[0] * pQ[0] - pQ[1] * pQ[1] - pQ[2] * pQ[2] + pQ[3] * pQ[3]);
      p = -asin(2.0f * (pQ[1] * pQ[3] - pQ[0] * pQ[2]));
      y   = atan2(2.0f * (pQ[1] * pQ[2] + pQ[0] * pQ[3]), pQ[0] * pQ[0] + pQ[1] * pQ[1] - pQ[2] * pQ[2] - pQ[3]* pQ[3]);
      
      *roll = r * RAD_TO_DEG;
      *pitch = p * RAD_TO_DEG;

      // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
      // 	8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
      // - http://www.ngdc.noaa.gov/geomag-web/#declination
      *yaw = (y * RAD_TO_DEG) - 8.5;      

      time_previous2 = millis();

      return 1;
  }

  return 0;
}

void getQValues(float* qParams)
{
#ifdef QUATERNION_ALGO
      getQuaternionParams(qParams);
#else
      qParams[0] = q0;
      qParams[1] = q1;
      qParams[2] = q2;
      qParams[3] = q3;
#endif
}

float getFilteredHeading(float Mx, float My, float Mz)
{
  float heading = 0.0f;
  float headingDegrees = 0.0f;
  float headingFiltered = 0.0f;
  float declination = 0.0f;

  float X = 0.0f;
  float Y = 0.0f;
  float Z = 0.0f;

  // From the datasheet: 0.92 mG/digit
  //* Earth magnetic field ranges from 0.25 to 0.65 Gauss, so these are the values that we need to get approximately.

  //---- X-Axis
  X = Mx*0.00092; // Gauss unit

  //---- Y-Axis
  Y = My*0.00092;

  //---- Z-Axis
  Z = Mz*0.00092;

  //Calculating Heading
  heading = atan2(Y, X);

  // Correcting the heading with the declination angle depending on your location
  // You can find your declination angle at: http://www.ngdc.noaa.gov/geomag-web/
  // At my location it's 4.2 degrees => 0.073 rad
  // My city = -12.44 degrees = > -0.19540706305329 rad
  //http://www.magnetic-declination.com/
  //declination = 0.073; 
  declination = -0.195; 
  heading += declination;

  // Correcting when signs are reveresed
  if (heading <0)
  {
    heading += 2*PI;
  }

  // Correcting due to the addition of the declination angle
  if (heading > 2*PI)
  {
    heading -= 2*PI;
  }

  headingDegrees = heading * 180/PI; // The heading in Degrees unit
  // Smoothing the output angle / Low pass filter 
#if 0
  headingFiltered = headingFiltered*0.85 + headingDegrees*0.15;
#else
  headingFiltered = headingDegrees;
#endif

  return headingFiltered;
}