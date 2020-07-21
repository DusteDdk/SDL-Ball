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
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include "display.hpp"

using namespace std;

bool displayClass::init(bool fullscreen, int displayToUseIn)
{
  bool success=1;
  int SDL_videomodeSettings = SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;

  //Save current resolution so it can be restored at exit
  // Declare display mode structure to be filled in.
  SDL_DisplayMode currentDisplayMode;

  SDL_Init(SDL_INIT_VIDEO);

  // Get current display mode of all displays.
  numOfDisplays = SDL_GetNumVideoDisplays();
  SDL_Rect displayBounds[numOfDisplays]={0};

  for(int i = 0; i < numOfDisplays; ++i){

    int should_be_zero = SDL_GetCurrentDisplayMode(i, &currentDisplayMode);

    if(should_be_zero != 0)
    {
      // In case of error...
      SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
    }
    else
    {
      // On success, print the current display mode.
      SDL_GetDisplayBounds( i, &displayBounds[i] );
      SDL_Rect currentDisplayBounds = displayBounds[i];
      SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz. %d %d %d %d",
    		  i, currentDisplayMode.w, currentDisplayMode.h, currentDisplayMode.refresh_rate,
			  currentDisplayBounds.x,currentDisplayBounds.y,currentDisplayBounds.h,currentDisplayBounds.w);
    }
  }

  if((numOfDisplays > 0) && (displayToUseIn<numOfDisplays))
  {
	  displayToUse = displayToUseIn;
  }
  else
  {
	  displayToUse = 0;
  }

  currentW = displayBounds[displayToUse].w;
  currentH = displayBounds[displayToUse].h;

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

//  if(fullscreen)
//  {
//    SDL_videomodeSettings |= SDL_WINDOW_FULLSCREEN;
//  }

  sdlWindow = SDL_CreateWindow("SDL-Ball",
		                    SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
							SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
							displayBounds[displayToUse].w, displayBounds[displayToUse].h,
							SDL_videomodeSettings);

  // Create an OpenGL context associated with the window.
  glcontext = SDL_GL_CreateContext(sdlWindow);

  resize(currentW, currentH);

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

/**@brief Method to reset our viewport after a window resize */
void displayClass::resize(int width, int height )
{
	cout << "displayClass::resize() called" << endl;

	/* Height / width ration */
	GLfloat ratio;

	/* Protect against a divide by zero */
	if ( height == 0 )
		height = 1;

	ratio = ( GLfloat )width / ( GLfloat )height;
	glunits_per_xpixel = (2.485281374*ratio) / currentH;
	glunits_per_ypixel = 2.485281374 / currentW;


	/* Setup our viewport. */
	glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

	/* change to the projection matrix and set our viewing volume. */
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	/* Set our perspective */
	gluPerspective( 45.0f, ratio, 0.1f, 10.0f );

	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode( GL_MODELVIEW );

	/* Reset The View */
	glLoadIdentity();
}

void displayClass::close()
{
	SDL_DestroyWindow(sdlWindow);
	cout << "displayClass::close() called" << endl;
}
