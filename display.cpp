/* ************************************************************************* *
    SDL-Ball - DX-Ball/Breakout remake with openGL and SDL for Linux
    Copyright (C) 2008-2014 Jimmy Christensen ( dusted at dusted dot dk )

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************* */

/*
 * display.cpp
 *
 *  Created on: Jul 9, 2020
 *      Author: Daniel Gullberg daniel_gullberg@hotmail.com
 */
#include <iostream>
#include <SDL2/SDL.h>
#include "display.hpp"

using namespace std;

bool displayClass::init(bool fullscreen)
{
  bool success=1;
  int SDL_videomodeSettings = SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;

  //Initialize SDL
  #ifndef NOSOUND
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) <0 )
  #else
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_JOYSTICK) <0 )
  #endif
  {
    printf("\nError: Unable to initialize SDL:%s\n", SDL_GetError());
    success = 0;
  }

  if(fullscreen)
  {
    SDL_videomodeSettings |= SDL_WINDOW_FULLSCREEN;
  }

  sdlWindow = SDL_CreateWindow("SDL-Ball",
		                    SDL_WINDOWPOS_CENTERED,
		                    SDL_WINDOWPOS_CENTERED,
							0, 0,
							SDL_videomodeSettings);

  // Create an OpenGL context associated with the window.
  SDL_GLContext glcontext = SDL_GL_CreateContext(sdlWindow);

  if( (sdlWindow == NULL) ||  (glcontext == NULL))
  {
    cout << "Error:" << SDL_GetError() << endl;
    success=0;
  }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  return(success);
}

bool displayClass::updateForMenu()
{
	cout << "displayClass::updateForMenu() called" << endl;
	return 1;
}

void displayClass::resize()
{
	cout << "displayClass::resize() called" << endl;
}
