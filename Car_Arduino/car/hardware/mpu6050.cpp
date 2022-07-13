#include "mpu6050.h"

MPU mpu_sensor;
float K1 = 0.02;
float angle, angle_dot;
float Q_angle = 0.001; // 过程噪声的协方差
float Q_gyro = 0.003; //0.003 过程噪声的协方差 过程噪声的协方差为一个一行两列矩阵
float R_angle = 0.5; // 测量噪声的协方差 既测量偏差
float dt = 0.010; //
char  C_0 = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] = {0, 0, 0, 0};
float PP[2][2] = { { 1, 0 }, { 0, 1 } };


void init_LED(void)
{
  pinMode(LED_PIN, OUTPUT);
}
void blink_LED(uint8_t cnt)
{
  for (int i = cnt; cnt > 0; cnt--)
  {
    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(500);
  }
}

void init_mpu(void)
{
  init_LED();
  Wire.beginTransmission(MPUAD);//默认I2C地址0b1101000
  Wire.write(MPUPM1);///Accessing the register 6B/107 - Power Management (Sec. 4.30)
  Wire.write(0x00);//Setting SLEEP register to 0, using the internal 8 Mhz oscillator
  Wire.endTransmission();

  Wire.beginTransmission(MPUAD); //I2C address of the MPU
  Wire.write(GYRO_CONFIG); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4)
  Wire.write(0x00); //Setting the gyro to full scale +/- 250deg./s
  Wire.endTransmission();

  Wire.beginTransmission(MPUAD); //I2C address of the MPU
  Wire.write(ACCEL_CONFIG); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5)
  Wire.write(0x00); //Setting the accel to +/- 2g（if choose +/- 16g，the value would be 0b00011000）
  Wire.endTransmission();


}

void mpu_getdata(void)
{
  Wire.beginTransmission(MPUAD); //I2C address of the MPU
  Wire.write(ACCEL_OUTRE_START); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(MPUAD, 6); //Request Accel Registers (3B - 40)

  mpu_sensor.accel.origin.x = Wire.read() << 8 | Wire.read();
  mpu_sensor.accel.origin.y = Wire.read() << 8 | Wire.read();
  mpu_sensor.accel.origin.z = Wire.read() << 8 | Wire.read();

  Wire.beginTransmission(MPUAD); //I2C address of the MPU
  Wire.write(GYRO_OUTRE_START); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(MPUAD, 6); //Request Accel Registers (3B - 40)

  mpu_sensor.gyro.origin.x = Wire.read() << 8 | Wire.read();
  mpu_sensor.gyro.origin.y = Wire.read() << 8 | Wire.read();
  mpu_sensor.gyro.origin.z = Wire.read() << 8 | Wire.read();

  mpu_sensor.accel.offset.x = mpu_sensor.accel.origin.x - mpu_sensor.accel.quiet.x;
  mpu_sensor.accel.offset.y = mpu_sensor.accel.origin.y - mpu_sensor.accel.quiet.y;
  mpu_sensor.accel.offset.z = mpu_sensor.accel.origin.y - mpu_sensor.accel.quiet.z;

  mpu_sensor.gyro.offset.x = mpu_sensor.gyro.origin.x - mpu_sensor.gyro.quiet.x;
  mpu_sensor.gyro.offset.y = mpu_sensor.gyro.origin.y - mpu_sensor.gyro.quiet.y;
  mpu_sensor.gyro.offset.z = mpu_sensor.gyro.origin.z - mpu_sensor.gyro.quiet.z;
  //Serial.print("mpu_value:");
  //Serial.print(mpu_sensor.gyro.offset.x);
  //Serial.print(",");
  //Serial.print(mpu_sensor.gyro.offset.y);
  //Serial.print(",");
  //Serial.println(mpu_sensor.gyro.offset.z);
  //Serial.print(mpu_sensor.accel.offset.x);
  //Serial.print(",");
  //Serial.print(mpu_sensor.accel.offset.y);
  //Serial.print(",");
  //Serial.println(mpu_sensor.accel.offset.z);

}

void mpu_setzero(void)
{
  uint8_t overflag = 0, cnt = 0;
  int error_sum[3] = {0, 0, 0};
  d_int16 last;
  int32_t temp_g[3] = {0, 0, 0}, temp_a[3] = {0, 0, 0};
  while (!overflag)
  {
    if (cnt < 200)
    {
      mpu_getdata();
      temp_g[0] += mpu_sensor.gyro.origin.x;
      temp_g[1] += mpu_sensor.gyro.origin.y;
      temp_g[2] += mpu_sensor.gyro.origin.z;

      temp_a[0] += mpu_sensor.accel.origin.x;
      temp_a[1] += mpu_sensor.accel.origin.y;
      temp_a[2] += mpu_sensor.accel.origin.z;

      error_sum[0] += abs(last.x - mpu_sensor.gyro.origin.x);
      error_sum[1] += abs(last.y - mpu_sensor.gyro.origin.y);
      error_sum[2] += abs(last.z - mpu_sensor.gyro.origin.z);

      last.x = mpu_sensor.gyro.origin.x;
      last.y = mpu_sensor.gyro.origin.y;
      last.z = mpu_sensor.gyro.origin.z;
    }
    else
    {
      /*校准失败*/
      if (error_sum[0]/131 >= 100 || error_sum[1]/131 >= 100 || error_sum[2]/131 >= 100)
      {
        /*
        Serial.print("error_sum:");
        Serial.print(error_sum[0]);
        Serial.print(",");
        Serial.print(error_sum[1]);
        Serial.print(",");
        Serial.println(error_sum[2]);
        */
        //Serial.println("mpu init error!");
        cnt = 0;
        blink_LED(5);
        
        for (int i = 0; i < 3; i++) temp_a[i] = temp_g[i] = error_sum[i] = 0;
      }
      /*校准成功*/
      else
      {
        mpu_sensor.accel.quiet.x = temp_a[0] / 200;
        mpu_sensor.accel.quiet.y = temp_a[1] / 200;
        mpu_sensor.accel.quiet.z = temp_a[2] / 200;

        mpu_sensor.gyro.quiet.x = temp_g[0] / 200;
        mpu_sensor.gyro.quiet.y = temp_g[1] / 200;
        mpu_sensor.gyro.quiet.z = temp_g[2] / 200;
        overflag = 1;
      }
    }
    cnt++;
  }
}

void mpu_calculation(void)
{
  float angle_x;//根据需求只需要求x轴倾角
  float accel_x = mpu_sensor.accel.offset.x / 16384;
  float accel_z = mpu_sensor.accel.offset.z / 16384;
  float gyro_x = mpu_sensor.gyro.offset.x / 131;

  angle_x = atan2(accel_x, accel_z) * 180 / 3.14;
  Kalman_Filter(angle_x,gyro_x);
  //first_order_Filter(angle_x, gyro_x);
  mpu_sensor.gyro_z = mpu_sensor.gyro.offset.z / 131;
  mpu_sensor.pitch = angle;
  Serial.print("pitch:");
  Serial.println(mpu_sensor.pitch);
}

void mpu_print(void)
{

  //Serial.print("Gyro (deg/s)");
  //Serial.print(" X=");
  //Serial.print(mpu_sensor.gyro.offset.x);
  //Serial.print(",");
  //Serial.print(mpu_sensor.gyro.offset.y);
  //Serial.print(",");
  //Serial.println(mpu_sensor.gyro.offset.z);

  //Serial.print(" Accel (g)");
  //Serial.print(" X=");
  //Serial.print(mpu_sensor.accel.offset.x);
  //Serial.print(" Y=");
  //Serial.print(mpu_sensor.accel.offset.y);
  //Serial.print(" Z=");
  //Serial.println(mpu_sensor.accel.offset.z);
}

void mpu_update(void)
{
  mpu_getdata();
  //mpu_calculation();
}
/*MPU6050 的加速度计和陀螺仪各有优缺点，三轴的加速度值没有累积误差，
  且通过算atan2() 可以得到倾角，但是它包含的噪声太多（因为待测物运动时
  会产生加速度，电机运行时振动会产生加速度等），不能直接使用；陀螺仪对
  外界振动影响小，精度高，通过对角速度积分可以得到倾角，但是会产生累积
  误差。所以，不能单独使用 MPU6050 的加速度计或陀螺仪来得到倾角，需要
  互补。一阶互补算法的思想就是给加速度和陀螺仪不同的权值，把它们结合到
  一起，进行修正。*/

void first_order_Filter(float angle_m, float gyro_m)
{
  angle = K1 * angle_m + (1 - K1) * (angle + gyro_m * dt);
}
void Kalman_Filter(float Accel, float Gyro)
{
  angle += (Gyro - Q_bias) * dt; //先验估计
  Pdot[0] = Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分

  Pdot[1] = -PP[1][1];
  Pdot[2] = -PP[1][1];
  Pdot[3] = Q_gyro;
  PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
  PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
  PP[1][0] += Pdot[2] * dt;
  PP[1][1] += Pdot[3] * dt;

  Angle_err = Accel - angle;	//zk-先验估计

  PCt_0 = C_0 * PP[0][0];
  PCt_1 = C_0 * PP[1][0];

  E = R_angle + C_0 * PCt_0;

  K_0 = PCt_0 / E;
  K_1 = PCt_1 / E;

  t_0 = PCt_0;
  t_1 = C_0 * PP[0][1];

  PP[0][0] -= K_0 * t_0;		 //后验估计误差协方差
  PP[0][1] -= K_0 * t_1;
  PP[1][0] -= K_1 * t_0;
  PP[1][1] -= K_1 * t_1;

  angle	+= K_0 * Angle_err;	 //后验估计
  Q_bias	+= K_1 * Angle_err;	 //后验估计
  angle_dot   = Gyro - Q_bias;	 //输出值(后验估计)的微分=角速度
}
