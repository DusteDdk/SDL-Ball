/* ************************************************************************* *
    OldSkoolGravityGame (OSGG) Lunar Lander-like game for linux.
    Copyright (C) 2008 Jimmy Christensen ( dusted at dusted dot dk )

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

#include <vector>
#include <SDL/SDL_mixer.h>
#include <iostream>


#ifndef DATADIR
    #define DATADIR "./"
#endif

using namespace std;

extern bool soundOn;
enum { sfxNozzle };
#define SNDSAMPLES 1

struct sampleQueuedItem {
  int s,num;
};

class soundClass {
  private:
  #ifndef NOSOUND
  //The actual sound samples
  Mix_Chunk *sample[SNDSAMPLES];
  //List of queued samples
  vector<struct sampleQueuedItem> q;
  void loadSample(const char *SampleName, int sampleNum);

  int m; //Secret undocumented variable
  void loadsounds();

  int nozzleCh;
  bool nozzleOn;

  #endif

  public:
  bool init();
  void play();
  void add(int i);
  ~soundClass();
};
