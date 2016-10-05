/*
 * Library to connect and control EV3 BRICK
 * Zlin University Tomas Bata
 * date: 10/05/2016
 * Author: Miguel Ángel Torres
 */

#include <ev3.h>
#include <ev3_light.h>
#include <brick.h>
#include <stdio.h>

#define SPEED_LINEAR    75	/* Motor speed for linear motion, in percents */
#define SPEED_CIRCULAR  50	/* ... for circular motion */
#define MOTOR_LEFT     OUTC
#define MOTOR_RIGHT    OUTB
#define MOTOR_BOTH     ( MOTOR_LEFT | MOTOR_RIGHT )

void move_robot(char key);
void connect();
void off_robot();
void stop_robot();
void light_red();
void light_green();
void light_orange();
void light_off();
void drive_robot();
void light(int color);

enum {
	STOP,
	FORTH,
	BACK,
	LEFT,
	RIGHT,
};

enum {
	RED,
	GREEN,
	ORANGE,
};

int max_speed;			/* Motor maximal speed (will be detected) */
int command = STOP;		/* Command for `drive` coroutine */

 /* Function to initialize the brick */
int init(void)
{
	light_off();

	if (tacho_is_plugged(MOTOR_BOTH, TACHO_TYPE__NONE_)) {	/* any type of motor */
		max_speed = tacho_get_max_speed(MOTOR_LEFT, 0);
		tacho_reset(MOTOR_BOTH);
	} else {
		printf("Please, plug LEFT motor in C port,\n"
		       "RIGHT motor in B port and try again.\n");
		/* Inoperative without motors */
		return (0);
	}
	return (1);
}

/* Function to control the robot */
void drive_robot()
{
	int speed_linear = max_speed * SPEED_LINEAR / 100;
	int speed_circular = max_speed * SPEED_CIRCULAR / 100;

	switch (command) {

	case STOP:
		tacho_stop(MOTOR_BOTH);
		break;

	case FORTH:
		tacho_set_speed_sp(MOTOR_BOTH, -speed_linear);
		tacho_run_forever(MOTOR_BOTH);
		break;

	case BACK:
		tacho_set_speed_sp(MOTOR_BOTH, speed_linear);
		tacho_run_forever(MOTOR_BOTH);
		break;

	case LEFT:
		tacho_set_speed_sp(MOTOR_LEFT, speed_circular);
		tacho_set_speed_sp(MOTOR_RIGHT, -speed_circular);
		tacho_run_forever(MOTOR_BOTH);
		break;

	case RIGHT:
		tacho_set_speed_sp(MOTOR_LEFT, -speed_circular);
		tacho_set_speed_sp(MOTOR_RIGHT, speed_circular);
		tacho_run_forever(MOTOR_BOTH);
		break;
	}
}

/* Connect the brick to the pc */
void connect()
{
#ifndef __ARM_ARCH_4T__
	ev3_brick_addr = "10.42.0.90";
#endif

	ev3_init();
	ev3_sensor_init();
	ev3_tacho_init();

}

/* Function to move the robot any direction */
void move_robot(char key)
{
	if (key == 's') {
		command = BACK;
		light_off();
		light(RED);
	} else if (key == 'w') {
		command = FORTH;
		light_off();
		light(GREEN);
	} else if (key == 'd') {
		command = LEFT;
		light_off();
	} else if (key == 'a') {
		command = RIGHT;
		light_off();
	}
	drive_robot();
}

/* Function to stop robot */
void stop_robot()
{
	command = STOP;
	light_off();
	light(ORANGE);
	drive_robot();
}

/* Function to poweroff brick EV3 */
void off_robot()
{
	printf("Bye bye, the brick is powering off \n");
	ev3_poweroff();
}

/* Switch on light */
void light(int color)
{
	switch (color) {

	case RED:
		set_light(LIT_LEFT, LIT_RED);
		set_light(LIT_RIGHT, LIT_RED);
		break;

	case GREEN:
		set_light(LIT_LEFT, LIT_GREEN);
		set_light(LIT_RIGHT, LIT_GREEN);
		break;

	case ORANGE:
		set_light(LIT_LEFT, LIT_AMBER);
		set_light(LIT_RIGHT, LIT_AMBER);
		break;
	}

}

/* Switch off all light */
void light_off()
{
	set_light(LIT_LEFT, LIT_OFF);
	set_light(LIT_RIGHT, LIT_OFF);
}
