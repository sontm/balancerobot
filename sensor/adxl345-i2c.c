// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// ADXL345
// This code is designed to work with the ADXL345_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Accelorometer?sku=ADXL345_I2CS#tabs-0-product_tabset-2



// ADXL345 Module                     Raspberry Pi
//GND  --------------------------   GND
//3.3V  --------------------------   3.3V
//SCL0 ---------------------------  SCL
//SDA0 --------------------------   SDA
// CS ----------------------------  3.3V
// SDO  --------------------------   GND

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int gFile;
int adxl345_i2c_init()
{
	int ret = 0;

	char *bus = "/dev/i2c-1";
	if ((gFile = open(bus, O_RDWR)) < 0) 
	{
		printf("[ERR] Failed to open the bus. \n");
		return 1;
	}
	// Get I2C device, ADXL345 I2C address is 0x53(83)
	if (ioctl(gFile, I2C_SLAVE, 0x53)< 0) {
		printf("[ERR] Cannot get bus access \n");
		return 2;
	}

	// Select Bandwidth rate register(0x2C)
	// Normal mode, Output data rate = 100 Hz(0x0A)
	char config[2]={0};
	config[0] = 0x2C;
	config[1] = 0x0A;
	int rW = write(gFile, config, 2);
	if (rW < 0) {
		printf("[ERR] Write Bandwidth failed \n");
		return 3;
	}
	// Select Power control register(0x2D)
	// Auto-sleep disable(0x08)
	config[0] = 0x2D;
	config[1] = 0x08;
	rW = write(gFile, config, 2);
	if (rW < 0) {
		printf("[ERR] Write Power control fail \n");
		return 3;
	}
	// Select Data format register(0x31)
	// Self test disabled, 4-wire interface, Full resolution, range = +/-4g(0x09)
	config[0] = 0x31;
	config[1] = 0x09;
	rW = write(gFile, config, 2);
	if (rW < 0) {
		printf("[ERR] Write Data Format fail \n");
		return 3;
	}
	sleep(1);

	return 0;
}

int adxl345_i2c_getdata(int* x, int* y, int *z)
{
	int ret = 0;
	char x1, x2, y1, y2, z1, z2;

	char reg[1] = {0x32};
	write(gFile, reg, 1);
	int rRet = read(gFile, &x1, 1);
	if (rRet < 0) {
		return 1;
	}

	reg[0] = 0x33;
	write(gFile, reg, 1);
	rRet = read(gFile, &x2, 1);
	if (rRet < 0) {
		return 2;
	}

	reg[0] = 0x34;
	write(gFile, reg, 1);
	rRet = read(gFile, &y1, 1);
	if (rRet < 0) {
		return 3;
	}
	reg[0] = 0x35;
	write(gFile, reg, 1);
	rRet = read(gFile, &y2, 1);
	if (rRet < 0) {
		return 4;
	}

	reg[0] = 0x36;
	write(gFile, reg, 1);
	rRet = read(gFile, &z1, 1);
	if (rRet < 0) {
		return 5;
	}
	reg[0] = 0x37;
	write(gFile, reg, 1);
	rRet = read(gFile, &z2, 1);
	if (rRet < 0) {
		return 6;
	}

	*x = (int)(x2 << 8) + (int)x1;
	*y = (int)(y2 << 8) + (int)y1;
	*z = (int)(z2 << 8) + (int)z1;
	return 0;
}

int adxl345_final()
{
	close(gFile);
}
void main() 
{
	// Create I2C bus
	adxl345_i2c_init();
 	while(1) {
		int xAccl, yAccl, zAccl;
		adxl345_i2c_getdata(&xAccl, &yAccl, &zAccl);

		// Output data to screen
		printf("X: %d, Y: %d, Z: %d\n", xAccl, yAccl, zAccl);
		usleep(1000000);
	}
}
