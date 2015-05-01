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

/*
 * sample: UDP communication with iotkit-agent.
 * For TCP you would have to slightly modify socket parameters and bind to the socket
 * rather than connect. Also the transmitted data is would have to contain the length of each message.
 * Pls consult documentation for further information.
 * Note: you have to register components prior.
 * E.g. using "iotkit-admin register <comp_name> <catalogid>" or via iotkit-agent
 * node (host) and service (port) iotkit-agent is listening for UDP data
 * as defined in /etc/iotkit-agent/config.json
 */

#include "iotAgent.hpp"

using namespace std;

int UdpClient::connectUdp(const string node, const string service) {
	struct addrinfo hints;
	hints.ai_flags = 0;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	struct addrinfo *res;
	int err_code;
	err_code = getaddrinfo(node.c_str(), service.c_str(), &hints, &res);
	if (err_code != 0) {
		cerr << "getaddrinfo failed: " << gai_strerror(err_code) << endl;
		exit(EXIT_FAILURE);
	}

	struct addrinfo *p_ai = res;
	/* loop through the result list of addrinfo and
	 * try connecting each element until connection succeeds or all elements are tested */
	bool success = false;
	while (p_ai != NULL) {
		sfd = socket(p_ai->ai_family, p_ai->ai_socktype, IPPROTO_UDP);
		if (sfd == -1) { //try next if any left
			p_ai = p_ai->ai_next;
			continue;
		}
		if (connect(sfd, p_ai->ai_addr, p_ai->ai_addrlen) == 0) {
			success = true;
			break; // success
		}
		p_ai = p_ai->ai_next;
		close(sfd);
	}
	if (!success) {
		cerr << "couldn't create socket" << endl;
		return (-1);
	}

	freeaddrinfo(res);
	connected = true;
	return 0;
}

bool UdpClient::isConnected() {
	return connected;
}

int UdpClient::writeData(const string &data) {
	if (!connected)
		return -1;
	write(sfd, data.c_str(), data.length());
	return 0;
}

int UdpClient::writeData(const char* data) {
	if (!connected)
		return -1;
	writeData(string(data));
	return 0;
}

int UdpClient::writeData(const stringstream &data) {
	if (!connected)
		return -1;
	write(sfd, data.str().c_str(), data.str().length());
	return 0;
}
