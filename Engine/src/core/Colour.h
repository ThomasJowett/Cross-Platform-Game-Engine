#pragma once
class Colour
{
public:
	enum ColourDatabase
	{
		RED,
		BLUE,
		WHITE,
		BLACK,
		CYAN,
		MAGENTA,
		YELLOW,
		SILVER,
		GREY,
		MAROON,
		OLIVE,
		GREEN,
		PURPLE,
		INDIGO,
		VIOLET,
		TEAL,
		NAVY,
		ORANGE,
		LIME_GREEN,
		FOREST_GREEN,
		BROWN,
		BEIGE,
		PINK,
		TURQUOISE,
		KHAKI
	};

	float r, g, b, a;

	Colour() { r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f; }
	Colour(float r, float g, float b, float a) :r(r), g(g), b(b), a(a) {}
	Colour(ColourDatabase colour)
	{
		SetColour(colour);
	}
	~Colour() = default;

	void SetColour(ColourDatabase colour)
	{
		switch (colour)
		{
		case Colour::RED:			r = 1.00f, g = 0.00f, b = 0.00f, a = 1.0f;	break;
		case Colour::LIME_GREEN:	r = 0.00f, g = 1.00f, b = 0.00f, a = 1.0f;	break;
		case Colour::BLUE:			r = 0.00f, g = 0.00f, b = 1.00f, a = 1.0f;	break;
		case Colour::WHITE:			r = 1.00f, g = 1.00f, b = 1.00f, a = 1.0f;	break;
		case Colour::BLACK:			r = 0.00f, g = 0.00f, b = 0.00f, a = 1.0f;	break;
		case Colour::CYAN:			r = 0.00f, g = 1.00f, b = 1.00f, a = 1.0f;	break;
		case Colour::MAGENTA:		r = 1.00f, g = 0.00f, b = 1.00f, a = 1.0f;	break;
		case Colour::YELLOW:		r = 1.00f, g = 1.00f, b = 0.00f, a = 1.0f;	break;
		case Colour::SILVER:		r = 0.75f, g = 0.75f, b = 0.75f, a = 1.0f;	break;
		case Colour::GREY:			r = 0.40f, g = 0.40f, b = 0.40f, a = 1.0f;	break;
		case Colour::MAROON:		r = 0.50f, g = 0.00f, b = 0.00f, a = 1.0f;	break;
		case Colour::OLIVE:			r = 0.50f, g = 0.50f, b = 0.00f, a = 1.0f;	break;
		case Colour::GREEN:			r = 0.00f, g = 0.50f, b = 0.50f, a = 1.0f;	break;
		case Colour::PURPLE:		r = 0.50f, g = 0.00f, b = 0.50f, a = 1.0f;	break;
		case Colour::INDIGO:		r = 0.30f, g = 0.00f, b = 0.50f, a = 1.0f;	break;
		case Colour::VIOLET:		r = 0.93f, g = 0.50f, b = 0.93f, a = 1.0f;	break;
		case Colour::TEAL:			r = 0.00f, g = 0.50f, b = 0.50f, a = 1.0f;	break;
		case Colour::NAVY:			r = 0.00f, g = 0.00f, b = 0.50f, a = 1.0f;	break;
		case Colour::ORANGE:		r = 1.00f, g = 0.64f, b = 0.50f, a = 1.0f;	break;
		case Colour::FOREST_GREEN:	r = 0.13f, g = 0.54f, b = 0.13f, a = 1.0f;	break;
		case Colour::BROWN:			r = 0.64f, g = 0.16f, b = 0.16f, a = 1.0f;	break;
		case Colour::BEIGE:			r = 0.96f, g = 0.96f, b = 0.86f, a = 1.0f;	break;
		case Colour::PINK:			r = 1.00f, g = 0.75f, b = 0.80f, a = 1.0f;	break;
		case Colour::TURQUOISE:		r = 0.25f, g = 0.95f, b = 0.81f, a = 1.0f;	break;
		case Colour::KHAKI:			r = 0.94f, g = 0.90f, b = 0.54f, a = 1.0f;	break;

		default:					r = 0.00f, g = 0.00f, b = 0.00f, a = 1.0f;	break;
		}

	}
};

