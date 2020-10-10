//
// Created by kszap on 09.10.2020.
//

#pragma once
#include <Arduino.h>

//////////////////////////////////////////////////
// 4x3 KEYPAD CLASS                             //
//////////////////////////////////////////////////

class Keypad4x3
{
private:
	char previous_key = '?';

public:
	uint8_t cols[3]{};
	uint8_t rows[4]{};

	char keys[4][3] = {{ '1', '2', '3' }, { '4', '5', '6' }, { '7', '8', '9' }, { '*', '0', '#' }};

	Keypad4x3(const uint8_t col_0, const uint8_t col_1, const uint8_t col_2,
			const uint8_t row_0, const uint8_t row_1, const uint8_t row_2, const uint8_t row_3)
	{
		cols[0] = col_0;
		cols[1] = col_1;
		cols[2] = col_2;

		rows[0] = row_0;
		rows[1] = row_1;
		rows[2] = row_2;
		rows[3] = row_3;

		for (uint8_t col : cols)
		{
			pinMode(col, INPUT_PULLUP);
		}

		for (uint8_t row : rows)
		{
			pinMode(row, INPUT);
		}
	}

	[[nodiscard]] char detectKey()
	{
		uint8_t row_index{};
		uint8_t col_index;

		for (uint8_t row : rows)
		{
			pinMode(row, OUTPUT);
			digitalWrite(row, LOW);

			col_index = 0;

			for (uint8_t col : cols)
			{
				if (digitalRead(col) == LOW)
				{
					const char detected_key = keys[row_index][col_index];

					// Don't forget to switch off output mode, otherwise when button is holded column
					// will still be grounded and it will always show first element of column held!
					pinMode(row, INPUT);

					if (previous_key == detected_key)
					{
						return '?';
					}
					else
					{
						previous_key = detected_key;
						return detected_key;
					}
				}

				++col_index;
			}

			pinMode(row, INPUT);
			++row_index;
		}

		previous_key = '?';
		return '?';
	}
};