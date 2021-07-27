#ifndef WHEELSON_FIRMWARE_RCDRIVER_H
#define WHEELSON_FIRMWARE_RCDRIVER_H

#include "../Autonomous/Driver.h"
#include <Loop/LoopManager.h>
#include <Display/Display.h>

#define CAMERA_FEED_PORT 5556
#define CONTROL_PORT 5557

#define FRAME_LENGTH 160*120

enum {
	FORWARD, BACKWARD, LEFT, RIGHT
};

class RCDriver : public Driver, public LoopListener{

public:

	RCDriver(Display* _display);

	void loop(uint) override;

	void draw() override;

	bool isConnected();

	void startFeedSocket();
	void startControlSocket();
	void stopFeedSocket();
	void stopControlSocket();

protected:
	void process() override;

private:

	Camera cam;
	Display* display;

	bool connected = false;

	const char* accessPointIP = "10.0.0.4";

	int feedSock = -1;
	int controlSock = -1;
	bool feedRunning = false;
	bool controlRunning = false;

	uint16_t command;

	const uint8_t feedBegin = 0x44;
	const uint8_t feedEnd = 0xCC;

	bool testVar1 = false;
	bool testVar2 = false;

};


#endif //WHEELSON_FIRMWARE_RCDRIVER_H
