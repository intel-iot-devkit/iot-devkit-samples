import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import mraa.Aio;
import mraa.mraa;
import mraa.Platform;

/*
 * Author: Thomas Lyet <thomas.lyet@intel.com>
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
 * @ingroup cloud 
 * @brief Intel(R) IoT cloud analytics 
 * 
 * Demonstrate how to continuously send data to the IoT Cloud Analytics
 * (https://dashboard.us.enableiot.com/)
 * Read an analog voltage value from an input pin using the MRAA library,
 * then send its value to the IoT Cloud Analytics.
 * Any sensor that outputs a variable voltage can be used with this example
 * code. Suitable ones in the Grove Starter Kit are the Rotary Angle Sensor,
 * Light Sensor, Sound Sensor, Temperature Sensor.
 * 
 * @web https://dashboard.us.enableiot.com
 * 
 * @hardware Analog sensor connected to pin A0 (Grove Base Shield Port A0)
 * 
 * @req mraa.jar
 *
 * @date 19/08/2015
 */

/*
 * Preliminary Step
 *
 * Follow the IoT Cloud Analytics Getting Started Guide:
 * http://www.intel.com/support/motherboards/desktop/sb/CS-035346.htm
 *
 * Please check if the iotkit-agent is active on your device via
 *  $ systemctl status iotkit-agent
 * If not, activate it with
 *  $ systemctl start iotkit-agent
 *
 * Check the date of your device! It is this date that will be registered
 * in IoT Cloud Analytics.
 */

public class IoTCloudAnalytics {

    /*
     * NODE (host) and SERVICE (port)
     * iotkit-agent is listening for UDP data
     * as defined in /etc/iotkit-agent/config.json
     */
    final static String NODE = "localhost";
    final static int SERVICE = 41234;

    /*
     * COMP_NAME is defined when a component is registered on the device
     *  $ iotkit-admin register ${COMP_NAME} ${CATALOG_ID}
     * In this example :
     *  $ iotkit-admin register temperature temperature.v1.0
     */
    final static String COMP_NAME = "temperature";

    public static void main(String[] args) {
        // check that we are running on Galileo or Edison
        Platform platform = mraa.getPlatformType();
        if (platform != Platform.INTEL_GALILEO_GEN1 &&
                platform != Platform.INTEL_GALILEO_GEN2 &&
                platform != Platform.INTEL_EDISON_FAB_C) {
            System.err.println("Unsupported platform, exiting");
            return;
        }

        // UdpClient class is wrapper for sending UDP data to iotkit-agent
        DatagramSocket client = null;
        try {
            client = new DatagramSocket();
            client.connect(InetAddress.getByName(NODE), SERVICE);
        } catch (SocketException | UnknownHostException e) {
            e.printStackTrace();
            return;
        }

        // create an analog input object from MRAA using pin A0
        Aio pin = new Aio(0);

        // loop forever sending the input value every second
        while (true) {
            float data = (float) pin.read();
            String str_data = "{\"n\":\"" + COMP_NAME + "\",\"v\":" + data + "}\n";
            DatagramPacket packet = new DatagramPacket(str_data.getBytes(),
                    str_data.getBytes().length);
            try {
                client.send(packet);
            } catch (IOException e) {
                e.printStackTrace();
                break;
            }

            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
        }
        client.close();
    }

}
