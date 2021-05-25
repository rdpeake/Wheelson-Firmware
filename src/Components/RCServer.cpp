#include <WiFi.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <Wheelson.h>
#include "RCServer.h"
#include "CameraFeed.h"

const uint8_t header[8] = { 0xb8, 0x12, 0x55, 0x45, 0xf8, 0xe1, 0x72, 0xd0 };

void RCServer::start(){
	printf("Connecting WiFi...\n");
	WiFi.begin("CircuitMess", "MAKERphone!");
	while(WiFi.status() != WL_CONNECTED && WiFi.status() != WL_CONNECT_FAILED) delayMicroseconds(1);
	if(WiFi.status() == WL_CONNECT_FAILED){
		printf("Connection failed\n");
		for(;;);
	}else{
		printf("Connected, IP address %s\n", WiFi.localIP().toString().c_str());
	}

	int sockFeed = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFeed < 0){
		printf("Feed socket failed: %d\n", sockFeed);
		return;
	}

	sockaddr_in servaddrFeed;
	bzero(&servaddrFeed, sizeof(servaddrFeed));
	servaddrFeed.sin_family = AF_INET;
	inet_aton("192.168.0.159", &servaddrFeed.sin_addr);
	servaddrFeed.sin_port = htons(6582);

	printf("Connecting stream...\n");
	if(connect(sockFeed, reinterpret_cast<const sockaddr*>(&servaddrFeed), sizeof(servaddrFeed)) == 0){
		printf("Connected\n");
	}else{
		printf("Error %d\n", errno);
		return;
	}

	int sockCtrl = socket(AF_INET, SOCK_STREAM, 0);
	if(sockCtrl < 0){
		printf("Feed socket failed: %d\n", sockCtrl);
		return;
	}

	sockaddr_in servaddrCtrl;
	bzero(&servaddrCtrl, sizeof(servaddrCtrl));
	servaddrCtrl.sin_family = AF_INET;
	inet_aton("192.168.0.159", &servaddrCtrl.sin_addr);
	servaddrCtrl.sin_port = htons(6583);

	printf("Connecting control...\n");
	if(connect(sockCtrl, reinterpret_cast<const sockaddr*>(&servaddrCtrl), sizeof(servaddrCtrl)) == 0){
		printf("Connected\n");
	}else{
		printf("Error %d\n", errno);
		return;
	}

	CameraFeed cam;

	int i = 0;
	for(;;){
		cam.loadFrame();
		camera_fb_t* frame = cam.getFrame();

		// printf("Frame size %d\n", frame->len);

		send(sockFeed, frame->buf, frame->len, MSG_DONTWAIT);
		send(sockFeed, header, sizeof(header), MSG_DONTWAIT);

		cam.releaseFrame();

		/*display.getBaseSprite()->drawIcon(cam.getRaw(), 0, 0, 160, 120);
		display.commit();*/

		uint8_t command;
		uint8_t readb = recv(sockCtrl, &command, 1, MSG_DONTWAIT);
		if(readb == 1){
			printf("got 0x%x\n", command);

			switch(command){
				case 0:
					Motors.setMotor(0, 0);
					Motors.setMotor(1, 0);
					Motors.setMotor(2, 0);
					Motors.setMotor(3, 0);
					break;
				case 0b0001:
					Motors.setMotor(0, 100);
					Motors.setMotor(1, 100);
					Motors.setMotor(2, 100);
					Motors.setMotor(3, 100);
					break;
				case 0b0011:
					Motors.setMotor(0, 5);
					Motors.setMotor(1, 5);
					Motors.setMotor(2, 120);
					Motors.setMotor(3, 120);
					break;
				case 0b1001:
					Motors.setMotor(0, 120);
					Motors.setMotor(1, 120);
					Motors.setMotor(2, 5);
					Motors.setMotor(3, 5);
					break;
				case 0b1000:
					Motors.setMotor(0, 100);
					Motors.setMotor(1, 100);
					Motors.setMotor(2, -100);
					Motors.setMotor(3, -100);
					break;
				case 0b0010:
					Motors.setMotor(0, -100);
					Motors.setMotor(1, -100);
					Motors.setMotor(2, 100);
					Motors.setMotor(3, 100);
					break;
				case 0b0100:
					Motors.setMotor(0, -100);
					Motors.setMotor(1, -100);
					Motors.setMotor(2, -100);
					Motors.setMotor(3, -100);
					break;
			}
		}
	}
}

void RCServer::stop(){

}

RCServer::RCServer(Display& display) : display(display){ }
