/* ************************************************************************* * 
    SDL-Ball - DX-Ball/Breakout remake with openGL and SDL for Linux 
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

struct sampleQueuedItem {
  int s,p,num;
};

class soundClass {
  private:
  #ifdef WITH_SOUND
  //The actual sound samples
  Mix_Chunk *sample[SNDSAMPLES];
  #endif 
  //List of queued samples
  vector<struct sampleQueuedItem> q;
  void loadSample(const char *SampleName, int sampleNum);
  
  public:
  bool init();
  void play();
  void add(int i, GLfloat x);
  void loadsounds();
  ~soundClass();
};

bool soundClass::init() {
  #ifdef WITH_SOUND
  int audio_rate = 44100;
  Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
  int audio_channels = 2;
  int audio_buffers = 1024;
  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
      cout << "Error: Could not open audio device."<<endl<<"Sound have been disabled."<<endl;
      setting.sound=0;
      return(0);
  }
  Mix_AllocateChannels(MIX_CHANNELS);
  
  if(setting.sound)
    loadsounds();
  #endif
  return(1);
}

void soundClass::loadSample(const char *SampleName, int sampleNum)
{
  #ifdef WITH_SOUND
  string F=DATADIR"snd/";
  F += SampleName;
  
  sample[sampleNum] = Mix_LoadWAV(F.data());
  if(!sample[sampleNum])
  {
    cout << "SoundManager '"<< F <<"' : " << Mix_GetError() << endl;
  }
  #endif
}


/* This function puts a sample in queue for playing */
void soundClass::add(int i, GLfloat x)
{
  #ifdef WITH_SOUND
  if(setting.sound==0)
    return;
  
  int p;
  if(setting.stereo)
  {
    p=(255.0/3.2)*(x+1.6); 
  }
  
  struct sampleQueuedItem qt;
  qt.s=i;
  qt.p=p;
  q.push_back( qt );
  
  #endif
}

void soundClass::loadsounds()
{

  #ifdef WITH_SOUND
  loadSample("start.ogg", SND_START);
  loadSample("ball-hit-border.ogg", SND_BALL_HIT_BORDER);
  loadSample("ball-hit-paddle.ogg", SND_BALL_HIT_PADDLE);
  loadSample("norm-brick-break.ogg", SND_NORM_BRICK_BREAK);
  loadSample("expl-brick-break.ogg", SND_EXPL_BRICK_BREAK);
  loadSample("glass-brick-hit.ogg", SND_GLASS_BRICK_HIT);
  loadSample("glass-brick-break.ogg", SND_GLASS_BRICK_BREAK);
  loadSample("invisible-brick-appear.ogg", SND_INVISIBLE_BRICK_APPEAR);
  loadSample("cement-brick-hit.ogg", SND_CEMENT_BRICK_HIT);
  loadSample("doom-brick-break.ogg", SND_DOOM_BRICK_BREAK);
  loadSample("po-hit-border.ogg", SND_PO_HIT_BORDER);
  loadSample("good-po-hit-paddle.ogg", SND_GOOD_PO_HIT_PADDLE);
  loadSample("evil-po-hit-paddle.ogg", SND_EVIL_PO_HIT_PADDLE);
  loadSample("shot.ogg", SND_SHOT);
  loadSample("die.ogg", SND_DIE);
  loadSample("nextlevel.ogg", SND_NEXTLEVEL);
  loadSample("gameover.ogg", SND_GAMEOVER);
  loadSample("highscore.ogg", SND_HIGHSCORE);
  loadSample("menuclick.ogg", SND_MENUCLICK);
  loadSample("glue-ball-hit-paddle.ogg", SND_GLUE_BALL_HIT_PADDLE);
  loadSample("buy-powerup.ogg", SND_BUY_POWERUP);
//   loadSample("score-tick.ogg", SND_SCORE_TICK); //Not sure if this is a good idea..
  #endif
}

/* This function is called only when drawing a frame, and plays the queue of samples,
   It will average the x/stereo position of a sample if it is queued more than once */
void soundClass::play()
{
  #ifdef WITH_SOUND
  if(setting.sound==0)
    return;

  //Playlist (lol, imagination for the win..)
  vector<struct sampleQueuedItem> pl;
  vector<struct sampleQueuedItem>::iterator plIt;
  bool same=0;
  int freeChannel = -1; //The channel we will use for this sample
  
  //Loop through queue and find samples thare are the same, average their position and put in a new vector
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
        plIt->p += it->p;
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
    if(setting.stereo)
    {
      plIt->p /=plIt->num;
      Mix_SetPanning(freeChannel, 255-plIt->p, plIt->p);
    }
    
    if(Mix_PlayChannel(freeChannel, sample[plIt->s], 0) == -1)
    {
      printf("Sample %i: %s\n",plIt->s, Mix_GetError());
    }
  }
  #endif
}

soundClass::~soundClass()
{
  #ifdef WITH_SOUND
  for(int i=0; i < SNDSAMPLES; i++)
  {
    Mix_FreeChunk(sample[i]);
  }
  Mix_CloseAudio();
  #endif
}
