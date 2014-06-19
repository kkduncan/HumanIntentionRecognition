/*
 *	This file part of HIP (Human Intention Prediction)
 *	Copyright (c) 2013, Kester Duncan
 *
 */
/// \file OARUtils.hpp
/// \brief Contains generic console functions etc.

#ifndef OAR_UTILS_HPP
#define OAR_UTILS_HPP

#include <cstdio>
#include <cstdarg>


#ifdef _WIN32
	#include <windows.h>
	#include <conio.h>
#endif


namespace oar {

/**
 * Object-Action Recognition Utils class
 */
struct OARUtils {
	/**
	 * \brief Changes the console foreground color
	 */
	static void changeTextColor(int colorChoice) {
			
#ifdef _WIN32

		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD fgColor;

		if (colorChoice == 1) {
			fgColor = FOREGROUND_GREEN | FOREGROUND_BLUE;
		} else {
			fgColor = FOREGROUND_GREEN;
		}

		SetConsoleTextAttribute(h, FOREGROUND_INTENSITY | fgColor | 0);

#else
		char command[13];
		int fgColor;

		if (colorChoice == 1) {
			fgColor = 6;
		} else {
			fgColor = 2;
		}

		sprintf (command, "%c[%d;%d;%dm", 0x1B, 1, fgColor + 30,  40);
		fprintf (stdout, "%s", command);

#endif

	}


	/**
	 * \brief Resets the consoles foreground color
	 */
	static void resetTextColor() {

#ifdef WIN32

		HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);
		WORD white = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED;
		SetConsoleTextAttribute (h, FOREGROUND_INTENSITY | white | 0);

#else
		char command[13];
		sprintf (command, "%c[0;m", 0x1B);
		fprintf (stdout, "%s", command);
#endif

	}	

};

} /* oar */


#endif /* OAR_UTILS_HPP */
