#include "dynamixel.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>


void move_to_location(unsigned char id,
			unsigned char loc_h, unsigned char loc_l) {
	int connection = open_connection("/dev/ttyUSB0",B1000000);

	unsigned char cs = ~ ( id + 0x07 + 0x03 + 0x1e + loc_l + loc_h +
				0x30 + 0x00);

	unsigned char arr[] = { 0xff, 0xff, id, 0x07, 0x03, 0x1e, loc_l,
                                       loc_h, 0x30, 0x00, cs };

	int buff_len = 100;
	unsigned char buff[buff_len];

	int bytes_read = write_to_connection(connection,arr,11,buff,buff_len);

}

checkMoving(int motor, int connection) {				// returns a 1 if the motor is currently moving

	unsigned char cs = ~ ( motor + 0x04 + 0x02 + 0x2e + 0x01);

	unsigned char arr[] = { 0xff, 0xff, motor, 0x04, 0x02, 0x2e, 0x01, cs };

	int buff_len = 100;
	unsigned char buff[buff_len];

	int bytes_read = write_to_connection(connection,arr,8,buff,buff_len);
	return buff[5];
}


void wait_until_done() {
	int connection = open_connection("/dev/ttyUSB0",B1000000);
	while (checkMoving(1, connection) + checkMoving(2, connection) + checkMoving(3, connection) + checkMoving(4, connection) + checkMoving(5, connection) > 0) {
		usleep(10000);
	}
}

void moveMotor(int motor, int angle) {		// angle 0-1023
	//int connection = open_connection("/dev/ttyUSB0",B1000000);
	char b = angle;
	char a = angle >> 8;
	move_to_location(motor, a, b);
}


void moveToTower(int tower) {
	// Constants representing the angle of each motor for the first tower
	const int M1_T1 = 400;
	const int M1_T2 = 500;
	const int M1_T3 = 600;
	const int M2_ABOVETOWER = 400;
	const int M3_ABOVETOWER = 300;
	const int M4_ABOVETOWER = 250;

	moveMotor(2, M2_ABOVETOWER);
	moveMotor(3, M3_ABOVETOWER);
	moveMotor(4, M4_ABOVETOWER);
	switch (tower) {
		case 0:
			moveMotor(1, M1_T1);
			break;
		case 1:
			moveMotor(1, M1_T2);
			break;
		case 2:
			moveMotor(1, M1_T3);
	}
}

void grabBlock(int height, int grab) {			// grab is 1 for grab, 0 for place
	const int M5_OPEN = 500;
	const int M5_CLOSED = 300;
	if (grab == 1) moveMotor(5, M5_OPEN);
	switch (height) {
		case 4:
			moveMotor(2, 375);
			moveMotor(3, 300);
			moveMotor(4, 270);
			wait_until_done();
			if (grab == 1) moveMotor(5, M5_CLOSED);
			else  moveMotor(5, M5_OPEN);
			break;
		case 3:
			moveMotor(2, 350);
			moveMotor(3, 300);
			moveMotor(4, 290);
			wait_until_done();
			if (grab == 1) moveMotor(5, M5_CLOSED);
			else  moveMotor(5, M5_OPEN);
			break;
		case 2:
			moveMotor(2, 325);
			moveMotor(3, 300);
			moveMotor(4, 310);
			wait_until_done();
			if (grab == 1) moveMotor(5, M5_CLOSED);
			else  moveMotor(5, M5_OPEN);
			break;
		case 1:
			moveMotor(2, 300);
			moveMotor(3, 300);
			moveMotor(4, 330);
			wait_until_done();
			if (grab == 1) moveMotor(5, M5_CLOSED);
			else  moveMotor(5, M5_OPEN);
			break;
	}

}


void moveBlock(int tower, int height, int grab) {
		moveToTower(tower);
		wait_until_done();
		grabBlock(height, grab);
		wait_until_done();
		moveToTower(tower);
		wait_until_done();
}

void allign(int numBlocks) {
	moveToTower(0);
	grabBlock(numBlocks, 0);
	printf("Allign the tower and press enter.\n");
	char enter = 0;
	while (enter != '\r' && enter != '\n') { enter = getchar(); }
}

void towerOfHanoi(int n, char fromTower, char toTower, char otherTower, int towerHeights[]) {
	int i;
	for (i = 0; i < 3; i++) {
		printf("%d", towerHeights[i]);
	}
    if (n == 1)
    {
		moveBlock(fromTower, towerHeights[fromTower], 1);
		towerHeights[fromTower]--;
		moveBlock(toTower, towerHeights[toTower]+1, 0);
		towerHeights[toTower]++;
        return;
    }
    towerOfHanoi(n-1, fromTower, otherTower, toTower, towerHeights);
	moveBlock(fromTower, towerHeights[fromTower], 1);
	towerHeights[fromTower]--;
	moveBlock(toTower, towerHeights[toTower]+1, 0);
	towerHeights[toTower]++;
    towerOfHanoi(n-1, otherTower, toTower, fromTower, towerHeights);
}


int main(int argc, char* argv[]) {
	int n = 4; // Number of blocks
	allign(n);
	int towerHeights[n];
	int i;
	for (i = 0; i < n; i++) {
		towerHeights[i] = 0;
	}
	towerHeights[0] = n;
    towerOfHanoi(n, 0, 2, 1, towerHeights);


	/*moveBlock(1, 3, 1);	// Green to 3
	moveBlock(3, 1, 0);
	moveBlock(1, 2, 1);	// Yellow to 2
	moveBlock(2, 1, 0);
	moveBlock(3, 1, 1); // Green to 2
	moveBlock(2, 2, 0);
	moveBlock(1, 1, 1); // Red to 3
	moveBlock(3, 1, 0);
	moveBlock(2, 2, 1); // Green to 1
	moveBlock(1, 1, 0);
	moveBlock(2, 1, 1); // Yellow to 3
	moveBlock(3, 2, 0);
	moveBlock(1, 1, 1); // Green to 3
	moveBlock(3, 3, 0);*/

	return 0;

}
