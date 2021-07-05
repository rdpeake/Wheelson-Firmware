#include <BallTracker.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "BallDriver.h"
#include <Wheelson.h>

using namespace cv;

void BallDriver::process(){
	const Color* frameData = getCameraImage();

	std::vector<Ball> balls = BallTracker::detect((uint8_t*) getCameraImage888(), 160, 120, BallTracker::RGB888, displayMode == BW ? processedBuffer : nullptr);

	Ball* bestBall = nullptr;
	float maxFitness = 0;

	if(displayMode == RAW && balls.empty()){
		memcpy(processedBuffer, getCameraImage(), 160 * 120 * 2);
	}else{
		Mat draw(120, 160, CV_8UC2);
		memcpy(draw.data, getCameraImage(), 160 * 120 * 2);



		for(auto &ball : balls){
			if(ball.fitness > maxFitness){
				maxFitness = ball.fitness;
				bestBall = &ball;
			}
			if(displayMode == RAW){
				cv::circle(draw, ball.center, ball.radius, Scalar(255, 0, 255));
			}
		}
		if(displayMode == RAW){
			memcpy(processedBuffer, draw.data, 160 * 120 * 2);
		}
	}

	if(bestBall == nullptr || maxFitness == 0){
		setMotor(MOTOR_FL, 0);
		setMotor(MOTOR_BL, 0);
		setMotor(MOTOR_FR, 0);
		setMotor(MOTOR_BR, 0);
		return;
	}


	float amt = abs(80.0 - (float)bestBall->center.x) / 80.0;
	float amtR, amtL;
	if(amt <= 0.1){
		amtL = 1;
		amtR = 1;
	}else{
		if(bestBall->center.x < 80){
			amtR = 30.0f * amt + 1.0f;
			amtL = 0;
		}else{
			amtR = 0;
			amtL = 30.0f * amt + 1.0f;
		}
	}

	setMotor(MOTOR_FL, (int8_t)amtL);
	setMotor(MOTOR_BL, (int8_t)amtL);
	setMotor(MOTOR_FR, (int8_t)amtR);
	setMotor(MOTOR_BR, (int8_t)amtR);

}

void BallDriver::toggleDisplayMode(){
	displayMode = static_cast<DisplayMode>((displayMode+1) % DisplayMode::COUNT);
}
