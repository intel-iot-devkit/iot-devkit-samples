/*
 * Author: Mihai Tudor Panu <mihai.tudor.panu@intel.com>
 * Copyright (c) 2018 Intel Corporation
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
package iotdk.example;

import mraa.Led;
import mraa.Platform;
import mraa.mraa;

public class Up2Leds {
    static {
        try {
            System.loadLibrary("mraajava");
        } catch (UnsatisfiedLinkError e) {
            System.err.println( "Native code library failed to load."
                + " See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
            System.exit(1);
        }
    }

    // Define led names and min/max brightness
    final static String[] ledNames = {"red", "green", "yellow", "blue"};
    final static int minBrightness = 0;
    final static int maxBrightness = 255;
    
    public static void consoleMessage(String str){
        System.err.println(str);
        try{
            Thread.sleep(10000);
        } catch (InterruptedException e)
        {
            System.err.println("Sleep interrupted: " + e.toString());
        }
    }

    public static void checkRoot(){
      String username = System.getProperty("user.name");
      System.out.println(username);
      String message = "This project uses Mraa I/O operations that require\n" +
              "'root' privileges, but you are running as non - root user.\n" +
              "Passwordless keys(RSA key pairs) are recommended \n" +
              "to securely connect to your target with root privileges. \n" +
              "See the project's Readme for more info.\n\n";
      if(!username.equals("root"))
      {
          consoleMessage(message);
      }
    }

    public static void main(String argv[]) throws InterruptedException {

        checkRoot();

        // Perform a basic platform and version check
        if (mraa.getPlatformType() != Platform.INTEL_UP2) {
            consoleMessage("This example is meant for the UP Squared board.\n"+
                        "Running it on different platforms will likely require code changes!");
        }

        if (mraa.getVersion().compareTo("v1.9.0") < 0) {
            consoleMessage("You need MRAA version 1.9.0 or newer to run this sample!");
            System.exit(1);
        }

        // Define and initialize our LEDs
        Led[] leds = new Led[ledNames.length];
        for (int i = 0; i < ledNames.length; i++) {
            leds[i] = new Led(ledNames[i]);
        }

        // Add a hook for handling keyboard interrupts and cleanup
        Runtime.getRuntime().addShutdownHook(new Thread()
        {
            @Override
            public void run()
            {
                System.out.println("Exiting...");
                for (Led led : leds) {
                    led.setBrightness(minBrightness);
                }
            }
        });

        // Main loop
        while (!Thread.interrupted()) {
            for (Led led : leds) {
                led.setBrightness(maxBrightness);
                Thread.sleep(200);
                led.setBrightness(minBrightness);
            }
            Thread.sleep(200);
        }
    }
}
