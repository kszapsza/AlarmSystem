#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "system.hpp"
#include "keypad.hpp"

/**
 * Arduino simple alarm system
 * (developed on: Arduino Leonardo)
 *
 * (C) 2020 Karol Szapsza
 */

System sys;

void setup()
{
	sys.initLcd();
	sys.splashscreen();
	sys.disarm();

	Signals::bark();
	Signals::bootedUp();
}

void loop()
{
	const auto motion_read = digitalRead(motion_sensor);
	sys.pin_code.getPin();

	// Motion handling.
	if (motion_read == HIGH)
	{
		digitalWrite(leds::yellow, HIGH);

		if (sys.state == State::ARMED)
		{
			sys.waitForDisarm();
		}
	}
	else
	{
		digitalWrite(leds::yellow, LOW);
	}

	if (sys.state == State::ARMED
			&& digitalRead(buttons::disarm) == LOW)
	{
		if (sys.pin_code.checkPin())
		{
			sys.disarm();
		}
		else
		{
			sys.alarm();
		}
	}

	if (sys.state == State::DISARMED
			&& digitalRead(buttons::arm) == LOW && sys.pin_code.checkPin())
	{
		sys.waitForExit();
	}

	// Alarm state.
	while (sys.state == State::ALARM)
	{
		sys.alarm();
	}
}