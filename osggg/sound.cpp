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

#include "sound.hpp"

bool soundClass::init() {
  #ifndef NOSOUND
  m=0;
  int audio_rate = 44100;
  Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
  int audio_channels = 2;
  int audio_buffers = 1024;
  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
      cout << "Error: Could not open audio device."<<endl<<"Sound have been disabled."<<endl;
      soundOn=0;
      return(0);
  }
  Mix_AllocateChannels(MIX_CHANNELS);
  
  loadsounds();
  #endif
  return(1);
}

#ifndef NOSOUND
void soundClass::loadSample(const char *SampleName, int sampleNum)
{
  string F=DATADIR;
  F += SampleName;
  
  sample[sampleNum] = Mix_LoadWAV(F.data());
  if(!sample[sampleNum])
  {
    cout << "SoundManager '"<< F <<"' : " << Mix_GetError() << endl;
  }
}
#endif


/* This function puts a sample in queue for playing */
void soundClass::add(int i)
{
  #ifndef NOSOUND
  if(!soundOn) return;
  struct sampleQueuedItem qt;
  qt.s=i;
  q.push_back( qt );
  
  #endif
}

void soundClass::loadsounds()
{

  #ifndef NOSOUND
  loadSample("nozzloop.ogg", sfxNozzle);
  #endif
}

/* This function is called only when drawing a frame, and plays the queue of samples,
   It will average the x/stereo position of a sample if it is queued more than once */
void soundClass::play()
{
  #ifndef NOSOUND
  if(!soundOn) return;

  //Playlist (lol, imagination for the win..)
  vector<struct sampleQueuedItem> pl;
  vector<struct sampleQueuedItem>::iterator plIt;
  bool same=0;
  int freeChannel = -1; //The channel we will use for this sample
  
  //Loop through queue and find samples thare are the same,and put in a new vector
  for(vector<struct sampleQueuedItem>::iterator it = q.begin(); it != q.end(); ++it)
  {
    //Loop thrugh the playlist to see find out if this allready exist
    same=0;
    for(plIt=pl.begin(); plIt != pl.end(); ++plIt)
    {
      if(plIt->s == it->s)
      {
        same=1;
        plIt->num++;
      }
    }
    
    //this sample is not yet in the playlist
    if(!same)
    {
       pl.push_back( *it );
       plIt = pl.end();
       --plIt;
       plIt->num=1;
    }
  }
  q.clear();
  
  //Play the actual samples :)
  nozzleOn=0;
  for(plIt = pl.begin(); plIt != pl.end(); ++plIt)
  {
    //Find a free channel:
    for(int i=0; i < MIX_CHANNELS; i++)
    {
      if(!Mix_Playing(i))
      {
        freeChannel=i;
        break;
      }
    }

    if(plIt->s == sfxNozzle)
    {
      nozzleOn = 1;
      //Only play if not allready playing
      if(nozzleCh == -1)
      {
        nozzleCh = Mix_PlayChannel(freeChannel, sample[plIt->s], -1);
      }
    } else {
      if(Mix_PlayChannel(freeChannel, sample[plIt->s], 0) == -1)
      {
        printf("Sample %i: %s\n",plIt->s, Mix_GetError());
      }
    }
  }
  
  //Check to see if the nozzle is on
  if(!nozzleOn)
  {
    //Kill the sound
    nozzleCh = -1;
    Mix_FadeOutChannel(nozzleCh,32);
  }
  #endif
}

soundClass::~soundClass()
{
  #ifndef NOSOUND
  for(int i=0; i < SNDSAMPLES; i++)
  {
    Mix_FreeChunk(sample[i]);
  }
  Mix_CloseAudio();
  #endif
}
