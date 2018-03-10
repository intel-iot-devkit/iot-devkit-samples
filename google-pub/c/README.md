# Google Cloud MQTT Sample

## Introduction
This sample app demonstrates connecting to Google Cloud IoT Core and publishing a single message. This Sample is modified for ease of creating a new Intel(R) System Studio project.

## Hardware requirements
No special H/W requirements, as long as your device could be connected to the internet.

## Software requirements
This version of the sample has been tested on Ubuntu Linux.

## Setup
Refer to [this document](https://cloud.google.com/iot/docs/quickstart) on how to create deviceid, registryid, and projectid parameters as they are used in calculating the client ID used for connecting to the MQTT bridge.

# Building
To build the sample, you must first download, build, and install the following projects, you can either install from individual links or follow the below commands.

* [Jansson](https://github.com/akheron/jansson)
	git clone https://github.com/akheron/jansson
	cd jansson && cmake . && make && make install
* [libjwt](https://github.com/benmcollins/libjwt)
	add-apt-repository -y ppa:ben-collins/libjwt && apt-get update
	apt-get install -y libjwt0
* [OpenSSL](https://github.com/openssl/openssl)
* [paho.mqtt.c](https://github.com/eclipse/paho.mqtt.c)
	apt-get install -y gcc
	apt-get install -y libssl-dev
	curl -OJL https://github.com/eclipse/paho.mqtt.c/archive/v1.2.0.tar.gz
	tar xf paho.mqtt.c-1.2.0.tar.gz
	cd paho.mqtt.c-1.2.0
	prefix=/usr make -j4 install

After you have installed all the dependencies, you can build the sample as:

    gcc mqtt_ciotc.c -lssl -lcrypto -lpaho-mqtt3cs -ljwt -o mqtt_ciotc

# Running
After you have successfully built the sample, you are almost ready to run the sample.

## Setup your libraries:

    export LD_LIBRARY_PATH=/usr/local/lib
    export LD_RUN_PATH=/usr/local/lib

## Run the sample
    ./mqtt_ciotc <message> \
        --deviceid <your device id>\
        --region <e.g. us-central1>\
        --registryid <your registry id>\
        --projectid <your project id>\
        --ecpath <e.g. ./ec_private.pem>\
        --rootpath <e.g. ./roots.pem>

Provide the deviceid, registryid, and projectid parameters which you created earlier. The ecpath parameter should point to your private EC key created when you registered your device. The rootpath parameter specifies the roots.pem file that can be downloaded from https://pki.google.com/roots.pem. For example:

    wget https://pki.google.com/roots.pem

The following example demonstrates usage of the sample if my device ID is my-device-id, registry ID is my-registry, and project ID is blue-ocean-123.

    mqtt_ciotc "Hello world!" --deviceid my-device-id --registryid my-registry\
        --projectid blue-ocean-123

Create a new project on Intel(R) System Studio. After choosing the PubSub sample, it'll ask you for the endpoint and the certificate filenames. 
