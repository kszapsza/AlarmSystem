//
// Created by kszap on 10.10.2020.
//

#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "keypad.hpp"

class PinCode
{
private:
	LiquidCrystal_I2C& lcd;
	Keypad4x3& keypad;

	// Correct PIN/password.
	const char pin[5] = "1234";
	char pin_attempt[5]{};

	size_t chars_entered{};

public:
	explicit PinCode(LiquidCrystal_I2C& lcd, Keypad4x3& keypad)
			:lcd(lcd), keypad(keypad)
	{
	}

	void getPin()
	{
		lcd.setCursor(chars_entered, 1);

		if (chars_entered < 4)
		{
			if (const auto key = keypad.detectKey(); key >= '0' && key <= '9')
			{
				lcd.print(key);

				pin_attempt[chars_entered] = key;
				Serial.println(pin_attempt);

				++chars_entered;
			}
		}

		if (chars_entered == 4)
		{
			pin_attempt[4] = NULL;
		}
	}

	bool checkPin()
	{
		const bool result = strcmp(pin, pin_attempt) == 0;

		for (size_t i{}; i < 5; ++i)
		{
			pin_attempt[i] = NULL;
		}

		chars_entered = 0;

		lcd.setCursor(chars_entered, 1);
		lcd.print("                ");

		Serial.println("Called checkPin()");
		Serial.print("\"pin\" was: ");
		Serial.print(pin);
		Serial.print("\"pin_attempt\" was: ");
		Serial.print(pin_attempt);
		Serial.println("Function returned: ");
		Serial.println(result ? "TRUE" : "FALSE");

		return result;
	}
};