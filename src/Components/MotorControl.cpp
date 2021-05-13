#include <Wheelson.h>
#include "MotorControl.h"
MotorControl* MotorControl::instance = nullptr;

//Motors start
//------------------------------------------------------
MotorControl::MotorControl(Mutex* _mutex) : mutex(_mutex)
{
	instance = this;
}

void MotorControl::leftMotor(uint8_t action)
{
	mutex->lock();
	switch(action){
		case MOTOR_FORWARD:
			Motors.setMotor(MOTOR_FL, 30);
			Motors.setMotor(MOTOR_BL, 30);
			break;
		case MOTOR_BACKWARD:
			Motors.setMotor(MOTOR_FL, -30);
			Motors.setMotor(MOTOR_BL, -30);
			break;
		case MOTOR_STOP:
			Motors.setMotor(MOTOR_FL, 0);
			Motors.setMotor(MOTOR_BL, 0);
			break;
	}
	mutex->unlock();
}
void MotorControl::rightMotor(uint8_t action)
{
	mutex->lock();
	switch(action){
		case MOTOR_FORWARD:
			Motors.setMotor(MOTOR_FR, 30);
			Motors.setMotor(MOTOR_BR, 30);
			break;
		case MOTOR_BACKWARD:
			Motors.setMotor(MOTOR_FR, -30);
			Motors.setMotor(MOTOR_BR, -30);
			break;
		case MOTOR_STOP:
			Motors.setMotor(MOTOR_FR, 0);
			Motors.setMotor(MOTOR_BR, 0);
			break;
	}
	mutex->unlock();
}

void MotorControl::stopAll()
{
	leftMotor(MOTOR_STOP);
	rightMotor(MOTOR_STOP);
}
MotorControl* MotorControl::getInstance()
{
	return instance;
}

//Motors end
//------------------------------------------------------
//Leds start
