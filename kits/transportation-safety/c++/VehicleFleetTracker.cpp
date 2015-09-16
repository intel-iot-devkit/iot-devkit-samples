/*
 * Author: John Van Drasek <john.r.van.drasek@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file
 * @ingroup other 
 * @brief Vehicle Fleet Tracker 
 * 
 * Example to show multiple sensors in a vehicle fleet management scenario.
 * To send data to the cloud, the iotkit-agent must be started at the edison
 * command prompt using:
 * - systemctl start iotkit-agent
 *
 * @hardware Sensors used:\n
 * Grove GPS (SEN10752P) is used to gather GPS data.\n
 * Grove IR Distance Interrupter (SEN09281P) is used to detect objects when backing up vehicle.\n
 * Grove LCD RGB Backlight (811004001) is used to display a warning message that the vehicle is close to an object when backing up.\n
 * Grove LCD RGB Backlight (811004001) is also used to display the local time.\n
 * Grove IR Distance Interrupter (SEN09281P) is used to detect when the tailgate is open.\n
 * Grove - LED (COM04054P) is used to indicate to the operator of the vehicle the tailgate is open.
 *
 * @cc 
 * @cxx -std=c++11
 * @ld -lupm-ublox6 -lupm-i2clcd -lupm-grove -lupm-rpr220 -lupm-rfr359f 
 *
 * Additional source files required to build this example:
 * @req UdpClient.cpp
 * @req UdpClient.hpp 
 *
 * @date 07/06/2015
 */

#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <sstream>
#include "UdpClient.hpp"/* Sending data to cloud over UDP */
#include <string.h>		/* string */
#include <time.h>		/* time */
#include <thread>		/* threading */
#include "ublox6.h"		/* gps */
#include "jhd1313m1.h"	/* lcd */
#include "rfr359f.h"	/* distance interrupter */
#include "rpr220.h"		/* reflective sensor */
#include "grove.h"		/* led */

using namespace std;

/*
 * NODE (host) and SERVICE (port)
 * iotkit-agent is listening for UDP data
 * as defined in /etc/iotkit-agent/config.json
 */
#define NODE "localhost"
#define SERVICE "41234"

volatile bool shouldRun = true;
const size_t bufferLength = 256;
static volatile bool g_displayTime = true;
UdpClient client;

/**
 * Interrupt handler to catch the SIGING signal, ctrl-c
 * From command line only
 */
void sig_handler(int signo)
{
	if (signo == SIGINT)
	{
		shouldRun = false;
		cout << "Exiting..." << endl;
		cout << "Please wait up to 10secs for threads to finish." << endl;
	}
}

/**
 * @brief Write data to the cloud
 * Note: you have to register components prior.
 * E.g. using "iotkit-admin register <comp_name> <catalogid>" or via iotkit-agent
 */
void writeDataToCloud(string &component, string &data) {
	UdpClient client;
	stringstream ss;

	/* Create a udp connection for iotkit-agent to communicate over */
	if (client.isConnected() == false) {
		if (client.connectUdp(NODE, SERVICE) < 0) {
			cerr << "connection failed" << endl;
			exit(EXIT_FAILURE);
		}
	}

	/*
	 * Create the string to send to the cloud
	 * eg. {"n":"gpsv1","v":"45.550953, -122.968719"}
	 */
	ss << "{\"n\":\"" << component << "\",\"v\":\"" << data << "\"}" << endl;
	cout << ss.str();

	if (client.writeData(ss) < 0)
		cerr << "Cannot write to cloud\n" << endl;

	ss.str("");
}

/**
 * @brief Split a string separated by a delimiter
 * @return a vector of strings
 */
vector<string> split(const string &str, char delimiter) {
	vector<string> elements;
	stringstream ss(str);
	string item;

	while (getline(ss, item, delimiter)) {
		elements.push_back(item);
	}
	return elements;
}

/**
 * @brief Convert the coordinates to degrees
 * @return a float value representing degrees
 */
float convertRawCoords(string rawCoord, string cardinalDirection) {
	string::size_type sz;
	float ret = 0.0f;
	float direction = 1;

	/* If the incoming coordinate is empty, exit */
	if (rawCoord.empty())
		return 0;

	/* If direction is either South or West, coordinate needs to be negative */
	if (cardinalDirection == "S" || cardinalDirection == "W")
		direction = -1;

	/* Get degrees, eg: rawCoord = 4532.5103 => 45 */
	float degrees = std::stoi(rawCoord, &sz) / 100;

	/* Get minutes, eg: rawCoord = 4532.5103 => 32 */
	float minutes = (((float) (std::stoi(rawCoord, &sz))) / 100);  //45.32
	minutes = (minutes - (long) minutes) * 100; // .32 * 100 = 32

	/* Get seconds, eg: rawCoord = 4532.5103 => 51.03 */
	float seconds = std::stof(rawCoord, &sz); // 4532.5103
	seconds = seconds - (long) seconds; // .5103
	seconds *= 100; // .5103 * 100 = 51.03

	/**
	 * Convert coord to degrees: degrees + (minutes/60) + (seconds/3600)
	 * eg: 45 + (32/60) + (51.03/3600) = 45.5475 degrees
	 * Then, Add direction, either negative(S or W) or positive(N or E)
	 */
	ret = degrees + (minutes / 60) + (seconds / 3600);
	ret *= direction;

	return ret;
}

/**
 * @brief Write Gps sensor data to the cloud
 */
void parseGpsData(char* nmeaBuffer, int result) {
	vector<string> vecStr;
	string strCloudComponent = "gpsv1";
	string strCloudData;
	string strLatitude;
	string strLatitudeDir;
	string strLongitude;
	string strLongitudeDir;
	string strGpsLine;
	char c;

	for (int i = 0; i < result; ++i) {
		c = nmeaBuffer[i];

		/* We've reached the end of file or the end of line */
		if (c == EOF || c == '\n') {
			/* If end of file reached, exit for loop */
			if (c == EOF)
				break;

			/* if we have an empty line, just continue with next iteration of loop */
			if (strGpsLine.empty())
				continue;

			/**
			 * Split the nmea string by the comma delimiter, putting
			 * each item into it's own element in the vector
			 * Example string:
			 *   $GPGGA,172410.000,4532.5103,N,12257.6936,W,1,7,0.99,74.9,M,-19.6,M,,*5F
			 *   where:
			 * 		$GPGGA 		- Global Positioning System Fix Data
			 *		172410.000 	- Universal Time Coordinated (UTC) hhmmss.ss
			 *		4532.5103 	- Latitude (Degree minutes and seconds)
			 *		N 			- North or South (South adds negative to latitude, North is positive)
			 *		12257.6936 	- Longitude (Degree minutes and seconds)
			 *		W			- East or West (West adds negative to Longitude, East is positive)
			 *		1			- GPS quality indicator - 1: gps fix
			 *		7			- Number of satelites in view
			 *		0.99		- Horizontal dillution of precision (meters)
			 *		74.9		- GPS antenna above/below mean-sea-level (geoid) (in meters)
			 *		M			- Units of antenna altitude, meters
			 *		-19.6		- Geoidal separation "-" is mean-sea-level below ellipsoid
			 *		M			- Units of Geoidal separation, meters
			 *		N/A			- Age of differential gps data
			 *		N/A			- Differential reference staion ID 0000-1023
			 *		*5F			- Checksum
			 */
			vecStr = split(strGpsLine, ',');

			/* We're only interested in the $GPGGA entries */
			if (vecStr[0] == "$GPGGA") {
				strCloudData = "";
				strLatitude = vecStr[2]; /* Latitude */
				strLatitudeDir = vecStr[3]; /* Cardinal direction */
				strLongitude = vecStr[4]; /* Longitude */
				strLongitudeDir = vecStr[5]; /* Cardinal direction */

				/* Convert raw latitude/longitude data to degree coordinates, eg. 45.2345 */
				strCloudData += to_string(convertRawCoords(strLatitude, strLatitudeDir));
				strCloudData += ", ";
				strCloudData += to_string(convertRawCoords(strLongitude, strLongitudeDir));
				writeDataToCloud(strCloudComponent, strCloudData);
			}
			/* Clear contents of string */
			strGpsLine.clear();
			break; /* Break, we just want one GPGGA string from the group of data */
		} else {
			/* Add the character to the string */
			strGpsLine += nmeaBuffer[i];
		}
	}
}

/**
 * @brief ublox6 GPS sensor - get GPS data from sensor
 */
void getGpsData(upm::Ublox6* gps) {
	char nmeaBuffer[bufferLength];

	/* Make sure port is initialized properly.  9600 baud is the default. */
	if (!gps->setupTty(B9600)) {
		cerr << "Failed to setup tty port parameters" << endl;
	}

	/**
	 * Collect and output NMEA data.  There are various libraries out on
	 * the Internet, such as tinyGPS or tinyGPS++ that can handle
	 * decoding NMEA data and presenting it in a more easily accessible
	 * format.  This example will just check for, and read raw NMEA data
	 * from the device and output it on stdout.
	 * This device also supports numerous configuration options, which
	 * you can set with writeData().  Please refer to the Ublox-6 data
	 * sheet for further information on the formats of the data sent and
	 * received, and the various operating modes available.
	 */
	while (shouldRun) {

		/* Check to see if data is available first. */
		if (gps->dataAvailable()) {
			int result = gps->readData(nmeaBuffer, bufferLength);

			/* Result will be non-zero if we have acquired data from sensor */
			if (result > 0) {
				/* Write Gps sensor data to the cloud */
				parseGpsData(nmeaBuffer, result);
			}

			if (result < 0) {
				/* Some sort of read error occurred */
				cerr << "Port read error." << endl;
				break;
			}
			continue;
		}
		usleep(10000000); // 10 secs
	}
}

/**
 * @brief Distance interrupter check - mounted on rear bumper of vehicle to check if close to an object when backing up
 */
void backupDistanceChecker(upm::RFR359F* distanceInterrupter,
		upm::Jhd1313m1* lcd) {
	while (shouldRun) {
		if (distanceInterrupter->objectDetected()) {
			/**
			 * An object was detected by the sensor
			 * Stop displaying time on the lcd
			 * Write line to lcd to notify operator to stop
			 */
			g_displayTime = false;
			lcd->clear();
			lcd->setCursor(0, 0);
			lcd->write("Within 4\" - STOP!!!");
			usleep(500000);
		} else {
			/**
			 * Display time when distance interrupter does not detect an object
			 */
			g_displayTime = true;
		}
	}
}

/**
 * @brief Reflective sensor check - used to check if the tailgate of the vehicle is open
 */
void tailgateChecker(upm::RPR220* reflectiveSensor, upm::GroveLed* led) {
	bool writeOnce = true;
	string strCloudComponent = "reflectorv1";
	string strCloudData = "true";

	while (shouldRun) {
		if (reflectiveSensor->blackDetected()) {
			/**
			 * The reflective sensor has been triggered
			 * Turn on led to indicate tailgate is open
			 */
			led->on();

			/* Write data to cloud each time the tailgate opens */
			if (writeOnce) {
				writeDataToCloud(strCloudComponent, strCloudData);
				writeOnce = false;
			}
			usleep(500000);
		} else {
			led->off();
			writeOnce = true;
		}
	}
}

/**
 * @brief Display the local time on the lcd
 */
void displayTime(upm::Jhd1313m1* lcd) {
	time_t rawtime;
	struct tm * timeinfo;

	while (shouldRun) {
		if (g_displayTime) {
			lcd->clear();
			lcd->setCursor(0, 0);
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			lcd->write(asctime(timeinfo));
			usleep(500000);
		}
	}
}

int main(int argc, char **argv) {
	signal(SIGINT, sig_handler);

	//! [Interesting]
	/* create a Ublox6 GPS device on UART 0 (port UART on shield) */
	upm::Ublox6* gps = new upm::Ublox6(0);
	/* create a led instance on GPIO pin 5 (port D5 on shield) */
	upm::GroveLed* led = new upm::GroveLed(5);
	/* Create a lcd instance on I2C bus 0 (port ) */
	/* 0x62 RGB_ADDRESS, 0x3E LCD_ADDRESS */
	upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(0, 0x3E, 0x62);
	/* Create a Grove IR Distance Interrupter on GPIO pin 2 (port D2 on shield) */
	upm::RFR359F* distanceInterruptor = new upm::RFR359F(2);
	/* Create a Grove IR Reflective Sensor on GPIO pin 4 (port D4 on shield) */
	upm::RPR220* reflectiveSensor = new upm::RPR220(4);

	/**
	 *  Start threads to start collecting data from sensors:
	 *    Get data from gps sensor
	 *    Distance interrupter checks for objects when backing up vehicle
	 *    Reflective sensor checks if tailgate of vehicle is open
	 *    Display the time on the lcd
	 *
	 *    These threads will run until user presses ctrl-c
	 */
	thread threadGetGpsData(getGpsData, gps);
	thread threadBackupDistanceChecker(backupDistanceChecker,
			distanceInterruptor, lcd);
	thread 	threadTailgateChecker(tailgateChecker, reflectiveSensor, led);
	thread threadDisplayTime(displayTime, lcd);


	/**
	 * Program is running now after the thread create and will not execute the
	 * thread joins() below until it is terminated with ctrl-c.
	 *
	 * When running in the terminal, press ctrl-c to exit threads and stop program
	 * In Eclipse, click the red square terminate button in the Console window toolbar
	 */


	/* Join threads */
	threadGetGpsData.join();
	threadBackupDistanceChecker.join();
	threadTailgateChecker.join();
	threadDisplayTime.join();

	lcd->clear();
	led->off();

	//! [Interesting]

	delete gps;
	delete led;
	delete lcd;
	delete distanceInterruptor;
	delete reflectiveSensor;

	return 0;
}
