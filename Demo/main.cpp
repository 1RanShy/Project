#include <pigpio.h>
#include "./include/Servo.h"
#include "./include/SR04.h"
#include "./include/AddFood.h"
#include "./include/Blue.h"
#include "BlueSendReceive.h"
// #include "globalVar.h"
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <mutex>
using namespace std;

mutex datsToSendLock;
mutex dataReceivedLock;
mutex autoAddFoodFlagLock;

char dataToSend[5] = {'H','E','L','L','O'};
char dataReceived[5] = {'H','E','L','L','O'};
bool autoAddFoodFlag = true; //1: autoAddFood 0: manually addFood

struct MyAddFoodCallback : AddFoodcallback {
public:
	void addFood() {
        Servo servo1(16);
        servo1.setAngle(0);
    }
    void stopAdd() {
        Servo servo1(16);
        servo1.setAngle(180);
    }
};

struct MyBlueToothcallback : BlueToothcallback {
public:
	void sendData(Blue *ble) //纯虚函数 必须在类中声明 
    {
        datsToSendLock.lock();
        ble->writeBytes(dataToSend, 5);
        for(int i=0; i <5; i++)
        {
            dataToSend[i] = 'R';
            
        }
        #ifdef TEST
        cout << "clear Array" << endl;
        #endif
        datsToSendLock.unlock();
    }

    void receiveData(Blue *ble)
    {
        // Blue ble;
        dataReceivedLock.lock();
        ble->readBytes(dataReceived);
        dataReceivedLock.unlock();  
        //The judgement should be like the code below:
    }

    void dataProcess()
    {
        if(dataReceived[0] == 'C' && dataReceived[4] == 'C')
        {
            //switch autoAddFood
            //Choose whether to feed automatically CAAAC
            if(dataReceived[1] == 'A' && dataReceived[2] == 'A' && dataReceived[3] == 'A')
            {
                autoAddFoodFlagLock.lock();
                autoAddFoodFlag = !autoAddFoodFlag;
                autoAddFoodFlagLock.unlock();
                
                //make bluetooth send data
                datsToSendLock.lock();
                // 1 自动喂食
                if(autoAddFoodFlag == true)
                {
                    for(int i = 0; i<5; i++)
                    {
                        dataToSend[i] = '1';
                    }
                }
                //2 手动喂食
                else
                {
                    for(int i = 0; i<5; i++)
                    {
                        dataToSend[i] = '2';
                    }
                }
                datsToSendLock.unlock();
            }

            for(int i = 0; i<5; i++)
            {
                // cout << dataReceived[i];
                // cout << endl;
                dataReceived[i] = 'z';
            }
        }
    }
};


int main()
{
	gpioInitialise();
	MyAddFoodCallback callback1; // 这个是回调函数
	AddFood addFood;	// 这个是真正的线程
	addFood.registerCallback(&callback1); // 线程注册回调函数
    
    addFood.start(); // 线程开启时就声明了这个线程从哪个函数开始执行 // t = thread(&scan::Readscan,this);
	

	MyBlueToothcallback  callback2; // 这个是回调函数
	BLEDataProcessing dataProcess;	// 这个是真正的线程
	dataProcess.registerCallback(&callback2); // 线程注册回调函数
	
	dataProcess.start(); // 线程开启时就声明了这个线程从哪个函数开始执行 // t = thread(&scan::Readscan,this);
	
	dataProcess.stop();
    // addFood.stop();
	cout << "Main thread finished." << endl;


	return 0;
}
