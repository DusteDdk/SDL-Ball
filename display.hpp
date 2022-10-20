/*
 * display.hpp
 *
 *  Created on: Jul 9, 2020
 *      Author: Daniel Gullberg daniel_gullberg@hotmail.com
 */

#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include <SDL2/SDL.h>

class displayClass
{
private:
	SDL_GLContext glcontext;
	int displayToUse;	//!< The display index to use
public:
	SDL_Window *sdlWindow;
	int currentW;
	int currentH;
	int numOfDisplays;
	GLfloat glunits_per_xpixel, glunits_per_ypixel;
	bool init(bool, int);
	bool updateForMenu();
	void resize(int, int);
	void close();
};


#endif /* DISPLAY_HPP_ */
