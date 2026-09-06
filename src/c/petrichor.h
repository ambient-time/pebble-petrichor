// Copyright 2026 Luke Steuber. MIT License.
#pragma once
#include <stdbool.h>
#define PETRICHOR_MAX_RIPPLES 6
typedef struct {int emitter; float age,life,maximum,strength,progress,lead,fade;} Ripple;
typedef struct {int value[3];float age[3];Ripple ripple[PETRICHOR_MAX_RIPPLES];int count;} RainTime;
typedef struct {int width,height;bool round;float x[3],y[3],gauge;} RainLayout;
RainTime rain_time(int hour,int minute,int second,int millis);
RainLayout rain_layout(int width,int height,bool round);
float rain_fill(int value,int total,int ring);
float rain_crest(Ripple ripple,int band);
