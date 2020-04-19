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
	Colour(int hexValue)
	{
		SetColour(hexValue);
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

	// Hex Value in the form 0xRRGGBBAA
	void SetColour(int hexValue)
	{
		r = ((hexValue >> 24) & 0x0FF) / 255.0f;
		g = ((hexValue >> 16) & 0x0FF) / 255.0f;
		b = ((hexValue >> 8) & 0xFF) / 255.0f;
		a = ((hexValue) & 0xFF) / 255.0f;
	}

	void SetColour(std::string hex)
	{
		std::stringstream stream;

		//Drop a hash if the value has one
		if (hex[0] == '#')
			hex.erase(0, 1);

		stream << std::hex << hex;

		if (hex.size() == 6)
		{
			stream << std::hex << 255;
		}
		else if (hex.size() != 8)
		{
			ENGINE_ERROR("Invalid Hex Code #{0}", hex);
			return;
		}
		int hexValue;
		stream >> hexValue;
		SetColour(hexValue);
	}

	// Get a formatted Hex Code string
	std::string HexCode()
	{
		int red = (int)(r * 255);
		int green = (int)(g * 255);
		int blue = (int)(b * 255);
		int alpha = (int)(a * 255);
		std::stringstream stream;
		stream << "#" << std::hex << red << green << blue << alpha;
		return stream.str();
	}

	int HexValue()
	{
		int red = (int)(r * 255);
		int green = (int)(g * 255);
		int blue = (int)(b * 255);
		int alpha = (int)(a * 255);
		std::stringstream stream;
		stream << std::hex << red << green << blue << alpha;
		int hexValue;
		stream >> hexValue;
		return hexValue;
	}

	std::string to_string()
	{
		return std::string("r:" + std::to_string(r) + " g:" + std::to_string(g) + " b:" + std::to_string(b) + " a:" + std::to_string(a));
	}
};

inline std::ostream& operator<<(std::ostream& os, Colour& c)
{
	return os << c.to_string();
}