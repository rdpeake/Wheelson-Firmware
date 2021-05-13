#ifndef AUTOCAR_MOTORS_H
#define AUTOCAR_MOTORS_H

#include <Arduino.h>
#include <Input/I2cExpander.h>
#include <Sync/Mutex.h>
#define MOTOR_FORWARD 0
#define MOTOR_BACKWARD 1
#define MOTOR_STOP 2
// TODO: enum

class MotorControl
{
public:
	MotorControl(Mutex* _mutex = nullptr);
	void leftMotor(uint8_t action);
	void rightMotor(uint8_t action);
	void stopAll();
	static MotorControl* getInstance();
	Mutex* mutex;
private:
	static MotorControl* instance;
};

#endif