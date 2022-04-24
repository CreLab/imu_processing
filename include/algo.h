#ifndef __ALGO_H
#define __ALGO_H

void AHRSupdate(float gx, float gy, float gz,
                float ax, float ay, float az,
                float mx, float my, float mz);

void AHRSupdateIMU(float gx, float gy, float gz,
                   float ax, float ay, float az);

void QuaternionUpdate(float gx, float gy, float gz,
                      float ax, float ay, float az,                      
                      float mx, float my, float mz);

int EulerUpdate(float* roll, float* pitch, float* yaw);

void getQValues(float* qParams);

float getFilteredHeading(float Mx, float My, float Mz);

#endif /* __ALGO_H */