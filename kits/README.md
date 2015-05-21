IoT Starter Sensor Kits
==============

This section holds examples showing you how to combine the sensors from the
starter kits together into more elaborate projects. Some of these will also
involve advanced connectivity and data transfers to and from the IoT device
either through cloud platforms and APIs, dedicated web servers or bluetooth.

Transportation & Safety
-------

This is a C++ project showing how the Intel Edison and the Intel IoT Analytics
cloud website can be used to manage fleet vehicles for a delivery company. The
sensors used are:
* A GPS sensor to gather position data of a vehicle and send it to the cloud.
* An Infrared Reflective Sensor to simulate if the back door on the truck is
  open, indicating the driver is making a delivery, this data is also sent to
  the cloud.
* An IR Distance Interrupter placed on the rear bumper of the vehicle, with the
  LCD visible to the driver, to indicate when the vehicle is getting close to a
  loading dock when backing up.

Robotics
-------

This example uses sensors from the Starter & Robotics kits mounted on a Seeed
4WD Hercules Mobile Platform to illustrate the concept of a smart vehicle.
Other frames are also suitable for the project as long as all the sensors are
available. The robot is controlled via string commands with the idea of using
the process as the child of another front-end service (e.g. webserver). The
sensor used are:
* I2C Motor Shield and DC Motors as the driving components.
* IR Distance Interrupters for avoiding collisions.
* A digital compass for determining orientation with possible use in way point
  based self driving.
* An I2C LCD for displaying information and telemetry data.
