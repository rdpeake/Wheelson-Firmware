#include <Loop/LoopManager.h>
#include <Wheelson.h>
#include "MotorHWTest.h"

MotorHWTest::MotorHWTest(UserHWTest* userHwTest) : HWTestPart(userHwTest){

}

MotorHWTest::~MotorHWTest(){

}

void MotorHWTest::draw(){
	FontWriter u8f = userHwTest->getScreen().getSprite()->startU8g2Fonts();
	u8f.setFont(u8g2_font_profont12_tf);
	u8f.setForegroundColor(TFT_WHITE);
	u8f.setFontMode(1);
	u8f.setCursor((160 - u8f.getUTF8Width("Motor test")) / 2, 8);
	u8f.print("Motor test");
	u8f.setForegroundColor(TFT_RED);
	u8f.setCursor((160 - u8f.getUTF8Width("Grab your Wheelson!")) / 2, 20);
	u8f.print("Grab your Wheelson!");
	u8f.setForegroundColor(TFT_GREEN);
	u8f.setCursor((160 - u8f.getUTF8Width("Motors should go forward")) / 2, 32);
	u8f.print("Motors should go forward");
	u8f.setForegroundColor(TFT_WHITE);
	u8f.setCursor(5, 47);
	u8f.print("Hold UP for FR motor");
	u8f.setCursor(5, 62);
	u8f.print("Hold RIGHT for BR motor");
	u8f.setCursor(5, 77);
	u8f.print("Hold DOWN for BL motor");
	u8f.setCursor(5, 92);
	u8f.print("Hold LEFT for FL motor");
	u8f.setCursor(5, 107);
	u8f.print("Hold MID for all motors");
	u8f.setCursor(5, 125);
	u8f.print("If they work,press BACK!");
}

void MotorHWTest::start(){
	LoopManager::addListener(this);
	Input::getInstance()->addListener(this);

	userHwTest->draw();
	userHwTest->getScreen().commit();
}

void MotorHWTest::stop(){
	LoopManager::removeListener(this);
	Input::getInstance()->removeListener(this);
	Motors.setMotor(MOTOR_FR, 0);
	Motors.setMotor(MOTOR_BR, 0);
	Motors.setMotor(MOTOR_FL, 0);
	Motors.setMotor(MOTOR_BL, 0);
}

void MotorHWTest::buttonPressed(uint id){
	switch(id){
		case BTN_UP:
			Motors.setMotor(MOTOR_FR, 127);
			break;
		case BTN_DOWN:
			Motors.setMotor(MOTOR_BL, 127);
			break;
		case BTN_LEFT:
			Motors.setMotor(MOTOR_FL, 127);
			break;
		case BTN_RIGHT:
			Motors.setMotor(MOTOR_BR, 127);
			break;
		case BTN_MID:
			Motors.setMotor(MOTOR_FR, 127);
			Motors.setMotor(MOTOR_BL, 127);
			Motors.setMotor(MOTOR_FL, 127);
			Motors.setMotor(MOTOR_BR, 127);
			break;
		case BTN_BACK:
			userHwTest->currentTestDone();
			break;
	}

}

void MotorHWTest::buttonReleased(uint i){
	switch(i){
		case BTN_UP:
			Motors.setMotor(MOTOR_FR, 0);
			break;
		case BTN_DOWN:
			Motors.setMotor(MOTOR_BL, 0);
			break;
		case BTN_LEFT:
			Motors.setMotor(MOTOR_FL, 0);
			break;
		case BTN_RIGHT:
			Motors.setMotor(MOTOR_BR, 0);
			break;
		case BTN_MID:
			Motors.setMotor(MOTOR_FR, 0);
			Motors.setMotor(MOTOR_BL, 0);
			Motors.setMotor(MOTOR_FL, 0);
			Motors.setMotor(MOTOR_BR, 0);
			break;
		case BTN_BACK:
			userHwTest->currentTestDone();
			break;
	}
}

void MotorHWTest::loop(uint micros){

}


