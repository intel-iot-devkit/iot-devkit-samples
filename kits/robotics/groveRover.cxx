/*
 * Author: Mihai T Panu <mihai.tudor.panu@intel.com>
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

#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "grovemd.h"
#include "jhd1313m1.h"
#include "rfr359f.h"
#include "hmc5883l.h"
#include "grovevdiv.h"

using namespace std;
using namespace upm;

bool running = true;

// Handler for POSIX signals
void signalHandler(int signo)
{
  if (signo == SIGINT)
    running = false;
}

int main(int argc, char **argv)
{
  signal(SIGINT, signalHandler);
  // Instantiate an I2C Grove Motor Driver on default I2C bus
  GroveMD *motors = new GroveMD(GROVEMD_I2C_BUS, GROVEMD_DEFAULT_I2C_ADDR);

  // Main event loop
  while(running){
  
  }

  cout << "Exiting..." << endl;

  delete motors;
  return 0;
}
