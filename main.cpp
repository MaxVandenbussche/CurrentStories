#include "plant.h"
#include <iostream>
#include <ctime>


#define MAX_STORYPLANTS 9
#define I2C_BUS "/dev/i2c-1"

using namespace std;

//Prototypes
void testLed(void);
void scanPlants(void);
void checkTouch(void);


// Global variables
unsigned char connectedStoryPlants = 0;
plant storyTellingPlants[MAX_STORYPLANTS];
motherPlant listeningPlant;
time_t now;

int main(void)
{

	scanPlants();
	printf("Found %i plants connected!\n", connectedStoryPlants);



	while(1){
		checkTouch();
		sleep(0.5);

	}

	cout << "exiting" << endl;
	//destructor called just before program exit
	return 0;
}

void testLed(){

}

void checkTouch(){
	for(int plantIndex = 0; plantIndex < connectedStoryPlants; plantIndex++){
		// Only request data from slaves one time -> save in local variable.
		bool touched = 	storyTellingPlants[plantIndex].isTouched();
		bool playing = storyTellingPlants[plantIndex].isPlayingSound();

		if(touched){
			time(&now);
			printf("Plant %i touched at %s",plantIndex, ctime(&now));
			storyTellingPlants[plantIndex].flashLed(2);

			if(touched & !playing){
				storyTellingPlants[plantIndex].playStory();
			}
		}

		storyTellingPlants[plantIndex].update();
	}
}


void scanPlants(){
	int retVal = -1;
	int i2cDescriptor;
	unsigned char sendbuf[1];
	sendbuf[0] = 0;
	i2cDescriptor = open(I2C_BUS, O_RDWR);

	for(unsigned char adress = 8; adress < 127; adress++){

		retVal = ioctl(i2cDescriptor, I2C_SLAVE, adress);
		retVal = write(i2cDescriptor, sendbuf , 1); // try to write a byte to the plant
		if(retVal == 1){
			printf("Found plant at adress: 0x%x\n", adress);
			plant newPlant(adress, string(I2C_BUS));
			storyTellingPlants[connectedStoryPlants] = newPlant;
			connectedStoryPlants++;
		}

		sleep(0.1);
	}

	retVal = close(i2cDescriptor);
}
