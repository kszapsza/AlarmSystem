//
// Created by kszap on 10.10.2020.
//

#pragma once
#include <Arduino.h>

//////////////////////////////////////////////////
// PIN DEFINITIONS                              //
//////////////////////////////////////////////////

struct leds
{
	static constexpr uint8_t red = 11;
	static constexpr uint8_t green = 12;
	static constexpr uint8_t yellow = A3;
};

struct buttons
{
	static constexpr uint8_t arm = 1;
	static constexpr uint8_t disarm = 0;
};

static constexpr uint8_t buzzer = A5;
static constexpr uint8_t motion_sensor = A4;
