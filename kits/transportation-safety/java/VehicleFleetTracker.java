
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;

import upm_grove.GroveLed;
import upm_i2clcd.Jhd1313m1;
import upm_rfr359f.RFR359F;
import upm_rpr220.RPR220;
import upm_ublox6.Ublox6;

/*
 * Author: John Van Drasek <john.r.van.drasek@intel.com>
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * 
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
 * @hardware - Grove GPS (SEN10752P) is used to gather GPS data.
 * - Grove IR Distance Interrupter (SEN09281P) is used to detect objects when backing up vehicle.
 * - Grove LCD RGB Backlight (811004001) is used to display a warning message that the vehicle is close to an object when backing up.
 * - Grove LCD RGB Backlight (811004001) is also used to display the local time.
 * - Grove IR Distance Interrupter (SEN09281P) is used to detect when the tailgate is open.
 * - Grove LED (COM04054P) is used to indicate to the operator of the vehicle the tailgate is open.
 *
 * @req upm_ublox6.jar
 * @req upm_i2clcd.jar
 * @req upm_rfr359f.jar
 * @req upm_rpr220.jar
 * @req upm_grove.jar
 *
 * @date 19/08/2015
 */

public class VehicleFleetTracker {

    static final String NODE = "localhost";
    static final int SERVICE = 41234;

    static final int bufferLength = 256;

    static boolean shouldRun = true;
    static boolean displayTime = true;

    static Jhd1313m1 lcd;
    static DatagramSocket client;
    static Ublox6 gps;
    static RFR359F distanceInterrupter;
    static RPR220 reflectiveSensor;
    static GroveLed led;

    static Runnable shutdown = new Runnable() {

        @Override
        public void run() {
            shouldRun = false;
            System.out.println("Exiting...");
            System.out.println("Please wait up to 10secs for threads to finish.");
        }
    };

    /**
     * @brief Write data to the cloud
     * Note: you have to register components prior.
     * E.g. using "iotkit-admin register <comp_name> <catalogid>" or via iotkit-agent
     */
    static void writeDataToCloud(String component, String data) {

        /* Create a udp connection for iotkit-agent to communicate over */
        if (client == null)
            try {
                client = new DatagramSocket(new InetSocketAddress(NODE, SERVICE));
            } catch (SocketException e) {
                System.err.println("Connection failed");
                e.printStackTrace();
                shouldRun = false;
                System.exit(1);
            }

        /*
         * Create the string to send to the cloud
         * eg. {"n":"gpsv1","v":"45.550953, -122.968719"}
         */
        String message = "{\"n\":\"" + component + "\",\"v\":\"" + data + "\"}\n";
        DatagramPacket packet = new DatagramPacket(message.getBytes(),
                message.getBytes().length);
        try {
            client.send(packet);
        } catch (IOException e) {
            System.err.println("Cannot write to cloud");
            e.printStackTrace();
        }
    }

    static Runnable gpsRunnable = new Runnable() {

        float convertRawCoords(String rawCoord, String cardinalDirection) {
            float ret = 0.0f;
            float direction = 1;
            float fRawCoord;
            try {
                fRawCoord = Float.valueOf(rawCoord);
            } catch (NumberFormatException e) {
                return 0;
            }

            /* If direction is either South or West, coordinate needs to be negative */
            if (cardinalDirection == "S" || cardinalDirection == "W")
                direction = -1;

            /* Get degrees, eg: rawCoord = 4532.5103 => 45 */
            float degrees = (float)((int)(fRawCoord / 100));


            /* Get minutes, eg: rawCoord = 4532.5103 => 32 */
            float minutes = (float) ((int)(fRawCoord - degrees * 100));  //32

            /* Get seconds, eg: rawCoord = 4532.5103 => 51.03 */
            float seconds = (fRawCoord - degrees * 100 - minutes); // .5103
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
        void parseGpsData(String nmeaBuffer, int result) {
            String[] vecStr;
            String strCloudComponent = "gpsv1";
            String strCloudData;
            String strLatitude;
            String strLatitudeDir;
            String strLongitude;
            String strLongitudeDir;

            for (String strGpsLine: nmeaBuffer.split("\n")) {

                /* if we have an empty line, just continue with next iteration of loop */
                if (strGpsLine.isEmpty())
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
                vecStr = strGpsLine.split(",");

                /* We're only interested in the $GPGGA entries */
                if (vecStr[0] == "$GPGGA") {
                    strCloudData = "";
                    strLatitude = vecStr[2]; /* Latitude */
                    strLatitudeDir = vecStr[3]; /* Cardinal direction */
                    strLongitude = vecStr[4]; /* Longitude */
                    strLongitudeDir = vecStr[5]; /* Cardinal direction */

                    /* Convert raw latitude/longitude data to degree coordinates, eg. 45.2345 */
                    strCloudData += Float.toString(convertRawCoords(strLatitude, strLatitudeDir));
                    strCloudData += ", ";
                    strCloudData += Float.toString(convertRawCoords(strLongitude, strLongitudeDir));
                    writeDataToCloud(strCloudComponent, strCloudData);
                }
            }
        }

        @Override
        public void run() {
            String nmeaBuffer = new String(new char[bufferLength]);
            /* Make sure port is initialized properly.  9600 baud is the default. */
            if (!gps.setupTty()) {
                System.err.println("Failed to setup tty port parameters");
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
                if (gps.dataAvailable()) {
                    byte[] buffer = new byte[bufferLength];
                    int result = gps.readData(buffer);

                    /* Result will be non-zero if we have acquired data from sensor */
                    if (result > 0) {
                        /* Write Gps sensor data to the cloud */
                        try {
                            nmeaBuffer = new String(buffer, "UTF-8");
                        } catch (UnsupportedEncodingException e) {
                            System.err.println("Wrong encoding");
                            e.printStackTrace();
                            break;
                        }
                        parseGpsData(nmeaBuffer, result);
                    }

                    if (result < 0) {
                        /* Some sort of read error occurred */
                        System.err.println("Port read error.");
                        break;
                    }
                    continue;
                }
                try {
                    Thread.sleep(10000); // 10 secs
                } catch (InterruptedException e) {
                    System.err.println("Sleep interrupted: " + e.toString());
                }
            }
        }
    };

    static Runnable distanceRunnable = new Runnable() {

        /**
         * @brief Distance interrupter check - mounted on rear bumper of vehicle to check if close to an object when backing up
         */
        public void run() {
            while (shouldRun) {
                if (distanceInterrupter.objectDetected()) {
                    /**
                     * An object was detected by the sensor
                     * Stop displaying time on the lcd
                     * Write line to lcd to notify operator to stop
                     */
                    displayTime = false;
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.write("Within 4\" - STOP!!!");
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                } else {
                    /**
                     * Display time when distance interrupter does not detect an object
                     */
                    displayTime = true;
                }
            }
        }
    };

    static Runnable reflectiveRunnable = new Runnable() {

        /**
         * @brief Reflective sensor check - used to check if the tailgate of the vehicle is open
         */
        @Override

        public void run() {
            boolean writeOnce = true;
            String strCloudComponent = "reflectorv1";
            String strCloudData = "true";

            while (shouldRun) {
                if (reflectiveSensor.blackDetected()) {
                    /**
                     * The reflective sensor has been triggered
                     * Turn on led to indicate tailgate is open
                     */
                    led.on();

                    /* Write data to cloud each time the tailgate opens */
                    if (writeOnce) {
                        writeDataToCloud(strCloudComponent, strCloudData);
                        writeOnce = false;
                    }
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                } else {
                    led.off();
                    writeOnce = true;
                }
            }
        }};

        static Runnable timeRunnable = new Runnable() {

            @Override
            public void run() {
                while(shouldRun) {
                    if (displayTime) {
                        lcd.clear();
                        lcd.setCursor(0, 0);
                        ZonedDateTime time = ZonedDateTime.now();
                        lcd.write(time.format(DateTimeFormatter.RFC_1123_DATE_TIME));
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException e) {
                            System.err.println("Sleep interrupted: " + e.toString());
                        }
                    }
                }

            }
        };

        public static void main(String[] args) {
            // Register signal handler
            Runtime runtime = Runtime.getRuntime();
            runtime.addShutdownHook(new Thread(shutdown));

            //! [Interesting]
            /* create a Ublox6 GPS device on UART 0 (port UART on shield) */
            gps = new Ublox6(0);
            /* create a led instance on GPIO pin 5 (port D5 on shield) */
            led = new GroveLed(5);
            /* Create a lcd instance on I2C bus 0 (port ) */
            /* 0x62 RGB_ADDRESS, 0x3E LCD_ADDRESS */
            lcd = new Jhd1313m1(0, 0x3E, 0x62);
            /* Create a Grove IR Distance Interrupter on GPIO pin 2 (port D2 on shield) */
            distanceInterrupter = new RFR359F(2);
            /* Create a Grove IR Reflective Sensor on GPIO pin 4 (port D4 on shield) */
            reflectiveSensor = new RPR220(4);

            Thread timeThread = new Thread(timeRunnable);
            timeThread.start();
            Thread gpsThread = new Thread(gpsRunnable);
            gpsThread.start();
            Thread distanceThread = new Thread(distanceRunnable);
            distanceThread.start();
            Thread reflectiveThread = new Thread(reflectiveRunnable);
            reflectiveThread.start();

            try {
                timeThread.join();
                gpsThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            lcd.clear();
            led.off();

        }

}
