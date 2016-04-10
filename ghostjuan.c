#pragma config(Sensor, S1,     Touch,          sensorEV3_Touch)
#pragma config(Sensor, S2,     Gyro,           sensorEV3_Gyro)
#pragma config(Sensor, S3,     Color,         sensorEV3_Color, modeEV3Color_RGB_Cpolor)
#pragma config(Sensor, S4,     Sonic,          sensorEV3_Ultrasonic)
#pragma config(Motor,  motorB,          motorLeft,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          motorRight,    tmotorEV3_Large, PIDControl, encoder)

int gyro1, gyro2, powL, powR, drift, x, y, xd, yd, r, gyroreset, a, timer, b, orientation, tsa, colorstop, BlameTheRAs;
int DaddelsGraspOfDayOfWeek, BlameTheSRAs, alignment;

void align()
{
	alignment = 0;
	while (SensorValue[Color] == 6 && alignment < 30)
	{
		motor[motorLeft] = powL/5;
		motor[motorRight] = powR;
		sleep(25);
		alignment = alignment + 1;
	}
	while (SensorValue[Color] == 6 && alignment < 60)
	{
		motor[motorLeft] = -powL/5;
		motor[motorRight] = -powR;
		sleep(25);
		alignment = alignment + 1;
	}
	while (SensorValue[Color] == 6 && alignment < 90)
	{
		motor[motorLeft] = powL;
		motor[motorRight] = powR/5;
		sleep(25);
		alignment = alignment + 1;
	}
	while (SensorValue[Color] == 6 && alignment < 120)
	{
		motor[motorLeft] = -powL;
		motor[motorRight] = -powR/5;
		sleep(25);
		alignment = alignment + 1;
	}
	return;
}

void straight()
{
	if (BlameTheSRAs == 0)
	{
		BlameTheRAs = 1500;
	}
	if (SensorValue[Color] != 6)
	{
		motor[motorLeft] = powL;
		motor[motorRight] = powR;
		sleep(25);
	}
	if (SensorValue[Color] == 6)
	{
		align();
	}
	return;
}

void right()
{
	BlameTheRAs = 2500;
	if (SensorValue[Gyro] - gyroreset > 89 && tsa == 0)
	{
		motor[motorLeft] = powL;
		motor[motorRight] = powR;
		sleep(1000);
		tsa = 1;
	}
	if (tsa == 1)
	{
		BlameTheSRAs = 1;
		straight();
	}
	if (SensorValue[Gyro] - gyroreset < 75 && tsa == 0)
	{
		motor[motorLeft] = powL;
		motor[motorRight] = -powR;
		sleep(10);
	}
	if (SensorValue[Gyro] - gyroreset > 74 && SensorValue[Gyro] - gyroreset < 90 && tsa == 0)
	{
		motor[motorLeft] = (95 - SensorValue[Gyro] + gyroreset)*powL/20;
		motor[motorRight] = -(95 - SensorValue[Gyro] + gyroreset)*powR/20;
		sleep(10);
	}
	return;
}

void left()
{
	BlameTheRAs = 2500;
	if (SensorValue[Gyro] - gyroreset < -89 && tsa == 0)
	{
		motor[motorLeft] = powL;
		motor[motorRight] = powR;
		sleep(1000);
		tsa = 1;
	}
	if (tsa == 1)
	{
		BlameTheSRAs = 1;
		straight();
	}
	if (SensorValue[Gyro] - gyroreset > -75 && tsa == 0)
	{
		motor[motorLeft] = -powL;
		motor[motorRight] = powR;
		sleep(10);
	}
	if (SensorValue[Gyro] - gyroreset < -74 && SensorValue[Gyro] - gyroreset > -90 && tsa == 0)
	{
		motor[motorLeft] = -(95 + SensorValue[Gyro] - gyroreset)*powL/20;
		motor[motorRight] = (95 + SensorValue[Gyro] - gyroreset)*powR/20;
		sleep(10);
	}
	return;
}

void backwards()
{
	BlameTheRAs = 4000;
	if (SensorValue[Gyro] - gyroreset > 177 && tsa == 0)
	{
		motor[motorLeft] = powL;
		motor[motorRight] = powR;
		sleep(700);
		tsa = 1;
	}
	if (tsa == 1)
	{
		BlameTheSRAs = 1;
		straight();
	}
	if (SensorValue[Gyro] - gyroreset < 155 && tsa == 0)
	{
		motor[motorLeft] = powL;
		motor[motorRight] = -powR;
		sleep(10);
	}
	if (SensorValue[Gyro] - gyroreset > 154 && SensorValue[Gyro] - gyroreset < 180 && tsa == 0)
	{
		motor[motorLeft] = (185 - SensorValue[Gyro] + gyroreset)*powL/30;
		motor[motorRight] = -(185 - SensorValue[Gyro] + gyroreset)*powR/30;
		sleep(10);
	}
	return;
}

void obeyorient()
{
	if (orientation%4 == 0)
	{
		BlameTheSRAs = 0;
		straight();
		return;
	}
	if (orientation%4 == 1)
	{
		right();
		return;
	}
	if (orientation%4 == 2)
	{
		backwards();
		return;
	}
	if (orientation%4 == 3)
	{
		left();
		return;
	}
}

task main()
{


	// Phase 1: Soft reset of the gyro with 2 second delay.  Indicated by solid red LED.
	// Goal is to get the gyro as stable as possible -- minimize drift.
	setLEDColor(ledRed);
	SensorType[Gyro]=sensorNone;
	wait1Msec(2000);
	SensorMode[Color] = modeEV3Color_Color;
	wait1Msec(3000);
	SensorMode[Color] = modeEV3Color_Color;
	SensorType[Gyro]=sensorEV3_Gyro;
	// Alternate version waits for 2000 ms here.

	// Phase 2: Measure gyro drift.  Indicated by flashing red LED.
	// Goal is to calculate drift rate so that it can be computationally removed in turn calculations.
	// This has not been tested properly, because Phase 1 appears to be a little too effective.
	// It would be easy enough to comment out Phase 1 and do the test; I just haven't had time yet.
	setLEDColor(ledRedFlash);
	sleep(1000);
	gyro1=SensorValue[Gyro];  // capture some gyro reading
	sleep(1000);
	gyro2=SensorValue[Gyro];  // capture gyro reading one second later
	drift=gyro2-gyro1;       // global variable "drift" measures gyro drift in degrees per second.

	// Conclusion
	setLEDColor(ledGreen);
	setLEDColor(ledOrangePulse);
	while(SensorValue[Touch]==0)
	{
		// Chillax until button press
		sleep(40);
	}
	setLEDColor(ledOff);
	setLEDColor(ledGreenFlash);

	nMotorEncoder[motorLeft] = 0;
	nMotorEncoder[motorRight] = 0;

	motor[motorLeft] = 25;
	motor[motorRight] = 25;
	sleep(5000);
	// Measure motor speeds.

	powL=50*nMotorEncoder[motorRight]/(nMotorEncoder[motorLeft]+nMotorEncoder[motorRight]);
	powR=50*nMotorEncoder[motorLeft]/(nMotorEncoder[motorLeft]+nMotorEncoder[motorRight]);
	// Use real motor speeds to prevent robot homosexuality. (robots must be straight)


	setLEDColor(ledOrangePulse);
	while(SensorValue[Touch]==0)
	{

		// Moar chillaxing is required.
		motor[motorLeft] = 0;
		motor[motorRight] = 0;
		sleep(40);
	}
	x = 0;
	y = 0;
	a = 0;
	b = 0;
	xd = 0;
	yd = 0;
	BlameTheRAs = 9001; // #YOLOSWAG BLAMING THE RAS IS SO AWESOME ITS OVER 9000
	BlameTheRAs = 5000; // Unfortunately though, if I do that my code doesn't work. (sadface)
	colorstop = 0;
	DaddelsGraspOfDayOfWeek = 0;
	orientation = 0;
	SensorValue[Gyro] = 0;
	gyroreset = SensorValue[Gyro];
	while(true)
	{
		if (x == 0 && y == 0 && a == 0)
		{
			setLEDColor(ledRed);
			clearTimer(T1);
			r = random(1);
			timer = time1[T1];
			SensorValue[Gyro] = 0;
			gyroreset = SensorValue[Gyro];
			colorstop = 0;
			tsa = 0;
			a = 1;
			orientation = DaddelsGraspOfDayOfWeek;
		}
		if (x == 0 && y == 0 && a == 1)
		{
			timer = time1[T1];
			if (r == 0 && colorstop == 0 && b == 0)
			{
				yd = yd + 1;
				DaddelsGraspOfDayOfWeek = 0;
				if (orientation%2 == 1)
				{
					orientation = orientation + 2;
					b = 1;
				}
				else
				{
					b = 1;
				}
			}
			if (r == 0 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 1 && colorstop == 0 && b == 0)
			{
				xd = xd + 1;
				DaddelsGraspOfDayOfWeek = 1;
				if (orientation%2 == 1)
				{
					orientation = orientation + 3;
					b = 1;
				}
				else
				{
					orientation = orientation + 1;
					b = 1;
				}
			}
			if (r == 1 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
		}
		if (x > 0 && x < 5 && y == 0 && a == 0)
		{
			setLEDColor(ledOrange);
			clearTimer(T1);
			r = random(2);
			timer = time1[T1];
			SensorValue[Gyro] = 0;
			gyroreset = SensorValue[Gyro];
			colorstop = 0;
			tsa = 0;
			a = 1;
			orientation = DaddelsGraspOfDayOfWeek;
		}
		if (x > 0 && x < 5 && y == 0 && a == 1)
		{
			timer = time1[T1];
			if (r == 0 && colorstop == 0 && b == 0)
			{
				xd = xd + 1;
				DaddelsGraspOfDayOfWeek = 1;
				if (orientation%2 == 1)
				{
					orientation = orientation + 3;
					b = 1;
				}
				else
				{
					orientation = orientation + 1;
					b = 1;
				}
			}
			if (r == 0 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 1 && colorstop == 0 && b == 0)
			{
				yd = yd + 1;
				DaddelsGraspOfDayOfWeek = 0;
				if (orientation%2 == 1)
				{
					orientation = orientation + 2;
					b = 1;
				}
				else
				{
					b = 1;
				}
			}
			if (r == 1 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 2 && colorstop == 0 && b == 0)
			{
				xd = xd - 1;
				DaddelsGraspOfDayOfWeek = 3;
				if (orientation%2 == 1)
				{
					orientation = orientation + 1;
					b = 1;
				}
				else
				{
					orientation = orientation + 3;
					b = 1;
				}
			}
			if (r == 2 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
		}
		if (x == 0 && y == 1 && a == 0)
		{
			setLEDColor(ledGreen);
			clearTimer(T1);
			r = random(2);
			timer = time1[T1];
			SensorValue[Gyro] = 0;
			gyroreset = SensorValue[Gyro];
			colorstop = 0;
			tsa = 0;
			a = 1;
			orientation = DaddelsGraspOfDayOfWeek;
		}
		if (x == 0 && y == 1 && a == 1)
		{
			timer = time1[T1];
			if (r == 0 && colorstop == 0 && b == 0)
			{
				xd = xd + 1;
				DaddelsGraspOfDayOfWeek = 1;
				if (orientation%2 == 1)
				{
					orientation = orientation + 3;
					b = 1;
				}
				else
				{
					orientation = orientation + 1;
					b = 1;
				}
			}
			if (r == 0 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 1 && colorstop == 0 && b == 0)
			{
				yd = yd + 1;
				DaddelsGraspOfDayOfWeek = 0;
				if (orientation%2 == 1)
				{
					orientation = orientation + 2;
					b = 1;
				}
				else
				{
					b = 1;
				}
			}
			if (r == 1 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 2 && colorstop == 0 && b == 0)
			{
				yd = yd - 1;
				DaddelsGraspOfDayOfWeek = 2;
				if (orientation%2 == 1)
				{
					b = 1;
				}
				else
				{
					orientation = orientation + 2;
					b = 1;
				}
			}
			if (r == 2 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
		}
		if (x == 0 && y == 2 && a == 0)
		{
			setLEDColor(ledRedPulse);
			clearTimer(T1);
			r = random(1);
			timer = time1[T1];
			SensorValue[Gyro] = 0;
			gyroreset = SensorValue[Gyro];
			colorstop = 0;
			tsa = 0;
			a = 1;
			orientation = DaddelsGraspOfDayOfWeek;
		}
		if (x == 0 && y == 2 && a == 1)
		{
			timer = time1[T1];
			if (r == 0 && colorstop == 0 && b == 0)
			{
				yd = yd - 1;
				DaddelsGraspOfDayOfWeek = 2;
				if (orientation%2 == 1)
				{
					b = 1;
				}
				else
				{
					orientation = orientation + 2;
					b = 1;
				}
			}
			if (r == 0 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 1 && colorstop == 0 && b == 0)
			{
				xd = xd + 1;
				DaddelsGraspOfDayOfWeek = 1;
				if (orientation%2 == 1)
				{
					orientation = orientation + 3;
					b = 1;
				}
				else
				{
					orientation = orientation + 1;
					b = 1;
				}
			}
			if (r == 1 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
		}
		if (x == 5 && y == 0 && a == 0)
		{
			setLEDColor(ledGreen);
			clearTimer(T1);
			r = random(1);
			timer = time1[T1];
			SensorValue[Gyro] = 0;
			gyroreset = SensorValue[Gyro];
			colorstop = 0;
			tsa = 0;
			a = 1;
			orientation = DaddelsGraspOfDayOfWeek;
		}
		if (x == 5 && y == 0 && a == 1)
		{
			timer = time1[T1];
			if (r == 0 && colorstop == 0 && b == 0)
			{
				yd = yd + 1;
				DaddelsGraspOfDayOfWeek = 0;
				if (orientation%2 == 1)
				{
					orientation = orientation + 2;
					b = 1;
				}
				else
				{
					b = 1;
				}
			}
			if (r == 0 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 1 && colorstop == 0 && b == 0)
			{
				xd = xd - 1;
				DaddelsGraspOfDayOfWeek = 3;
				if (orientation%2 == 1)
				{
					orientation = orientation + 1;
					b = 1;
				}
				else
				{
					orientation = orientation + 3;
					b = 1;
				}
			}
			if (r == 1 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
		}
		if (x > 0 && x < 5 && y == 1 && a == 0)
		{
			setLEDColor(ledOrange);
			clearTimer(T1);
			r = random(3);
			timer = time1[T1];
			SensorValue[Gyro] = 0;
			gyroreset = SensorValue[Gyro];
			colorstop = 0;
			tsa = 0;
			a = 1;
			orientation = DaddelsGraspOfDayOfWeek;
		}
		if (x > 0 && x < 5 && y == 1 && a == 1)
		{
			timer = time1[T1];
			if (r == 0 && colorstop == 0 && b == 0)
			{
				xd = xd + 1;
				DaddelsGraspOfDayOfWeek = 1;
				if (orientation%2 == 1)
				{
					orientation = orientation + 3;
					b = 1;
				}
				else
				{
					orientation = orientation + 1;
					b = 1;
				}
			}
			if (r == 0 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 1 && colorstop == 0 && b == 0)
			{
				yd = yd + 1;
				DaddelsGraspOfDayOfWeek = 0;
				if (orientation%2 == 1)
				{
					orientation = orientation + 2;
					b = 1;
				}
				else
				{
					b = 1;
				}
			}
			if (r == 1 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 2 && colorstop == 0 && b == 0)
			{
				xd = xd - 1;
				DaddelsGraspOfDayOfWeek = 3;
				if (orientation%2 == 1)
				{
					orientation = orientation + 1;
					b = 1;
				}
				else
				{
					orientation = orientation + 3;
					b = 1;
				}
			}
			if (r == 2 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 3 && colorstop == 0 && b == 0)
			{
				yd = yd - 1;
				DaddelsGraspOfDayOfWeek = 2;
				if (orientation%2 == 1)
				{
					b = 1;
				}
				else
				{
					orientation = orientation + 2;
					b = 1;
				}
			}
			if (r == 3 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
		}
		if (x > 0 && x < 5 && y == 2 && a == 0)
		{
			setLEDColor(ledOrange);
			clearTimer(T1);
			r = random(2);
			timer = time1[T1];
			SensorValue[Gyro] = 0;
			gyroreset = SensorValue[Gyro];
			colorstop = 0;
			tsa = 0;
			a = 1;
			orientation = DaddelsGraspOfDayOfWeek;
		}
		if (x > 0 && x < 5 && y == 2 && a == 1)
		{
			timer = time1[T1];
			if (r == 0 && colorstop == 0 && b == 0)
			{
				xd = xd + 1;
				DaddelsGraspOfDayOfWeek = 1;
				if (orientation%2 == 1)
				{
					orientation = orientation + 3;
					b = 1;
				}
				else
				{
					orientation = orientation + 1;
					b = 1;
				}
			}
			if (r == 0 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 1 && colorstop == 0 && b == 0)
			{
				yd = yd - 1;
				DaddelsGraspOfDayOfWeek = 2;
				if (orientation%2 == 1)
				{
					b = 1;
				}
				else
				{
					orientation = orientation + 2;
					b = 1;
				}
			}
			if (r == 1 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 2 && colorstop == 0 && b == 0)
			{
				xd = xd - 1;
				DaddelsGraspOfDayOfWeek = 3;
				if (orientation%2 == 1)
				{
					orientation = orientation + 1;
					b = 1;
				}
				else
				{
					orientation = orientation + 3;
					b = 1;
				}
			}
			if (r == 2 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
		}
		if (x == 5 && y == 1 && a == 0)
		{
			setLEDColor(ledGreen);
			clearTimer(T1);
			r = random(2);
			timer = time1[T1];
			SensorValue[Gyro] = 0;
			gyroreset = SensorValue[Gyro];
			colorstop = 0;
			tsa = 0;
			a = 1;
			orientation = DaddelsGraspOfDayOfWeek;
		}
		if (x == 5 && y == 1 && a == 1)
		{
			timer = time1[T1];
			if (r == 0 && colorstop == 0 && b == 0)
			{
				xd = xd - 1;
				DaddelsGraspOfDayOfWeek = 3;
				if (orientation%2 == 1)
				{
					orientation = orientation + 1;
					b = 1;
				}
				else
				{
					orientation = orientation + 3;
					b = 1;
				}
			}
			if (r == 0 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 1 && colorstop == 0 && b == 0)
			{
				yd = yd + 1;
				DaddelsGraspOfDayOfWeek = 0;
				if (orientation%2 == 1)
				{
					orientation = orientation + 2;
					b = 1;
				}
				else
				{
					b = 1;
				}
			}
			if (r == 1 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 2 && colorstop == 0 && b == 0)
			{
				yd = yd - 1;
				DaddelsGraspOfDayOfWeek = 2;
				if (orientation%2 == 1)
				{
					b = 1;
				}
				else
				{
					orientation = orientation + 2;
					b = 1;
				}
			}
			if (r == 2 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
		}
		if (x == 5 && y == 2 && a == 0)
		{
			setLEDColor(ledGreen);
			clearTimer(T1);
			r = random(1);
			timer = time1[T1];
			SensorValue[Gyro] = 0;
			gyroreset = SensorValue[Gyro];
			colorstop = 0;
			tsa = 0;
			a = 1;
			orientation = DaddelsGraspOfDayOfWeek;
		}
		if (x == 5 && y == 2 && a == 1)
		{
			timer = time1[T1];
			if (r == 0 && colorstop == 0 && b == 0)
			{
				yd = yd - 1;
				DaddelsGraspOfDayOfWeek = 2;
				if (orientation%2 == 1)
				{
					b = 1;
				}
				else
				{
					orientation = orientation + 2;
					b = 1;
				}
			}
			if (r == 0 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
			if (r == 1 && colorstop == 0 && b == 0)
			{
				xd = xd - 1;
				DaddelsGraspOfDayOfWeek = 3;
				if (orientation%2 == 1)
				{
					orientation = orientation + 1;
					b = 1;
				}
				else
				{
					orientation = orientation + 3;
					b = 1;
				}
			}
			if (r == 1 && colorstop == 0 && b == 1)
			{
				obeyorient();
			}
		}
		if ((SensorValue(Color) == 5 || SensorValue(Color) == 2) && timer < 7000 && timer > BlameTheRAs)
		{
			motor[motorLeft] = 0;
			motor[motorRight] = 0;
			sleep(10);
			colorstop = 1;
			timer = time1[T1];
		}
		if (timer > 6999)
		{
			a = 0;
			b = 0;
			x = xd;
			y = yd;
			colorstop = 0;
			clearTimer(T1);
			timer = time1[T1];
		}
	}
}
