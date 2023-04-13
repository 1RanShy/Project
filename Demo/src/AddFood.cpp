#include "AddFood.h"
#include "SR04.h"
#include "Servo.h"
#include "globalVar.h"
#include "Pump.h"
#include <iostream>
#include <thread>
using namespace std;

/*
	Automatically detect whether the food has decreased, and if so, refill the food
	Detect frequency : 1 time/s
	The timer will call this function.
*/
void timerCallback1()
{
	if(autoAddFoodFlag == 1)
	{
		SR04 foodDtect(23,24);
		Servo addFood(16);
		SR04 waterDetect(8,7);
		PumpControl pump(12);
		pump.stop();
		/*
			After detection and experimentation:

			The strong electric current (about 100mA) generated by the water pump during operation interferes with the normal operation of the circuit, causing instability in the operation of SR04 and resulting in unstable distance measurement.

			Solution:

			Turn off the water pump before turning on SR04 each time.
		*/
		float foodLeft = foodDtect.get_distance();
		cout << foodLeft << "cm" << endl;
		if(foodLeft < 4)
		{
			addFood.setAngle(0);
			//食物多停止加东西
		}

		if(foodLeft > 10)
		{
			addFood.setAngle(180);
			//食物少加东西
		}


		float waterLeft = waterDetect.get_distance();
		cout << waterLeft << "cm" << endl;
		if(waterLeft < 4)
		{
			pump.stop();
		}

		if(waterLeft > 10)
		{
			pump.start();
		}


	}
	else
	{
		SR04 foodDtect(23,24);
		Servo addFood(16);
		SR04 waterDetect(8,7);
		PumpControl pump(12);
		cout << "Manully add Food" << endl;
		cout << "Engine Stop" << endl;
		pump.stop();
		addFood.setAngle(0);
	}

#ifdef TEST
	// cout << Bluetooth Test <<  endl;
	// cout << "already add" << endl;
	// datsToSendLock.lock();
    // dataToSend[0] = 'C';
    // dataToSend[1] = 'C';
    // dataToSend[2] = 'C';
    // dataToSend[3] = 'C';
    // dataToSend[4] = 99;
    // datsToSendLock.unlock();
#endif
}

void AddFood::scanFood() {
    running = true;
	gpioSetTimerFunc(0, 1000, &timerCallback1);//1s callback
    
    while (running) //while(1)
    {

    }
}

void AddFood::registerCallback(AddFoodcallback* af){
	addFoodCallbacks.push_back(af);
}

void AddFood::start(){
	t = thread(&AddFood::scanFood,this);
}

void AddFood::stop(){
	// running = false;
	t.join();
}
