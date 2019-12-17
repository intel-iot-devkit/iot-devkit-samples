/*
*  Copyright (c) 2018 Intel Corporation.
*  Licensed under the MIT license. See LICENSE file in the project root for full license information.
*/

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char**argv)
{
	ofstream outfile;
	outfile.open("/tmp/ubuntu/temp_out.txt");
	cout << "Hello, Cmake!\n";
	outfile << "argc = " << argc << endl;
	outfile << "app name: " << argv[0] << endl;
	outfile << "first arg: " << argv[1] << endl;
	outfile << "second arg: " << argv[2] << endl;
	cout << "argc = " << argc << endl;
	cout << "app name: " << argv[0] << endl;
	cout << "first arg: " << argv[1] << endl;
	cout << "second arg: " << argv[2] << endl;
	return 0;
}