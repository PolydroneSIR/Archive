CC = g++
NAVIO = ../../Navio
INCLUDES = -I ../..

all:
	$(CC) $(INCLUDES) ServoMatt.cpp $(NAVIO)/MS5611.cpp $(NAVIO)/MPU9250.cpp $(NAVIO)/PCA9685.cpp $(NAVIO)/I2Cdev.cpp $(NAVIO)/gpio.cpp -o ServoMatt

clean:
	rm ServoMatt
