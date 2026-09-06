// Copyright 2026 Luke Steuber. MIT License.
#include "../src/c/petrichor.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
static void frame(int h,int m,int s,int ms){
  RainTime t=rain_time(h,m,s,ms);assert(t.value[0]==h&&t.value[1]==m&&t.value[2]==s);
  int counts[3]={0};
  for(int i=0;i<t.count;i++){
    Ripple r=t.ripple[i];counts[r.emitter]++;
    assert(r.age>=0&&r.age<=r.life&&r.fade>=0&&r.fade<=1);
    double speed=r.emitter==0?.5:r.emitter==1?.7:1;
    double maximum=r.emitter==0?1.05:r.emitter==1?.66:.34;
    assert(fabs(r.life-3.2/speed)<.000001);
    double progress=r.age/(3.2/speed);
    assert(fabs(r.lead-maximum*fmin(1,progress*1.15))<.000001);
    assert(fabs(r.fade-pow(1-progress,2))<.000001);
    for(int band=0;band<3;band++)assert(fabs(rain_crest(r,band)-(r.lead-band*(.05+maximum*.04)))<.000001);
  }
  assert(counts[0]==(m*60+s+ms/1000.0<=6.4));
  assert(counts[1]==(s+ms/1000.0<=3.2/.7));
  assert(counts[2]==(ms<=200?4:3));
}
int main(void){
  for(int second=0;second<86400;second++)for(int ms=0;ms<1000;ms+=125)
    frame(second/3600,second/60%60,second%60,ms);
  for(int total=24;total<=60;total+=36)for(int value=0;value<total;value++){
    float filled=0;for(int ring=0;ring<12;ring++)filled+=rain_fill(value,total,ring);
    assert(fabs(filled-(float)value*12/total)<.000002);
  }
  int sizes[][2]={{144,168},{180,180},{144,168},{200,228},{144,168},{260,260}};
  for(int target=0;target<6;target++){
    int w=sizes[target][0],h=sizes[target][1];bool round=w==h;
    RainLayout l=rain_layout(w,h,round);
    for(int e=0;e<3;e++){
      float ext=l.gauge+6+(w>=240?6:3)+4*(w>=240?2:1);
      assert(l.x[e]-l.gauge>=1&&l.x[e]+l.gauge<w-1);
      assert(l.y[e]-ext>=1&&l.y[e]+ext<h-1);
      if(round)for(int sign=-1;sign<=1;sign+=2){
        double dx=l.x[e]-w/2,dy=l.y[e]+sign*ext-h/2;
        assert(hypot(dx,dy)<w/2-1);
      }
    }
  }
  frame(0,0,0,199);frame(0,0,0,200);frame(0,0,0,201);
  frame(0,0,4,571);frame(0,0,4,572);frame(0,0,6,400);frame(0,0,6,401);
  assert(rain_time(0,0,0,0).count==6&&rain_time(12,0,0,0).count==6);
  puts("PASS: 691200 time samples, real H/M/S splash cadence, source crest equations, 12-ring fills, six-target label bounds, noon/midnight.");
}
