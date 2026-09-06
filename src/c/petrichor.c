// Copyright 2026 Luke Steuber. MIT License.
#include "petrichor.h"
static float minimum(float a,float b){return a<b?a:b;}
static void append(RainTime *t,int emitter,float age){
  const float speeds[]={.5f,.7f,1.0f},maximum[]={1.05f,.66f,.34f},strength[]={2.2f,1.5f,.9f};
  float life=3.2f/speeds[emitter];
  if(age>life||t->count>=PETRICHOR_MAX_RIPPLES)return;
  float progress=age/life,fade=1-progress;
  t->ripple[t->count++]=(Ripple){emitter,age,life,maximum[emitter],strength[emitter],progress,
    maximum[emitter]*minimum(1,progress*1.15f),fade*fade};
}
RainTime rain_time(int hour,int minute,int second,int millis){
  float fraction=millis/1000.0f;
  RainTime t={.value={hour,minute,second},.age={minute*60+second+fraction,second+fraction,fraction}};
  append(&t,0,t.age[0]);append(&t,1,t.age[1]);
  for(int i=3;i>=0;i--)append(&t,2,fraction+i);
  return t;
}
RainLayout rain_layout(int width,int height,bool round){
  int unit=width<height?width:height;
  return (RainLayout){width,height,round,
    {width*(round?.30f:.25f),width*.50f,width*(round?.70f:.75f)},
    {height*.34f,height*(round?.70f:.73f),height*(round?.34f:.36f)},unit*(round?.17f:.205f)};
}
float rain_fill(int value,int total,int ring){
  float fraction=(float)value*12/total-ring;
  return fraction<0?0:fraction>1?1:fraction;
}
float rain_crest(Ripple ripple,int band){return ripple.lead-band*(.05f+ripple.maximum*.04f);}
