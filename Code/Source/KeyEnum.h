// Name:
// KeyEnum.h
// Description:
// KeyEnum enum.
// Lists all of the keys that Natural Fury is able to use. These keys are sent to PlayerViews in response to OS events.
// Notes:
// OS-Unaware

#ifndef KEY_ENUM_H
#define KEY_ENUM_H

enum class KeyEnum
{
	Invalid,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Num0,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	Apostrophe, Dash, Equals, LeftSquare, RightSquare, Backslash, Semicolon, Quote, Comma, Period, ForwardSlash,
	Escape, Tab, CapsLock, Leftshift, LeftControl, Space, RightControl, RightShift, Enter, Backspace, Delete, Insert,
	Up, Down, Left, Right,
	Home, End, PageUp, PageDown,
	NumLock, KeypadSlash, KeypadAsterisk, KeypadMinus, KeypadPlus, KeypadEnter, KeypadPeriod, Keypad0, Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9
};

#endif