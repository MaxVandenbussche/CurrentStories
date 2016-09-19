/*
 * plant.cpp
 *
 *  Created on: May 7, 2016
 *      Author: max
 */


#include "plant.h"
#include <iostream>
#include <wait.h>

using namespace std;


plant::plant(void){
    this->i2cFileName = "/dev/i2c-0";
    this->deviceAddress= 0;
    this->i2cDescriptor = -1;
    this->toucheThreshold = 10;
    this->playingSound = false;

}

plant::plant(unsigned char dev_addr, std::string i2c_file_name){
    this->i2cFileName = i2c_file_name;
    this->deviceAddress = dev_addr;
    this->i2cDescriptor = -1;
    this->toucheThreshold = 10;
    this->playingSound = false;

}

plant::~plant(void){
    cout << " Closing I2C Device" << endl;
    //this->closeI2C();
}

int plant::openI2C(){
    this->i2cDescriptor = open(i2cFileName.c_str(), O_RDWR);
    if(this->i2cDescriptor < 0){
        perror("Could not open file (1)");
        exit(1);
    }

    return i2cDescriptor;
}

int plant::closeI2C(){
    int retVal = -1;
    retVal = close(this->i2cDescriptor);
    if(retVal < 0){
        perror("Could not close file (1)");
        //exit(1);
    }
    return retVal;
}

int plant::sendByte(unsigned char data){
	int retVal = -1;
	unsigned char sendbuf[1];
	sendbuf[0] = data;

	this->openI2C();
	retVal = ioctl(this->i2cDescriptor, I2C_SLAVE, this->deviceAddress); 	// Opening the bus
	if(retVal < 0){
		perror("Plant node did not respond to address");
	}

	retVal = write(this->i2cDescriptor, sendbuf, 1);						// Sending data
	if(retVal < 0){
		perror("Plant node did not acknowledge data");
	}
	this->closeI2C();

	return retVal;

}

int plant::readByte(unsigned char *data){
	int retVal = -1;
	unsigned char buf[1];

	this->openI2C();

	retVal = ioctl(this->i2cDescriptor, I2C_SLAVE, this->deviceAddress);
	if(retVal < 0){
			perror("Plant node did not respond to address");
	}

	retVal = read(this->i2cDescriptor, buf, 1);
	if(retVal != 1){
		perror("Plant node did not respond properly");
	}

	this->closeI2C();

	*data = buf[0];

	return retVal;
}


int plant::setLed(unsigned char Brightness){

	if(Brightness != this->currentBrightness){
		this->currentBrightness = Brightness;
		return this->sendByte(Brightness);
	}
	return 0;

}

// This needs a more clever approach of detecting touch
bool plant::isTouched(bool debounce = true){
	time(&now);
	if(difftime(now, this->touchTimer) < 1.5 & debounce){
		return false;
	}

	unsigned char toucheData = 0;
	this->readByte(&toucheData);

	if(toucheData < this->toucheThreshold){
		time(&this->touchTimer);
		return true;
	}
	return false;
}

bool plant::isTouched(){
	return this->isTouched(false);
}

void plant::playFile(std::string file){
	this-> playPid = fork();
	if (playPid == 0){  // child
		char  command[100];
		sprintf(command,"aplay -Dplant0 /home/pi/%s > /dev/null 2>&1", file.c_str());
		cout << "playing psst.wav..." << endl;
		//system("aplay -Dplant0 /home/pi/psst.wav > /dev/null 2>&1");
		system(command);
		cout << "End of sound file." << endl;
		_exit(0);
	}

}

int plant::assignStory(std::string file){

	return 1;
}

int plant::playStory(){
	sleep(0.01);
	if(this->isPlayingSound()){
		return 0;
	}

	if(assignedStories == 0){ // no stories assigned yet to this plant.
		playFile("Geen verhaal.wav");
		return 0;
	}

	int randStory = rand() % assignedStories;
	playFile(this->stories[randStory]);

	return randStory;
}

bool plant::isPlayingSound(){
	int status;
	pid_t result;
	if(this->playPid > 0){
		result = waitpid(playPid, &status, WNOHANG);
	}
	if(result == 0){
		return true;
	}
	else if (result == -1){
		perror("Something is wrong");
	}
	return false;
}

void plant::flashLed(unsigned int seconds){
	prevBrightness = currentBrightness;
	if(currentBrightness > 127){
		this->setLed(0);
	}
	else{
		this->setLed(255);
	}
	time(&this->flashTimer);
}

void plant::update(){
	// Check led flash timer
	time(&now);
	double timeDiff = difftime(now, this->flashTimer);
	if(timeDiff > 1.5){
		this->setLed(prevBrightness);
	}


}

int motherPlant::update(){
	switch(this->state){
	case IDLE:
		if(isTouched()){
			this->playFile("Explanation.wav");
			this->state = EXPLAINING;
		}
		break;

	case EXPLAINING:
		if(this->isPlayingSound()){
			break;
		}
		if(this->isTouched()){
			//Start recording
			this->state = RECORDING;
		}
		break;

	case RECORDING:


		break;

	case CONFIRMING:


		break;
	}
	return 1;
}

int motherPlant::record(){
	sleep(0.01);
	if(this->isPlayingSound()){
		return 0;
	}
	string filename = "Story1";

	recordPid = fork();
	if (recordPid == 0){  // child
		char  command[100];
		sprintf(command,"arecordplay -Dplant0 /home/pi/%s > /dev/null 2>&1", filename.c_str());
		system(command);
		_exit(0);
	}
	return 1;
}

bool motherPlant::isRecording(){
	int status;
	pid_t result;
	if(this->recordPid > 0){
		result = waitpid(recordPid, &status, WNOHANG);
	}
	if(result == 0){
		return true;
	}
	else if (result == -1){
		perror("Something is wrong");
	}
	return false;
}

