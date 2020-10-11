//
// Created by kszap on 10.10.2020.
//

#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "keypad.hpp"
#include "pinouts.hpp"
#include "pincode.hpp"
#include "signals.hpp"

//////////////////////////////////////////////////
// ALARM SYSTEM CLASS                           //
//////////////////////////////////////////////////

enum class State
{
	WAIT_FOR_EXIT, ARMED, WAIT_FOR_DISARM, DISARMED, ALARM
};

class PinCode;

class System
{
public:
	LiquidCrystal_I2C lcd;
	Keypad4x3 keypad;

	// Signalisation (visual/acoustic) class.
	Signals signals;

	PinCode pin_code;

	// Current system state.
	State state;

	uint32_t milliseconds_for_exit;
	uint32_t milliseconds_for_disarm;

	// Beware, that pin no. 2 is SDA and pin no. 3 is SCL
	// and they are already occupied by I2C LCD!!!

	System()
			:lcd(0x27, 16, 2), keypad(8, 10, 6, 9, 4, 5, 7), pin_code(lcd, keypad),
			 state(State::DISARMED), milliseconds_for_exit(30000), milliseconds_for_disarm(20000)
	{
		pinMode(leds::red, OUTPUT);
		pinMode(leds::green, OUTPUT);
		pinMode(leds::yellow, OUTPUT);

		pinMode(buttons::arm, INPUT_PULLUP);
		pinMode(buttons::disarm, INPUT_PULLUP);

		pinMode(buzzer, OUTPUT);
		pinMode(motion_sensor, INPUT);
	}

	void initLcd()
	{
		lcd.begin(16, 2);
		lcd.init();
		lcd.backlight();
		lcd.clear();
	}

	void splashscreen()
	{
		lcd.clear();
		lcd.setCursor(0, 0);

		lcd.print("* ALARM SYSTEM *");
		lcd.setCursor(0, 1);
		lcd.print("(c) 2020, DJF SA");

		delay(1000);

		lcd.clear();
		lcd.setCursor(0, 0);
	}

	void waitForDisarm()
	{
		const uint32_t start_time = millis();
		state = State::WAIT_FOR_DISARM;

		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("ARM: TIME=");

		while ((millis() - start_time < milliseconds_for_disarm)
				&& state == State::WAIT_FOR_DISARM)
		{
			// Keep tracking motion in inner loop and turn on LED if necessary.
			if (const auto motion_read = digitalRead(motion_sensor); motion_read == HIGH)
				digitalWrite(leds::yellow, HIGH);
			else
				digitalWrite(leds::yellow, LOW);

			const uint32_t elapsed_secs = (millis() - start_time) / 1000;
			const uint32_t remaining_secs = (milliseconds_for_disarm / 1000) - elapsed_secs;

			lcd.setCursor(10, 0);
			lcd.print("0:");
			lcd.setCursor(12, 0);

			if (remaining_secs >= 10)
			{
				lcd.print(remaining_secs);
			}
			else
			{
				lcd.print('0');
				lcd.setCursor(13, 0);
				lcd.print(remaining_secs);
			}

			signals.countdown();
			pin_code.getPin();

			if (digitalRead(buttons::disarm) == LOW)
			{
				if (pin_code.checkPin())
					disarm();
				else
					alarm();
			}
		}

		// Raise an alarm if alarm was not disarmed in given time
		// – when the time is over, raise an alarm.
		if (state == State::WAIT_FOR_DISARM)
			state = State::ALARM;
	}

	void disarm()
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("    DISARMED    ");

		Signals::blink(leds::green);
		Signals::bark();

		digitalWrite(leds::red, LOW);

		state = State::DISARMED;
	}

	void waitForExit()
	{
		const uint32_t start_time = millis();
		state = State::WAIT_FOR_EXIT;

		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("ARM: EXIT=");

		while ((millis() - start_time < milliseconds_for_exit)
				&& state == State::WAIT_FOR_EXIT)
		{
			// Keep tracking motion in inner loop and turn on LED if necessary.
			if (const auto motion_read = digitalRead(motion_sensor); motion_read == HIGH)
				digitalWrite(leds::yellow, HIGH);
			else
				digitalWrite(leds::yellow, LOW);

			const uint32_t elapsed_secs = (millis() - start_time) / 1000;
			const uint32_t remaining_secs = (milliseconds_for_exit / 1000) - elapsed_secs;

			lcd.setCursor(10, 0);
			lcd.print("0:");
			lcd.setCursor(12, 0);

			if (remaining_secs >= 10)
			{
				lcd.print(remaining_secs);
			}
			else
			{
				lcd.print('0');
				lcd.setCursor(13, 0);
				lcd.print(remaining_secs);
			}

			pin_code.getPin();
			signals.countdown();

			if (digitalRead(buttons::disarm) == LOW)
			{
				if (pin_code.checkPin())
					disarm();
				else
					alarm();
			}
		}

		// Arm ONLY if user DID NOT disarmed during exiting time!
		if (state == State::WAIT_FOR_EXIT)
			arm();
	}

	void arm()
	{
		state = State::ARMED;
		Signals::bark();

		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("     ARMED      ");
	}

	void alarm()
	{
		state = State::ALARM;
		lcd.setCursor(0, 0);
		lcd.print("     ALARM!     ");

		signals.alarm();
		pin_code.getPin();

		if (digitalRead(buttons::disarm) == LOW
				&& pin_code.checkPin())
		{
			disarm();
		}
	}
};