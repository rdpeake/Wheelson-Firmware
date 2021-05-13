#include <Arduino.h>
#include <CircuitOS.h>
#include <Wheelson.h>
#include <Nuvoton/Nuvoton.h>
#include <Nuvoton/WheelsonInput.h>
#include <Loop/LoopManager.h>
#include <Display/Display.h>
#include <SPIFFS.h>
#include <esp32-hal-psram.h>
#include "src/IntroScreen.h"
#include "src/Apps/Timeline/Timeline.h"
#include "src/Apps/Timeline/TimelineApp.h"
#include "src/Apps/MainMenu/MainMenu.h"
#include "src/Components/MotorControl.h"
#include "src/Components/ActionProcessor.h"
#include "src/Components/CameraFeed.h"
#include "src/Apps/AutonomousDriving/autonomousSettings.h"
#include "src/Apps/AutonomousDriving/autonomousApp.h"
#include <Util/Task.h>
#include <esp_wifi_types.h>
#include <esp_wifi.h>
#include <WiFi.h>


Display display(160, 128, -1, -1);

void setup(){
	Serial.begin(115200);

	WiFi.mode(WIFI_OFF);
	btStop();

	disableCore0WDT();
	disableCore1WDT();

	if(psramFound()){
		Serial.printf("PSRAM init: %s, free: %d B\n", psramInit() ? "Yes" : "No", ESP.getFreePsram());
	}else{
		Serial.println("No PSRAM");
	}

	if(!SPIFFS.begin()){
		Serial.println("SPIFFS error");
	}

	if(!Nuvo.begin()){
		Serial.println("Nuvoton error");
		//for(;;);
	}

/*	CameraFeed cam(120, 160);
	display.begin();
	display.getBaseSprite()->clear(TFT_GOLD);
	display.commit();
	for(;;){
		cam.loadFrame();
		display.getBaseSprite()->drawIcon(cam.getRaw(), 0, 0, 160, 120);
		display.commit();
		for(int i = 0; i < 4; i++){
			Motors.setMotor(i, random(255) - 128);
			delay(10);
		}
	}*/

	Settings::init(new SettingsStruct, sizeof(SettingsStruct));

	WheelsonInput* input = new WheelsonInput();
	input->preregisterButtons({ 0, 1, 2, 3, 4, 5 });
	LoopManager::addListener(input);

	new MotorControl(&input->getMut());
	new ActionProcessor();

	LED.setHeadlight(false);

	display.begin();
/*	LED.setBacklight(false);
	CameraFeed cam(160, 128);
	for(;;){
		cam.loadFrame();
		display.getBaseSprite()->drawIcon(cam.getRaw(), 0, 0, 160, 128);
		display.commit();

		delay(100);
	}*/

	Context* intro = new MainMenu(display);
	intro->unpack();
	intro->start();

	LED.setBacklight(true);
	LED.setHeadlight(true);

	Task::setPinned(true);
	LoopManager::startTask(1, 1);

	vTaskDelete(NULL);
}

void loop(){
	LoopManager::loop();
}