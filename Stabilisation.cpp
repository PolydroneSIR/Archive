/*
./Servo
*/

/*
Pin 1 -> NAVIO_RCOUTPUT_1 3 -> Allume le moteur sur la Pin 1 de la Navio
Pin 2 -> NAVIO_RCOUTPUT_1 4 -> Allume le moteur sur la Pin 2 de la Navio
Pin 3 -> NAVIO_RCOUTPUT_1 5 -> Allume le moteur sur la Pin 3 de la Navio
Pin 4 -> NAVIO_RCOUTPUT_1 6 -> Allume le moteur sur la Pin 4 de la Navio

*/

#define NAVIO_RCOUTPUT_1 3
#define NAVIO_RCOUTPUT_2 4
#define NAVIO_RCOUTPUT_3 5
#define NAVIO_RCOUTPUT_4 6
#define SERVO_MIN 1.250 /*mS*/
#define SERVO_MAX 1.750 /*mS*/
#define A 0.025
#define B 0.025

#include <Navio/gpio.h>
#include "Navio/PCA9685.h"
#include "Navio/MPU9250.h"
#include "Navio/MS5611.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "AHRS.hpp"

using namespace Navio;



//--------------------------------------------------fonctions de l'AHRS --------------//

// Objects

MPU9250 imu;    // MPU9250
AHRS    ahrs;   // Mahony AHRS

// Sensor data

float ax, ay, az;
float gx, gy, gz;
float mx, my, mz;

// Orientation data

float roll, pitch, yaw;

// Timing data// Objects

// Timing data

float offset[3];
struct timeval tv;
float dt, maxdt;
float mindt = 0.01;
unsigned long previoustime, currenttime;
float dtsumm = 0;
int isFirst = 1;

// Network data

int sockfd;
struct sockaddr_in servaddr = {0};
char sendline[80];

//============================= Initial setup =================================

void imuSetup()
{
    //----------------------- MPU initialization ------------------------------

    imu.initialize();

    //-------------------------------------------------------------------------

	printf("Beginning Gyro calibration...\n");
	for(int i = 0; i<100; i++)
	{
		imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
		offset[0] += (-gx*0.0175);
		offset[1] += (-gy*0.0175);
		offset[2] += (-gz*0.0175);
		usleep(10000);
	}
	offset[0]/=100.0;
	offset[1]/=100.0;
	offset[2]/=100.0;

	printf("Offsets are: %f %f %f\n", offset[0], offset[1], offset[2]);
	ahrs.setGyroOffset(offset[0], offset[1], offset[2]);
}

//============================== Main loop ====================================

void imuLoop()
{
    //----------------------- Calculate delta time ----------------------------

	gettimeofday(&tv,NULL);
	previoustime = currenttime;
	currenttime = 1000000 * tv.tv_sec + tv.tv_usec;
	dt = (currenttime - previoustime) / 1000000.0;
	if(dt < 1/1300.0) usleep((1/1300.0-dt)*1000000);
        gettimeofday(&tv,NULL);
        currenttime = 1000000 * tv.tv_sec + tv.tv_usec;
	dt = (currenttime - previoustime) / 1000000.0;

    //-------- Read raw measurements from the MPU and update AHRS --------------

    // Accel + gyro.
    imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    ahrs.updateIMU(ax, ay, az, gx*0.0175, gy*0.0175, gz*0.0175, dt);

    // Accel + gyro + mag. 
    // Soft and hard iron calibration required for proper function.
    /*
    imu.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
    ahrs.update(ax, ay, az, gx*0.0175, gy*0.0175, gz*0.0175, my, mx, -mz, dt);
    */

    //------------------------ Read Euler angles ------------------------------

    ahrs.getEuler(&roll, &pitch, &yaw);

    //------------------- Discard the time of the first cycle -----------------

    if (!isFirst)
    {
    	if (dt > maxdt) maxdt = dt;
    	if (dt < mindt) mindt = dt;
    }
    isFirst = 0;

    //------------- Console and network output with a lowered rate ------------

    dtsumm += dt;
    if(dtsumm > 0.05)
    {
        // Console output
        printf("ROLL: %+05.2f PITCH: %+05.2f YAW: %+05.2f\n", roll, pitch, yaw * -1);

        // Network output
        sprintf(sendline,"%10f %10f %10f %10f %dHz\n", ahrs.getW(), ahrs.getX(), ahrs.getY(), ahrs.getZ(), int(1/dt));
        sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

        dtsumm = 0;
    }
}







int main()
{

// Permet d'initialiser la pin 27 de la RPI en mode Output
    static const uint8_t outputEnablePin = RPI_GPIO_27;

    Pin pin(outputEnablePin);

    if (pin.init()) {
        pin.setMode(Pin::GpioModeOutput);
        pin.write(0); /* drive Output Enable low */
    } else {
        fprintf(stderr, "Output Enable not set. Are you root?");
    }
//------------------------------------------------------------
// Initialisation de la bibliothèque PCA9685 avec l'objet "pwm"
    PCA9685 pwm;

// Initialisation de la connection RPI / NAVIO
    pwm.initialize();
    pwm.setFrequency(50);

float pitchReel=0;
float rollReel=0;
float ValPitchMot1;
float ValPitchMot2;
float ValPitchMot3;
float ValPitchMot4;

float ValRollMot1;
float ValRollMot2;
float ValRollMot3;
float ValRollMot4;

float ValMot1;
float ValMot2;
float ValMot3;
float ValMot4;

imuSetup();
imuLoop();
int t=0;
int i=0;

/*	pwm.setPWMmS(NAVIO_RCOUTPUT_1, SERVO_MIN);
        pwm.setPWMmS(NAVIO_RCOUTPUT_2, SERVO_MIN);
        pwm.setPWMmS(NAVIO_RCOUTPUT_3, SERVO_MIN);
        pwm.setPWMmS(NAVIO_RCOUTPUT_4, SERVO_MIN);

  /*      for(i=0;i<0.25;i+=0.025)
        {
                pwm.setPWMmS(NAVIO_RCOUTPUT_1, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_2, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_3, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_4, (SERVO_MIN+i));
                sleep(2); // Valeur en seconde

        }i;
*/

// Boucle d'exemple
    while (true) 
    {	
/*	pwm.setPWMmS(NAVIO_RCOUTPUT_1, SERVO_MIN);
        pwm.setPWMmS(NAVIO_RCOUTPUT_2, SERVO_MIN);
        pwm.setPWMmS(NAVIO_RCOUTPUT_3, SERVO_MIN);
        pwm.setPWMmS(NAVIO_RCOUTPUT_4, SERVO_MIN);

    for(i=0;i<0.5;i+=0.025)
        {
                pwm.setPWMmS(NAVIO_RCOUTPUT_1, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_2, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_3, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_4, (SERVO_MIN+i));
                sleep(0.5); // Valeur en seconde

        }i;
*/

	pitchReel = pitch * 90/85;	
	imuLoop();
	ValPitchMot1 = (- A) * pitchReel;
	ValPitchMot2 = (- A) * pitchReel;
	ValPitchMot3 = A * pitchReel;
	ValPitchMot4 = A * pitchReel;
	
	rollReel = roll ;
	ValRollMot1 = (-B) * rollReel;
	ValRollMot2 = B * rollReel;
	ValRollMot3 = B * rollReel;
	ValRollMot4 = (-B) * rollReel;

	ValMot1 = 1.2515 + ValPitchMot1 + ValRollMot1;
	ValMot2 = 1.2515 + ValPitchMot2 + ValRollMot2;
	ValMot3 = 1.2515 + ValPitchMot3 + ValRollMot3;
	ValMot4 = 1.2515 + ValPitchMot4 + ValRollMot4;

	//printf("\n%f\n", ValMot1);
	
	if(t>5000)
{
	pwm.setPWMmS(NAVIO_RCOUTPUT_1, (ValMot1));
	pwm.setPWMmS(NAVIO_RCOUTPUT_2, (ValMot2));
	pwm.setPWMmS(NAVIO_RCOUTPUT_3, (ValMot3));
	pwm.setPWMmS(NAVIO_RCOUTPUT_4, (ValMot4));
	};
	t=t+1;
	
	sleep(0.3);

	
	
			








	/*for(i=0;i<0.2;i+=0.0025)
	{
		pwm.setPWMmS(NAVIO_RCOUTPUT_1, (SERVO_MIN+i));
	                pwm.setPWMmS(NAVIO_RCOUTPUT_1, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_2, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_3, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_4, (SERVO_MIN+i));
                sleep(1.5); // Valeur en seconde
                pwm.setPWMmS(NAVIO_RCOUTPUT_1, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_2, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_3, (SERVO_MIN+i));
                pwm.setPWMmS(NAVIO_RCOUTPUT_4, (SERVO_MIN+i));
                sleep(1.5); // Valeur en seconde
	pwm.setPWMmS(NAVIO_RCOUTPUT_2, (SERVO_MIN+i));		
		pwm.setPWMmS(NAVIO_RCOUTPUT_3, (SERVO_MIN+i));
		pwm.setPWMmS(NAVIO_RCOUTPUT_4, (SERVO_MIN+i));
		sleep(1.5); // Valeur en seconde       

	}i;	*/

    }
    return 0;
}
