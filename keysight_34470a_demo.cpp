/*
 * keysight_34470a_demo.cpp
 *
 *  Created on: Dec 28, 2017
 *      Author: drohrer
 */

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

using namespace std;

double read_usbtmc_meter(int file, char cmd[128], char result_str[4000])
{
	//cmd assumes you included \n
	int retval,en;
	double result;

	retval=write(file,cmd,strlen(cmd));
	if(retval==-1) {
		en=errno;
		printf("Error during write: %s\n",strerror(en));
	}
	retval=read(file,result_str,4000);
	if(retval==-1) {
		en=errno;
		printf("Error during read: %s\n",strerror(en));
		result = 9.99e9;
	}
	else {
		result_str[retval]=0;
		result = atof(result_str);
	}
	return result;
}

int write_usbtmc_meter(int file, char cmd[128])
{
	//cmd assumes you included \n
	int retval,en;

	retval=write(file,cmd,strlen(cmd));
	if(retval==-1) {
		en=errno;
		printf("Error during write: %s\n",strerror(en));
	}
	return retval;
}

int main( int argc, char *argv[] )
{
	time_t start_time, end_time;
	int loop_size = 0;
	double delta_time,time_per_meas;
	double result, max, min;

	int myfile;
	char buffer[4000];
	char cmd[128];
	char mydev[128] = "/dev/usbtmc"; //Use usbtmc0 for rpi, usbtmc1 for ubuntu 16.04

	//Handle Input Args, should be a single arg of integer 0 to 16
	if(argc != 2 && atoi(argv[1]) >-1 && atoi(argv[1]) <17)
	{
		printf("ERROR! Invalid Input Arguments Expected integer between 0 and 16\n");
		printf("example usage: ./keysight_34470a_demo 0\n");
		return 0;
	}
	else
	{
		if(atoi(argv[1]) >-1 && atoi(argv[1]) <17)
			strcat(mydev, argv[1]);
	}

	//Only need this line for the tinkerboard or other system where drive is not loaded by default
	//system("ls -la /dev/usbtmc*; sudo rmmod usbtmc; sudo insmod ~/linux-usbtmc/usbtmc.ko;");
	sprintf(cmd, "sudo chmod a+rw %s", mydev);
	system(cmd);
	printf("\n");

	//Open device usbtmc0, change this if running on ubuntu to usbtmc1
	myfile=open(mydev,O_RDWR);
	if(myfile==-1)
	{
		printf("ERROR! Could Not Open Device %s\n",mydev);
		return 0;
	}
	else
		printf("SUCCESS! Opened File %s for communication\n",mydev);

//TEST 0. READ Meter ID
	printf("\n************************************************************\n");
	sprintf(cmd, "*IDN?\n");
	read_usbtmc_meter(myfile, cmd, buffer);
	printf("ID: %s\n",buffer);

//TEST 1. USE READ Command with CONF 10V Range 0.3ppm Range (NPLC=10) SAMP=1
	printf("\n************************************************************\n");
	printf("BEGIN 100 MEASUREMENTS READ DCV 10V RANGE 0.3ppm RES SAMP=1\n");

	sprintf(cmd, "CONF:VOLT:DC 10,3E-7\n");
	write_usbtmc_meter(myfile, cmd);

	sprintf(cmd, "SAMP:COUN 1\n");
	write_usbtmc_meter(myfile, cmd);

	sprintf(cmd, "READ?\n");

	loop_size = 100;
	time(&start_time);
	printf("\nSTART TIME = %ld sec\n\n",start_time);

	min=1e9; max=-1e9; //Initialize min/max values

	//Loop the measurements
	for(int i=0; i<loop_size; i++)
	{
		//Read result from meter
		result = read_usbtmc_meter(myfile, cmd, buffer);

		printf("%4d of %4d READ DC: %6.9f\n", i+1, loop_size, result);
		if(result>max)
			max=result;
		if(result<min)
			min=result;
	}

	//Print some stats and results
	time(&end_time);
	delta_time = end_time-start_time;
	time_per_meas = delta_time/loop_size;
	printf("END 100 MEASUREMENTS READ DCV 10V RANGE 0.3ppm RES SAMP=1\n");
	printf("MAX   = %6.9f V\n",max);
	printf("MIN   = %6.9f V\n",min);
	printf("PK-PK = %6.9f V\n",max-min);
	printf("TOTAL TIME = %f sec\n",delta_time);
	printf("AVG TIME PER MEAS = %6.3f sec\n",time_per_meas);
	printf("************************************************************\n");

//TEST 2. USE READ Command with CONF 10V Range 0.3ppm Range (NPLC=10)
	printf("\n************************************************************\n");
	printf("BEGIN 10 MEASUREMENTS READ DCV 10V RANGE 0.3ppm RES SAMP=10\n");

	sprintf(cmd, "CONF:VOLT:DC 10,3E-7\n");
	write_usbtmc_meter(myfile, cmd);

	sprintf(cmd, "SAMP:COUN 10\n");
	write_usbtmc_meter(myfile, cmd);

	sprintf(cmd, "READ?\n");

	loop_size = 10;
	time(&start_time);
	printf("\nSTART TIME = %ld sec\n\n",start_time);

	min=1e9; max=-1e9; //Initialize min/max values

	//Loop the measurements
	for(int i=0; i<loop_size; i++)
	{
		//Read result from meter
		result = read_usbtmc_meter(myfile, cmd, buffer);
		//Print string instead of double since it contains 10 csv measurements
		printf("%4d of %4d READ DC: %s", i+1, loop_size, buffer);
	}

	//Print some stats and results
	time(&end_time);
	delta_time = end_time-start_time;
	time_per_meas = delta_time/loop_size;
	printf("END 10 MEASUREMENTS READ DCV 10V RANGE 0.3ppm RES SAMP=10\n");
	printf("TOTAL TIME = %f sec\n",delta_time);
	printf("AVG TIME PER MEAS = %6.3f sec\n",time_per_meas);
	printf("************************************************************\n");

//TEST 3. USE READ Command with CONF 10V Range 0.3ppm Range (NPLC=10) SAMP=1
	printf("\n************************************************************\n");
	printf("BEGIN 1000 MEASUREMENTS READ DCV 10V RANGE 5ppm RES SAMP=1\n");

	sprintf(cmd, "CONF:VOLT:DC 10,5E-6\n");
	write_usbtmc_meter(myfile, cmd);

	sprintf(cmd, "SAMP:COUN 1\n");
	write_usbtmc_meter(myfile, cmd);

	sprintf(cmd, "READ?\n");

	loop_size = 1000;
	time(&start_time);
	printf("\nSTART TIME = %ld sec\n\n",start_time);

	min=1e9; max=-1e9; //Initialize min/max values

	//Loop the measurements
	for(int i=0; i<loop_size; i++)
	{
		//Read result from meter
		result = read_usbtmc_meter(myfile, cmd, buffer);

		printf("%4d of %4d READ DC: %6.9f\n", i+1, loop_size, result);
		if(result>max)
			max=result;
		if(result<min)
			min=result;
	}

	//Print some stats and results
	time(&end_time);
	delta_time = end_time-start_time;
	time_per_meas = delta_time/loop_size;
	printf("END 1000 MEASUREMENTS READ DCV 10V RANGE 5ppm RES SAMP=1\n");
	printf("MAX   = %6.9f V\n",max);
	printf("MIN   = %6.9f V\n",min);
	printf("PK-PK = %6.9f V\n",max-min);
	printf("TOTAL TIME = %f sec\n",delta_time);
	printf("AVG TIME PER MEAS = %6.3f sec\n",time_per_meas);
	printf("************************************************************\n");

	close(myfile);

	return 0;
}
