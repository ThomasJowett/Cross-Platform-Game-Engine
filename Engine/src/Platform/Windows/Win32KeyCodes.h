#pragma once
#include <WinUser.h>
#include "Core/KeyCodes.h"

static int VirtualCodesToKeyCodes(int keycode)
{
	switch (keycode)
	{
	case VK_SPACE: return KEY_SPACE;
	case VK_OEM_COMMA: return KEY_COMMA;
	case VK_OEM_MINUS: return KEY_MINUS;
	case VK_OEM_PERIOD: return KEY_PERIOD;
	case VK_OEM_1: return KEY_SEMICOLON;
	case VK_OEM_2: return KEY_SLASH;
	case VK_OEM_3: return KEY_WORLD_1;
	case VK_OEM_4: return KEY_LEFT_BRACKET;
	case VK_OEM_5: return KEY_BACKSLASH;
	case VK_OEM_6: return KEY_RIGHT_BRACKET;
	case VK_OEM_7: return KEY_BACKSLASH;
	case VK_OEM_8: return KEY_GRAVE_ACCENT;
	case VK_OEM_PLUS: return KEY_EQUAL;

	case VK_ESCAPE: return KEY_ESCAPE;
	case VK_RETURN: return KEY_ENTER;
	case VK_TAB: return KEY_TAB;
	case VK_BACK: return KEY_BACKSPACE;
	case VK_INSERT: return KEY_INSERT;
	case VK_DELETE: return KEY_DELETE;
	case VK_RIGHT: return KEY_RIGHT;
	case VK_LEFT: return KEY_LEFT;
	case VK_DOWN: return KEY_DOWN;
	case VK_UP: return KEY_UP;
	case VK_PRIOR: return KEY_PAGE_UP;
	case VK_NEXT: return KEY_PAGE_DOWN;
	case VK_HOME: return KEY_HOME;
	case VK_END: return KEY_END;
	case VK_CAPITAL: return KEY_CAPS_LOCK;
	case VK_SCROLL: return KEY_SCROLL_LOCK;
	case VK_NUMLOCK: return KEY_NUM_LOCK;
	case VK_SNAPSHOT: return KEY_PRINT_SCREEN;
	case VK_PAUSE: return KEY_PAUSE;

	case '0': return KEY_0;
	case '1': return KEY_1;
	case '2': return KEY_2;
	case '3': return KEY_3;
	case '4': return KEY_4;
	case '5': return KEY_5;
	case '6': return KEY_6;
	case '7': return KEY_7;
	case '8': return KEY_8;
	case '9': return KEY_9;

	case 'A':return KEY_A;
	case 'B':return KEY_B;
	case 'C':return KEY_C;
	case 'D':return KEY_D;
	case 'E':return KEY_E;
	case 'F':return KEY_F;
	case 'G':return KEY_G;
	case 'H':return KEY_H;
	case 'I':return KEY_I;
	case 'J':return KEY_J;
	case 'K':return KEY_K;
	case 'L':return KEY_L;
	case 'M':return KEY_M;
	case 'N':return KEY_N;
	case 'O':return KEY_O;
	case 'P':return KEY_P;
	case 'Q':return KEY_Q;
	case 'R':return KEY_R;
	case 'S':return KEY_S;
	case 'T':return KEY_T;
	case 'U':return KEY_U;
	case 'V':return KEY_V;
	case 'W':return KEY_W;
	case 'X':return KEY_X;
	case 'Y':return KEY_Y;
	case 'Z':return KEY_Z;

	case VK_F1: return KEY_F1;
	case VK_F2: return KEY_F2;
	case VK_F3: return KEY_F3;
	case VK_F4: return KEY_F4;
	case VK_F5: return KEY_F5;
	case VK_F6: return KEY_F6;
	case VK_F7: return KEY_F7;
	case VK_F8: return KEY_F8;
	case VK_F9: return KEY_F9;
	case VK_F10: return KEY_F10;
	case VK_F11: return KEY_F11;
	case VK_F12: return KEY_F12;
	case VK_F13: return KEY_F13;
	case VK_F14: return KEY_F14;
	case VK_F15: return KEY_F15;
	case VK_F16: return KEY_F16;
	case VK_F17: return KEY_F17;
	case VK_F18: return KEY_F18;
	case VK_F19: return KEY_F19;
	case VK_F20: return KEY_F20;
	case VK_F21: return KEY_F21;
	case VK_F22: return KEY_F22;
	case VK_F23: return KEY_F23;
	case VK_F24: return KEY_F24;

	case VK_DECIMAL: return KEY_KP_DECIMAL;
	case VK_DIVIDE: return KEY_KP_DIVIDE;
	case VK_MULTIPLY: return KEY_KP_MULTIPLY;
	case VK_SUBTRACT: return KEY_KP_SUBTRACT;
	case VK_ADD: return KEY_KP_ADD;
	case VK_SHIFT: return KEY_LEFT_SHIFT;
	case VK_CONTROL: return KEY_LEFT_CONTROL;
	case VK_LMENU: return KEY_LEFT_ALT;
	case VK_LWIN: return KEY_LEFT_SUPER;
	case VK_RSHIFT: return KEY_RIGHT_SHIFT;
	case VK_RCONTROL: return KEY_RIGHT_CONTROL;
	case VK_RMENU: return KEY_RIGHT_ALT;
	case VK_RWIN: return KEY_RIGHT_SUPER;
	case VK_MENU: return KEY_MENU;

	default: return KEY_UNKNOWN;
	}
}

static int KeyCodesToVirtualCodes(int keycode)
{
	switch (keycode)
	{
	case KEY_SPACE: return VK_SPACE;
	case KEY_COMMA: return VK_OEM_COMMA;
	case KEY_MINUS: return VK_OEM_MINUS;
	case KEY_PERIOD: return VK_OEM_PERIOD;
	case KEY_SEMICOLON: return VK_OEM_1;
	case KEY_SLASH: return VK_OEM_2;
	case KEY_WORLD_1: return VK_OEM_3;
	case KEY_LEFT_BRACKET: return VK_OEM_4;
	case KEY_BACKSLASH: return VK_OEM_5;
	case KEY_RIGHT_BRACKET: return VK_OEM_6;
	case KEY_GRAVE_ACCENT: return VK_OEM_8;
	case KEY_EQUAL: return VK_OEM_PLUS;

	case KEY_ESCAPE: return VK_ESCAPE;
	case KEY_ENTER: return VK_RETURN;
	case KEY_TAB: return VK_TAB;
	case KEY_BACKSPACE: return VK_BACK;
	case KEY_INSERT: return VK_INSERT;
	case KEY_DELETE: return VK_DELETE;
	case KEY_RIGHT: return VK_RIGHT;
	case KEY_LEFT: return VK_LEFT;
	case KEY_DOWN: return VK_DOWN;
	case KEY_UP: return VK_UP;
	case KEY_PAGE_UP: return VK_PRIOR;
	case KEY_PAGE_DOWN: return VK_NEXT;
	case KEY_HOME: return VK_HOME;
	case KEY_END: return VK_END;
	case KEY_CAPS_LOCK: return VK_CAPITAL;
	case KEY_SCROLL_LOCK: return VK_SCROLL;
	case KEY_NUM_LOCK: return VK_NUMLOCK;
	case KEY_PRINT_SCREEN: return VK_SNAPSHOT;
	case KEY_PAUSE: return VK_PAUSE;

	case KEY_0: return '0';
	case KEY_1: return '1';
	case KEY_2: return '2';
	case KEY_3: return '3';
	case KEY_4: return '4';
	case KEY_5: return '5';
	case KEY_6: return '6';
	case KEY_7: return '7';
	case KEY_8: return '8';
	case KEY_9: return '9';

	case KEY_A: return 'A';
	case KEY_B: return 'B';
	case KEY_C: return 'C';
	case KEY_D: return 'D';
	case KEY_E: return 'E';
	case KEY_F: return 'F';
	case KEY_G: return 'G';
	case KEY_H: return 'H';
	case KEY_I: return 'I';
	case KEY_J: return 'J';
	case KEY_K: return 'K';
	case KEY_L: return 'L';
	case KEY_M: return 'M';
	case KEY_N: return 'N';
	case KEY_O: return 'O';
	case KEY_P: return 'P';
	case KEY_Q: return 'Q';
	case KEY_R: return 'R';
	case KEY_S: return 'S';
	case KEY_T: return 'T';
	case KEY_U: return 'U';
	case KEY_V: return 'V';
	case KEY_W: return 'W';
	case KEY_X: return 'X';
	case KEY_Y: return 'Y';
	case KEY_Z: return 'Z';

	case KEY_F1: return VK_F1;
	case KEY_F2: return VK_F2;
	case KEY_F3: return VK_F3;
	case KEY_F4: return VK_F4;
	case KEY_F5: return VK_F5;
	case KEY_F6: return VK_F6;
	case KEY_F7: return VK_F7;
	case KEY_F8: return VK_F8;
	case KEY_F9: return VK_F9;
	case KEY_F10: return VK_F10;
	case KEY_F11: return VK_F11;
	case KEY_F12: return VK_F12;
	case KEY_F13: return VK_F13;
	case KEY_F14: return VK_F14;
	case KEY_F15: return VK_F15;
	case KEY_F16: return VK_F16;
	case KEY_F17: return VK_F17;
	case KEY_F18: return VK_F18;
	case KEY_F19: return VK_F19;
	case KEY_F20: return VK_F20;
	case KEY_F21: return VK_F21;
	case KEY_F22: return VK_F22;
	case KEY_F23: return VK_F23;
	case KEY_F24: return VK_F24;

	case KEY_KP_DECIMAL: return VK_DECIMAL;
	case KEY_KP_DIVIDE: return VK_DIVIDE;
	case KEY_KP_MULTIPLY: return VK_MULTIPLY;
	case KEY_KP_SUBTRACT: return VK_SUBTRACT;
	case KEY_KP_ADD: return VK_ADD;
	case KEY_LEFT_SHIFT: return VK_SHIFT;
	case KEY_LEFT_CONTROL: return VK_CONTROL;
	case KEY_LEFT_ALT: return VK_LMENU;
	case KEY_LEFT_SUPER: return VK_LWIN;
	case KEY_RIGHT_SHIFT: return VK_RSHIFT;
	case KEY_RIGHT_CONTROL: return VK_RCONTROL;
	case KEY_RIGHT_ALT: return VK_RMENU;
	case KEY_RIGHT_SUPER: return VK_RWIN;
	case KEY_MENU: return VK_MENU;

	default: return KEY_UNKNOWN;
	}
}