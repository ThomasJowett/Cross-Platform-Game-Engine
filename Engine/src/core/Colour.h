#pragma once
#include "Utilities/Random.h"

enum class Colours
{
	BEIGE = 0,
	BLACK,
	BLUE,
	BROWN,
	CYAN,
	FOREST_GREEN,
	GREEN,
	GREY,
	INDIGO,
	KHAKI,
	LIME_GREEN,
	MAGENTA,
	MAROON,
	NAVY,
	OLIVE,
	ORANGE,
	PINK,
	PURPLE,
	RED,
	SILVER,
	TEAL,
	TURQUOISE,
	VIOLET,
	WHITE,
	YELLOW,

	RANDOM,
	NUM_COLOURS
};

class Colour
{
public:
	float r, g, b, a;

	Colour() { r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f; }
	Colour(float r, float g, float b, float a) :r(r), g(g), b(b), a(a) {}
	Colour(Colours colour)
	{
		SetColour(colour);
	}
	~Colour() = default;

	void SetColour(Colours colour)
	{
		switch (colour)
		{
		case Colours::BEIGE:		r = 0.96f, g = 0.96f, b = 0.86f, a = 1.0f;	break;
		case Colours::BLACK:		r = 0.00f, g = 0.00f, b = 0.00f, a = 1.0f;	break;
		case Colours::BLUE:			r = 0.00f, g = 0.00f, b = 1.00f, a = 1.0f;	break;
		case Colours::BROWN:		r = 0.64f, g = 0.16f, b = 0.16f, a = 1.0f;	break;
		case Colours::CYAN:			r = 0.00f, g = 1.00f, b = 1.00f, a = 1.0f;	break;
		case Colours::FOREST_GREEN:	r = 0.13f, g = 0.54f, b = 0.13f, a = 1.0f;	break;
		case Colours::GREEN:		r = 0.00f, g = 0.50f, b = 0.50f, a = 1.0f;	break;
		case Colours::GREY:			r = 0.40f, g = 0.40f, b = 0.40f, a = 1.0f;	break;
		case Colours::INDIGO:		r = 0.30f, g = 0.00f, b = 0.50f, a = 1.0f;	break;
		case Colours::KHAKI:		r = 0.94f, g = 0.90f, b = 0.54f, a = 1.0f;	break;
		case Colours::LIME_GREEN:	r = 0.00f, g = 1.00f, b = 0.00f, a = 1.0f;	break;
		case Colours::MAGENTA:		r = 1.00f, g = 0.00f, b = 1.00f, a = 1.0f;	break;
		case Colours::MAROON:		r = 0.50f, g = 0.00f, b = 0.00f, a = 1.0f;	break;
		case Colours::NAVY:			r = 0.00f, g = 0.00f, b = 0.50f, a = 1.0f;	break;
		case Colours::OLIVE:		r = 0.50f, g = 0.50f, b = 0.00f, a = 1.0f;	break;
		case Colours::ORANGE:		r = 1.00f, g = 0.64f, b = 0.50f, a = 1.0f;	break;
		case Colours::PINK:			r = 1.00f, g = 0.75f, b = 0.80f, a = 1.0f;	break;
		case Colours::PURPLE:		r = 0.50f, g = 0.00f, b = 0.50f, a = 1.0f;	break;
		case Colours::RED:			r = 1.00f, g = 0.00f, b = 0.00f, a = 1.0f;	break;
		case Colours::SILVER:		r = 0.75f, g = 0.75f, b = 0.75f, a = 1.0f;	break;
		case Colours::TEAL:			r = 0.00f, g = 0.50f, b = 0.50f, a = 1.0f;	break;
		case Colours::TURQUOISE:	r = 0.25f, g = 0.95f, b = 0.81f, a = 1.0f;	break;
		case Colours::VIOLET:		r = 0.93f, g = 0.50f, b = 0.93f, a = 1.0f;	break;
		case Colours::WHITE:		r = 1.00f, g = 1.00f, b = 1.00f, a = 1.0f;	break;
		case Colours::YELLOW:		r = 1.00f, g = 1.00f, b = 0.00f, a = 1.0f;	break;
		case Colours::RANDOM:
			r = Random::Float(), g = Random::Float(), b = Random::Float(), a = 1.0f; break;

		default:					r = 0.00f, g = 0.00f, b = 0.00f, a = 1.0f;	break;
		}
	}
};