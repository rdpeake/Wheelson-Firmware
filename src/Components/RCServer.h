#ifndef WHEELSON_FIRMWARE_RCSERVER_H
#define WHEELSON_FIRMWARE_RCSERVER_H


#include <Display/Display.h>

class RCServer {
public:
	RCServer(Display& display);

	void start();
	void stop();

private:
	Display& display;

};


#endif //WHEELSON_FIRMWARE_RCSERVER_H
