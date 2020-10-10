//
// Created by kszap on 10.10.2020.
//

#pragma once
#include <Arduino.h>
#include "pinouts.hpp"

//////////////////////////////////////////////////
// ACOUSTIC AND OPTICAL SIGNALLING              //
//////////////////////////////////////////////////

class Signals
{
private:
	static void tripleBlink(const uint8_t led)
	{
		for (size_t i{}; i < 3; ++i)
		{
			digitalWrite(led, HIGH);
			delay(150);
			digitalWrite(led, LOW);
			if (i != 2) delay(150);
		}
	}

	// Timing variables for ALARM siren and flash

	uint32_t alarm_previous_time{};
	uint32_t alarm_interval{ 1000 };

	uint32_t alarm_led_previous_time{};
	uint32_t alarm_led_interval{ 50 };

	bool alarm_led_state{ LOW };

	// Timing variables for COUNTDOWN siren (wait for disarm)

	uint32_t countdown_previous_time{};
	uint32_t countdown_interval{ 2000 };

public:
	static void blink(const uint8_t led)
	{
		digitalWrite(led, HIGH);
		delay(150);
		digitalWrite(led, LOW);
	}

	static void bootedUp()
	{
		tripleBlink(leds::green);
	}

	static void bark()
	{
		tone(buzzer, 2000, 100);
	}

	// Alarm routine.
	void alarm()
	{
		const uint32_t current_time = millis();

		if (current_time - alarm_previous_time >= alarm_interval)
		{
			tone(buzzer, 560, 800);
			alarm_previous_time = current_time;
		}

		if (current_time - alarm_led_previous_time >= alarm_led_interval)
		{
			if (alarm_led_state == LOW)
			{
				digitalWrite(leds::red, HIGH);
				alarm_led_state = HIGH;
			}
			else
			{
				digitalWrite(leds::red, LOW);
				alarm_led_state = LOW;
			}

			alarm_led_previous_time = current_time;
		}
	}

	void countdown()
	{
		const uint32_t current_time = millis();

		if (current_time - countdown_previous_time >= countdown_interval)
		{
			tone(buzzer, 4000, 300);
			countdown_previous_time = current_time;
		}
	}
};