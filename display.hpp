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
public:
	SDL_Window *sdlWindow;
	bool init(bool fullscreen);
	bool updateForMenu();
	void resize();
};







#endif /* DISPLAY_HPP_ */
