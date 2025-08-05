#pragma once

enum class Color
{
	None = 0,
	Blue = 1,
	Green = 2,
	Cyan = Blue | Green,			//3
	Red = 4,
	Magenta = Red | Blue,			//5
	Yellow = Green | Red,			//6
	IGray = Red | Green | Blue,		//7
	Intensity = 8,
	IBlue = Blue | Intensity,		//9
	IGreen = Green | Intensity,		//10
	ICyan = IBlue | IGreen,			//11
	IRed = Red | Intensity,			//12
	IMagenta = IRed | IBlue,		//13
	IYellow = IGreen | IRed,		//14
	White = IRed | IGreen | IBlue,	//15
};
