/**
 * FSAE LCD Library
 *
 * Processor:   PIC32MZ2048EFM100
 * Compiler:    Microchip XC32
 * Author:      Jake Leonard
 * Created:     2015-2016
 */
#include "RA8875_driver.h"
#include "Wheel.h"
#include "FSAE_LCD.h"

// Initialize all the data streams
// This fn must be run before CAN is initialized
void initDataItems(void){
	// Motec Vars
	initDataItem(&rpm,0,0,1000,2,1);
	initDataItem(&throtPos,0,0,1000,2,1);
	initDataItem(&oilPress,0,0,1000,1,1);
	initDataItem(&oilTemp,0,0,1000,3,0);
	initDataItem(&waterTemp,0,0,1000,3,0);
	initDataItem(&lambda,0,0,1000,2,1);
	initDataItem(&manifoldPress,0,0,1000,2,1);
	initDataItem(&batVoltage,0,0,1000,2,1);
	initDataItem(&wheelSpeedFL,0,0,1000,2,1);
	initDataItem(&wheelSpeedFR,0,0,1000,2,1);
	initDataItem(&wheelSpeedRL,0,0,1000,2,1);
	initDataItem(&wheelSpeedRR,0,0,1000,2,1);
	initDataItem(&gpsLong,0,0,1000,2,1);
	initDataItem(&gpsLat,0,0,1000,2,1);
	initDataItem(&groundSpeed,0,0,1000,2,1);
	initDataItem(&driveSpeed,0,0,1000,2,1);
	initDataItem(&gpsSpeed,0,0,1000,2,1);
	initDataItem(&manifoldTemp,0,0,1000,2,1);
	initDataItem(&ambientTemp,0,0,1000,2,1);
	initDataItem(&ambientPress,0,0,1000,2,1);
	initDataItem(&fuelTemp,0,0,1000,2,1);
	initDataItem(&fuelPress,0,0,1000,2,1);
	initDataItem(&lambda1,0,0,1000,2,1);
	initDataItem(&lambda2,0,0,1000,2,1);
	initDataItem(&lambda3,0,0,1000,2,1);
	initDataItem(&lambda4,0,0,1000,2,1);
	initDataItem(&lcEnablity,0,0,1000,2,1);
	initDataItem(&fuelConsum,0,0,1000,2,1);
	initDataItem(&gpsAltitude,0,0,1000,2,1);
	initDataItem(&gpsTime,0,0,1000,2,1);
	initDataItem(&runTime,0,0,1000,2,1);
	initDataItem(&fuelInjDuty,0,0,1000,2,1);
	initDataItem(&fuelTrim,0,0,1000,2,1);

	// Tire Temps
	initDataItem(&ttFL1,0,0,1000,2,1);
	initDataItem(&ttFL2,0,0,1000,2,1);
	initDataItem(&ttFL3,0,0,1000,2,1);
	initDataItem(&ttFL4,0,0,1000,2,1);
	initDataItem(&ttFR1,0,0,1000,2,1);
	initDataItem(&ttFR2,0,0,1000,2,1);
	initDataItem(&ttFR3,0,0,1000,2,1);
	initDataItem(&ttFR4,0,0,1000,2,1);
	initDataItem(&ttRL1,0,0,1000,2,1);
	initDataItem(&ttRL2,0,0,1000,2,1);
	initDataItem(&ttRL3,0,0,1000,2,1);
	initDataItem(&ttRL4,0,0,1000,2,1);
	initDataItem(&ttRR1,0,0,1000,2,1);
	initDataItem(&ttRR2,0,0,1000,2,1);
	initDataItem(&ttRR3,0,0,1000,2,1);
	initDataItem(&ttRR4,0,0,1000,2,1);

	// Steering Wheel
	initDataItem(&swTemp,0,0,1000,2,1);
	initDataItem(&swSW1,0,0,1000,2,1);
	initDataItem(&swSW2,0,0,1000,2,1);
	initDataItem(&swSW3,0,0,1000,2,1);
	initDataItem(&swSW4,0,0,1000,2,1);
	initDataItem(&swROT1,0,0,1000,2,1);
	initDataItem(&swROT2,0,0,1000,2,1);
	initDataItem(&swROT3,0,0,1000,2,1);
	initDataItem(&swTROT1,0,0,1000,2,1);
	initDataItem(&swTROT2,0,0,1000,2,1);
	initDataItem(&swBUT1,0,0,1000,2,1);
	initDataItem(&swBUT2,0,0,1000,2,1);
	initDataItem(&swBUT3,0,0,1000,2,1);
	initDataItem(&swBUT4,0,0,1000,2,1);

	// Paddle Shifting
	initDataItem(&paddleTemp,0,0,1000,2,1);
	initDataItem(&gearPos,0,0,1000,1,0);
	initDataItem(&neutQueue,0,0,1000,2,1);
	initDataItem(&upQueue,0,0,1000,2,1);
	initDataItem(&downQueue,0,0,1000,2,1);
	initDataItem(&gearVoltage,0,0,1000,2,1);

	// PDM
	initDataItem(&pdmTemp,0,0,1000,2,1);
	initDataItem(&pdmICTemp,0,0,1000,2,1);
	initDataItem(&pdmCurrentDraw,0,0,1000,2,1);
	initDataItem(&pdmVBat,0,0,1000,2,1);
	initDataItem(&pdm12v,0,0,1000,2,1);
	initDataItem(&pdm5v5,0,0,1000,2,1);
	initDataItem(&pdm5v,0,0,1000,2,1);
	initDataItem(&pdm3v3,0,0,1000,2,1);
	initDataItem(&pdmIGNdraw,0,0,1000,2,1);
	initDataItem(&pdmIGNcut,0,0,1000,2,1);
	initDataItem(&pdmINJdraw,0,0,1000,2,1);
	initDataItem(&pdmINJcut,0,0,1000,2,1);
	initDataItem(&pdmFUELdraw,0,0,1000,2,1);
	initDataItem(&pdmFUELNcut,0,0,1000,2,1);
	initDataItem(&pdmFUELPcut,0,0,1000,2,1);
	initDataItem(&pdmECUdraw,0,0,1000,2,1);
	initDataItem(&pdmECUNcut,0,0,1000,2,1);
	initDataItem(&pdmECUPcut,0,0,1000,2,1);
	initDataItem(&pdmWTRdraw,0,0,1000,2,1);
	initDataItem(&pdmWTRNcut,0,0,1000,2,1);
	initDataItem(&pdmWTRPcut,0,0,1000,2,1);
	initDataItem(&pdmFANdraw,0,0,1000,2,1);
	initDataItem(&pdmFANNcut,0,0,1000,2,1);
	initDataItem(&pdmFANPcut,0,0,1000,2,1);
	initDataItem(&pdmAUXdraw,0,0,1000,2,1);
	initDataItem(&pdmAUXcut,0,0,1000,2,1);
	initDataItem(&pdmPDLUdraw,0,0,1000,2,1);
	initDataItem(&pdmPDLUcut,0,0,1000,2,1);
	initDataItem(&pdmPDLDdraw,0,0,1000,2,1);
	initDataItem(&pdmPDLDcut,0,0,1000,2,1);
	initDataItem(&pdm5v5draw,0,0,1000,2,1);
	initDataItem(&pdm5v5cut,0,0,1000,2,1);
	initDataItem(&pdmBATdraw,0,0,1000,2,1);
	initDataItem(&pdmBATcut,0,0,1000,2,1);
	initDataItem(&pdmSTR0draw,0,0,1000,2,1);
	initDataItem(&pdmSTR0cut,0,0,1000,2,1);
	initDataItem(&pdmSTR1draw,0,0,1000,2,1);
	initDataItem(&pdmSTR1cut,0,0,1000,2,1);
	initDataItem(&pdmSTR2draw,0,0,1000,2,1);
	initDataItem(&pdmSTR2cut,0,0,1000,2,1);
	initDataItem(&pdmSTRdraw,0,0,1000,2,1);

	// Uptimes
	initDataItem(&paddleUptime,0,0,1000,2,1);
	initDataItem(&loggerUptime,0,0,1000,2,1);
	initDataItem(&swUptime,0,0,1000,2,1);
	initDataItem(&pdmUptime,0,0,1000,2,1);
}

void initDataItem(dataItem* data, double warn, double err, uint32_t refresh, 
				uint8_t whole, uint8_t dec){
	data->value = 0;
	data->warnThreshold = warn;
	data->errThreshold = err;
	data->refreshInterval = refresh;
	data->wholeDigits = whole;
	data->decDigits = dec;
}

// Initializes all the screen and screenitem variables in all
// the screens that might be displayed
void initAllScreens(void){
	allScreens[0] = &raceScreen;
	raceScreen.items = raceScreenItems;
	raceScreen.len = 5;
	initScreenItem(&(raceScreen.items[0]), 10, 30, 30, &oilTemp);
	initScreenItem(&(raceScreen.items[1]), 300, 30, 30, &waterTemp);
	initScreenItem(&(raceScreen.items[2]), 10, 150, 30, &oilPress);
	initScreenItem(&(raceScreen.items[3]), 300, 180, 30, 0x0);
	initScreenItem(&(raceScreen.items[4]), 150, 50, 100, &gearPos);
}

void initScreenItem(screenItem* item, uint16_t x, uint16_t y, uint16_t size, 
				dataItem* data){
	item->x = x;
	item->y = y;
	item->size = size;
	item->currentValue = 1;
	item->data = data;
	item->refreshTime = millis;
}

// Helper function for drawing labels and such on a new screen
void initScreen(uint8_t num){
	if(num == RACE_SCREEN){
		textMode();
		textSetCursor(0, 0);
		textTransparent(FOREGROUND_COLOR);
		textEnlarge(0);
		textWrite("OIL TMP", 0);
		textSetCursor(0, 100);
		textWrite("OIL PRESS", 0);
		textSetCursor(300, 0);
		textWrite("WTR TMP", 0);
	}
}

// User called function to change which screen is being displayed
void changeScreen(uint8_t num){
	if( num < 0 || num > NUM_SCREENS) {return;}
	screenNumber = num;
	clearScreen();
	initScreen(num);
	refreshScreenItems();
}

// Function to asynchronously update all the items currently being displayed
// A number will only be redrawn if its dataItem value has changed, and it 
// has surpassed its refresh interval
void refreshScreenItems(void){
	screen *currScreen = allScreens[screenNumber];
	int i;
	for(i = 0;i<currScreen->len;i++){
		if(currScreen->items[i].data == 0x0){
			continue;
		}
		if((currScreen->items[i]).currentValue != (currScreen->items[i]).data->value && 
						millis - (currScreen->items[i]).refreshTime >= 
						(currScreen->items[i]).data->refreshInterval){
			redrawItem(&(currScreen->items[i]));
		}
	}
}

// Redraw Helper Function
void redrawItem(screenItem * item){
	uint16_t fillColor;
	if(item->currentValue >= item->data->warnThreshold){
		if(item->currentValue >= item->data->errThreshold){
			fillColor = ERROR_COLOR;
		}else{
			fillColor = WARNING_COLOR;
		}
	}else{
		fillColor = BACKGROUND_COLOR;
	}
	uint16_t wholeNums, decNums;
	wholeNums = item->data->wholeDigits;
	decNums = item->data->decDigits;
	uint16_t fillWidth = (item->size)*(wholeNums + decNums) + (item->size)*0.2*(wholeNums + decNums - 1);
	if(decNums){
		fillWidth += (item->size)/5;
	}
	fillRect(item->x, item->y, fillWidth, (item->size)*1.75, fillColor);
	sevenSegmentDecimal(item->x, item->y, item->size, wholeNums + decNums, decNums, FOREGROUND_COLOR, item->data->value);
	item->refreshTime = millis;
	item->currentValue = item->data->value;
}

void clearScreen(void){
	fillScreen(BACKGROUND_COLOR);
}