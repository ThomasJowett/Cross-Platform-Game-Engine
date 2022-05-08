#pragma once

#include <sstream>
#include <iomanip>

#include "Logging/Logger.h"
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
	MUSTARD,
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

	/**
	 * Construct a new Colour object
	 * 
	 */
	Colour() { r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f; }

	/**
	 * Construct a new Colour object from rgba
	 * 
	 * @param r red
	 * @param g green
	 * @param b blue
	 * @param a alpha
	 */
	Colour(float r, float g, float b, float a) :r(r), g(g), b(b), a(a) {}

	/**
	 * Construct a new Colour object from Colours enum
	 * 
	 * @param colour 
	 */
	Colour(Colours colour)
	{
		SetColour(colour);
	}

	/**
	 * Construct a new Colour object from hex value
	 * 
	 * @param hexValue 
	 */
	Colour(int hexValue)
	{
		SetColour(hexValue);
	}

	/**
	 * Destroy the Colour object
	 * 
	 */
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
		case Colours::GREEN:		r = 0.00f, g = 0.50f, b = 0.00f, a = 1.0f;	break;
		case Colours::GREY:			r = 0.40f, g = 0.40f, b = 0.40f, a = 1.0f;	break;
		case Colours::INDIGO:		r = 0.30f, g = 0.00f, b = 0.50f, a = 1.0f;	break;
		case Colours::KHAKI:		r = 0.94f, g = 0.90f, b = 0.54f, a = 1.0f;	break;
		case Colours::LIME_GREEN:	r = 0.00f, g = 1.00f, b = 0.00f, a = 1.0f;	break;
		case Colours::MAGENTA:		r = 1.00f, g = 0.00f, b = 1.00f, a = 1.0f;	break;
		case Colours::MAROON:		r = 0.50f, g = 0.00f, b = 0.00f, a = 1.0f;	break;
		case Colours::MUSTARD:		r = 0.60f, g = 0.60f, b = 0.00f, a = 1.0f;	break;
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

	/**
	 * Set the Colour object from integer
	 * 
	 * @param hexValue in the form of 0xRRGGBBAA
	 */
	void SetColour(const uint32_t& hexValue)
	{
		r = ((hexValue >> 24) & 0x0FF) / 255.0f;
		g = ((hexValue >> 16) & 0x0FF) / 255.0f;
		b = ((hexValue >> 8) & 0xFF) / 255.0f;
		a = ((hexValue) & 0xFF) / 255.0f;
	}

	/**
	 * Set the Colour object from hexValue string
	 * 
	 * @param hex in the form of "#RRGGBBAA"
	 */
	void SetColour(const std::string& hex)
	{
		std::string thisHex = hex;
		std::stringstream stream;

		//Drop a hash if the value has one
		if (thisHex[0] == '#')
			thisHex.erase(0, 1);

		stream << std::hex << thisHex;

		if (thisHex.size() == 6)
		{
			stream << std::hex << 255;
		}
		else if (thisHex.size() != 8)
		{
			ENGINE_ERROR("Invalid Hex Code #{0}", hex);
			return;
		}
		int hexValue;
		stream >> hexValue;
		SetColour(hexValue);
	}

	// Get a formatted Hex Code string
	std::string HexCode() const
	{
		std::stringstream stream;
		stream << "#" << std::hex << std::setw(8) << std::setfill('0') << HexValue();
		return stream.str();
	}
	
	uint32_t HexValue() const
	{
		int red = (int)(r * 255);
		int green = (int)(g * 255);
		int blue = (int)(b * 255);
		int alpha = (int)(a * 255);
		return ((alpha & 0xff) << 24) + ((red & 0xff) << 16) + ((green & 0xff) << 8) + (blue & 0xff);
	}

	std::string to_string() const
	{
		return std::string("r:" + std::to_string(r) + " g:" + std::to_string(g) + " b:" + std::to_string(b) + " a:" + std::to_string(a));
	}

	float& operator[](const int i)
	{
		return i == 0 ? this->r : (i == 1 ? this->g : i == 2 ? this->b : this->a);
	}

	Colour operator=(const Colour& rhs)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	Colour operator=(const uint32_t& rhs)
	{
		SetColour(rhs);
		return *this;
	}

	Colour operator=(const std::string& rhs)
	{
		SetColour(rhs);
		return *this;
	}

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(r,g,b,a);
	}
};

inline std::ostream& operator<<(std::ostream& os, Colour& c)
{
	return os << c.to_string();
}