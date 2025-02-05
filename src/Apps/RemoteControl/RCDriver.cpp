#include "RCDriver.h"
#include "WiFi.h"
#include <netinet/in.h>
#include <unistd.h>
#include <Wheelson.h>

RCDriver::RCDriver(Connection* connection) : con(connection), feedClient(connection->getFeedClient()), controlClient(connection->getControlClient()), feedTask("RC-Feed", RCDriver::feedTaskFunc, 2048, this){
	Camera::initialize(true, con->getInfo().quality);
	LoopManager::addListener(this);
	feedTask.start(1, 0);
}

RCDriver::~RCDriver(){
	LoopManager::removeListener(this);
	feedTask.stop(true);
	Camera::initialize(false);
}

#define FRAME_LEN 8

const uint8_t frameStart[FRAME_LEN] = {0x18, 0x20, 0x55, 0xf2, 0x5a, 0xc0, 0x4d, 0xaa};
const uint8_t frameEnd[FRAME_LEN] = {0x42, 0x2c, 0xd9, 0xe3, 0xff, 0xa0, 0x11, 0x01};

bool RCDriver::send(WiFiClient& client, const uint8_t* buffer, size_t size){
	if(!client.connected()) return false;

	size_t sent = 0;
	while(sent < size){
		if(!client.connected()) return false;

		size_t psent = client.write(buffer + sent, min((size_t) 256, size - sent));
		if(psent <= 0){
			if(client.getWriteError() != 0) return false;

			delayMicroseconds(500);
		}else{
			sent += psent;
		}
		yield();
	}
	return true;
}

bool RCDriver::feedFunc(){
	RCDriver* driver = this;
	WiFiClient& client = driver->feedClient;

	if(!client.connected()){
		return false;
	}

	uint8_t level = Battery.getLevel();

	driver->cam.loadFrame();

	uint8_t* frame = driver->cam.getFrame()->buf;
	uint32_t frameSize = driver->cam.getFrame()->len;

	if(!driver->send(client, frameStart, FRAME_LEN)){
		printf("Frame start send error\n");
		goto fail;
	}

	if(!driver->send(client, reinterpret_cast<const uint8_t*>(&frameSize), sizeof(frameSize))){
		printf("Frame size send error\n");
		goto fail;
	}

	if(!driver->send(client, frame, frameSize)){
		printf("Frame send error\n");
		goto fail;
	}

	if(!driver->send(client, frameEnd, FRAME_LEN)){
		printf("Frame end send error\n");
		goto fail;
	}

	if(!driver->send(client, &level, 1)){
		printf("Battery level send error\n");
		goto fail;
	}
	goto success;

	fail:
	driver->cam.releaseFrame();
	return false;

	success:
	driver->cam.releaseFrame();
	return true;
}

void RCDriver::feedTaskFunc(Task* task){
	RCDriver* driver = static_cast<RCDriver*>(task->arg);

	while(task->running){
		if(!driver->feedFunc()) return;
	}
}

void RCDriver::loop(uint){
	/*if(!feedFunc()){
		LoopManager::removeListener(this);
		return;
	}*/

	WiFiClient& client = controlClient;
	if(!client.connected()){
		con->disconnected();
		LoopManager::removeListener(this);
		return;
	}

	if(client.read(&command, 1) != 1) return;

	if(command & 0b10000){
		LED.setHeadlight(!LED.getHeadlight());
	}

#define FW (command & 0b0001)
#define BW (command & 0b0010)
#define L (command & 0b0100)
#define R (command & 0b1000)
#define MECHANIM (command & 0b100000)

#define ML(x) do { Motors.setMotor(MOTOR_FL, x); Motors.setMotor(MOTOR_BL, x); } while(0)
#define MR(x) do { Motors.setMotor(MOTOR_FR, x); Motors.setMotor(MOTOR_BR, x); } while(0)

#define MFL(x) do { Motors.setMotor(MOTOR_FL, x); } while(0)
#define MFR(x) do { Motors.setMotor(MOTOR_FR, x); } while(0)
#define MBL(x) do { Motors.setMotor(MOTOR_BL, x); } while(0)
#define MBR(x) do { Motors.setMotor(MOTOR_BR, x); } while(0)

	if (MECHANIM) {
		if (FW && L) {
			//diagonal up left
			MFL(0);
			MFR(127);
			MBL(127);
			MBR(0);
		} else if (FW && R) {
			//diagonal up right
			MFL(127);
			MFR(0);
			MBL(0);
			MBR(127);
		} else if (FW && BW) {
			//rotate on the spot right
			ML(127);
			MR(-127);
		} else if (BW && L) {
			//diagonal back left
			MFL(-127);
			MFR(0);
			MBL(0);
			MBR(-127);
		} else if (BW && R) {
			//diagonal back right
			MFL(0);
			MFR(-127);
			MBL(-127);
			MBR(0);
		} else if (L && R) {
			//rotate on the spot left
			ML(-127);
			MR(127);
		} else if (FW) {
			ML(127);
			MR(127);
		} else if (BW) {
			ML(-127);
			MR(-127);
		} else if (L) {
			MFL(-127);
			MFR(127);
			MBL(127);
			MBR(-127);
		} else if (R) {
			MFL(127);
			MFR(-127);
			MBL(-127);
			MBR(127);
		} else {
			ML(0);
			MR(0);
		}
	} else {
		if(FW && L){
			ML(10);
			MR(127);
		}else if(FW && R){
			ML(127);
			MR(10);
		}else if(BW && L){
			ML(-10);
			MR(-127);
		}else if(BW && R){
			ML(-127);
			MR(-10);
		}else if(FW){
			ML(127);
			MR(127);
		}else if(BW){
			ML(-127);
			MR(-127);
		}else if(L){
			ML(-127);
			MR(127);
		}else if(R){
			ML(127);
			MR(-127);
		}else{
			ML(0);
			MR(0);
		}
	}
}
