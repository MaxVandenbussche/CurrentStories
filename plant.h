/*
 * plant.h
 *
 *  Created on: May 7, 2016
 *      Author: max
 */

#ifndef PLANT_H_
#define PLANT_H_

#include <string>
#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define MOTHERADDRESS 0xFB
#define MAXSTORIES 3


// Recording plant states for statemachine


enum states{
	IDLE,
	EXPLAINING,
	RECORDING,
	CONFIRMING
};

class plant
{
    public:
		plant(void); 											// default constructor
		plant(unsigned char dev_addr, std::string i2cfilename); //over loaded constructor
        ~plant(void); 											// destructor
        //public member functions
        int setLed(unsigned char brightness); 					// Set the led brightness
        bool isTouched(bool debounce);							// Check wether plant is touched
        bool isTouched();
        int assignStory(std::string story);
        int playStory();
        bool isPlayingSound();
        void flashLed(unsigned int seconds);
        void update();
        // public member variables
        int storieFade = 0;


    private:
        //private member functions
        int openI2C();
        int closeI2C();
        int sendByte(unsigned char data);
        int readByte(unsigned char *data);
        void playFile(std::string file);

        // private member variables
        std::string  i2cFileName;
        int i2cDescriptor;
        unsigned char deviceAddress;
        unsigned char toucheThreshold;
        unsigned char currentBrightness = 0;
        unsigned char prevBrightness = 0;

        bool playingSound;
        pid_t playPid = -1;
        time_t now = 0;
        time_t touchTimer = 0;
        time_t flashTimer = 0;
        std::string stories[MAXSTORIES] = 0;
        int assignedStories = 0;

        //static const char *aplayCmd = "aplay -D%s %s &";
};

class motherPlant : public plant{
	public:
		motherPlant();
		int update();

	private:
		int record();
		bool isRecording();

		pid_t recordPid = -1;
		int state = IDLE;
};



#endif /* PLANT_H_ */
