/*
 * Author: Matthias Hahn <matthias.hahnl@intel.com>
 * Copyright (c) 2014-2015 Intel Corporation.
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

#ifndef CPP_IOTKIT_AGENT
#define CPP_IOTKIT_AGENT

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

using namespace std;

static const string NODE = "localhost";
static const string SERVICE = "41234";

/**
 * @brief non failsafe communication (udp) communication. If no udp server is listening writes will be nops
 */
class UdpClient {
public:
	UdpClient() :
		connected(false), sfd(-1) {
	}
	;

	/**
	 * @brief tries to setup connection and returns 0. In case it fails returns -1
	 * @param address IP address
	 * @param port port
	 */
	int connectUdp(const string node, const string service);

	/**
	 * @brief sends data via udp in case connection is established. Otherwise it will just do nothing
	 * @returns 0 on success, -1 on failure
	 */
	int writeData(const string &data);

	/**
	 * @brief sends data via udp in case connection is established. Otherwise it will just do nothing
	 * @returns 0 on success, -1 on failure
	 */
	int writeData(const char* data);

	/**
	 * @brief sends data via udp in case connection is established. Otherwise it will just do nothing
	 * @returns 0 on success, -1 on failure
	 */
	int writeData(const stringstream &data);

	/**
	 * @brief returns true if connected
	 */
	bool isConnected();

private:
	bool connected;
	int sfd; // stream file descriptor
};

#endif
