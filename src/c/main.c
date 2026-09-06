// Copyright 2026 Luke Steuber. MIT License.
#include <pebble.h>
#include "petrichor.h"
#include "digits.h"
static Window *s_window;
static Layer *s_layer;
static bool s_focused;
static uint32_t s_frames;
static AppTimer *s_timer;
static int s_burst_frames,s_last_second=-1;
static int rounded(float value){return (int)(value+.5f);}
static GColor cyan(int level){
#ifdef PBL_COLOR
  if(level>2)return GColorFromRGB(170,255,255);
  if(level>1)return GColorFromRGB(85,255,255);
  if(level>0)return GColorFromRGB(0,170,170);
  return GColorFromRGB(0,85,85);
#else
  return GColorWhite;
#endif
}
static void disc(GContext *ctx,GPoint point,int radius,GColor color){
  graphics_context_set_fill_color(ctx,color);graphics_fill_circle(ctx,point,radius);
}
#ifndef PBL_COLOR
static void dotted(GContext *ctx,GPoint point,int radius,int gap,int phase){
  graphics_context_set_stroke_color(ctx,GColorWhite);
  int count=radius*628/(100*gap);if(count<6)count=6;
  for(int a=0;a<count;a++){
    int angle=(a*TRIG_MAX_ANGLE/count+phase*TRIG_MAX_ANGLE/360)%TRIG_MAX_ANGLE;
    GPoint p=GPoint(point.x+sin_lookup(angle)*radius/TRIG_MAX_RATIO,point.y-cos_lookup(angle)*radius/TRIG_MAX_RATIO);
    graphics_draw_pixel(ctx,p);
  }
}
#endif
static void draw(Layer *layer,GContext *ctx){
  time_t epoch;uint16_t millis;time_ms(&epoch,&millis);struct tm local=*localtime(&epoch);
  // Sample the same event ages at 1 Hz; a tap briefly includes subsecond motion.
  RainTime t=rain_time(local.tm_hour,local.tm_min,local.tm_sec,s_burst_frames?millis:0);
  GRect bounds=layer_get_bounds(layer);int w=bounds.size.w,h=bounds.size.h,unit=w<h?w:h;
  RainLayout layout=rain_layout(w,h,PBL_IF_ROUND_ELSE(true,false));
  graphics_context_set_antialiased(ctx,true);graphics_context_set_fill_color(ctx,GColorBlack);
  graphics_fill_rect(ctx,bounds,0,GCornerNone);
  for(int i=0;i<t.count;i++){
    Ripple rp=t.ripple[i];GPoint p=GPoint(rounded(layout.x[rp.emitter]),rounded(layout.y[rp.emitter]));
    for(int k=0;k<3;k++){
      int radius=rounded(rain_crest(rp,k)*unit);
      float alpha=rp.fade*rp.strength*(.10f-k*.025f);
      if(radius<=1||alpha<=.002f)continue;
#ifdef PBL_COLOR
      graphics_context_set_stroke_color(ctx,cyan(alpha>.065f?1:0));
      graphics_context_set_stroke_width(ctx,alpha>.08f?2:1);
      graphics_draw_circle(ctx,p,radius);
#else
      // Sparse crests keep passing waves separate from the solid gauge rings.
      if(alpha>.018f)dotted(ctx,p,radius,alpha>.065f?3:6,(i*47+k*19)%360);
#endif
    }
    if(rp.progress<.18f){
      float flash=(.18f-rp.progress)/.18f;
      disc(ctx,p,rounded(unit*.02f*flash)+2,cyan(3));
    }
  }
  static const char *labels[]={"H","M","S"};
  for(int e=0;e<3;e++){
    GPoint p=GPoint(rounded(layout.x[e]),rounded(layout.y[e]));
    int maximum=rounded(layout.gauge),base=3;
    for(int ring=11;ring>=0;ring--){
      int radius=base+(maximum-base)*ring/11;
      float fill=rain_fill(t.value[e],e==0?24:60,ring);
      graphics_context_set_stroke_width(ctx,1);
#ifdef PBL_COLOR
      graphics_context_set_stroke_color(ctx,cyan(fill>.75f?2:fill>.15f?1:0));
      graphics_draw_circle(ctx,p,radius);
#else
      if(fill>=1){graphics_context_set_stroke_color(ctx,GColorWhite);graphics_draw_circle(ctx,p,radius);}
      else{
        dotted(ctx,p,radius,3,ring*47%360);
        if(fill>0){graphics_context_set_stroke_color(ctx,GColorWhite);
          graphics_draw_arc(ctx,GRect(p.x-radius,p.y-radius,radius*2,radius*2),GOvalScaleModeFitCircle,0,(int)(fill*TRIG_MAX_ANGLE));}
      }
#endif
    }
    float flash=t.age[e]<.6f?(1-t.age[e]/.6f):0;
    disc(ctx,p,2+rounded(flash*2),cyan(3));
    int scale=w>=240?2:1,offset=maximum+6+scale*3;
    // A black halo lets these source labels read when a crest passes beneath.
    graphics_context_set_fill_color(ctx,GColorBlack);
    graphics_fill_rect(ctx,GRect(p.x-4*scale,p.y-offset-4*scale,8*scale,8*scale),0,GCornerNone);
    graphics_fill_rect(ctx,GRect(p.x-7*scale,p.y+offset-4*scale,14*scale,8*scale),0,GCornerNone);
    draw_digits(ctx,labels[e],p.x,p.y-offset,scale,cyan(2));
    char value[3]={'0'+t.value[e]/10,'0'+t.value[e]%10,0};
    draw_digits(ctx,value,p.x,p.y+offset,scale,cyan(2));
  }
  s_frames++;
  if(s_last_second!=local.tm_sec){
    uint16_t end;time_ms(NULL,&end);
    APP_LOG(APP_LOG_LEVEL_INFO,"Petrichor time=%02d:%02d:%02d.%03d ripples=%d ms=%d heap=%lu frames=%lu",
      local.tm_hour,local.tm_min,local.tm_sec,millis,t.count,(end+1000-millis)%1000,
      (unsigned long)heap_bytes_free(),(unsigned long)s_frames);s_last_second=local.tm_sec;
  }
}
static void cancel_timer(void){if(s_timer){app_timer_cancel(s_timer);s_timer=NULL;}}
static void animate(void *context){s_timer=NULL;if(!s_focused||!s_burst_frames)return;
  s_burst_frames--;layer_mark_dirty(s_layer);if(s_burst_frames)s_timer=app_timer_register(125,animate,NULL);}
static void burst(void){cancel_timer();if(!s_focused)return;s_burst_frames=48;
  layer_mark_dirty(s_layer);s_timer=app_timer_register(125,animate,NULL);}
static void tick(struct tm *t,TimeUnits changed){if(s_focused&&!s_burst_frames)layer_mark_dirty(s_layer);}
static void tap(AccelAxisType axis,int32_t direction){burst();}
static void focus(bool focused){s_focused=focused;if(focused)layer_mark_dirty(s_layer);else{cancel_timer();s_burst_frames=0;}}
static void load(Window *window){s_layer=layer_create(layer_get_bounds(window_get_root_layer(window)));
  layer_set_update_proc(s_layer,draw);layer_add_child(window_get_root_layer(window),s_layer);}
static void unload(Window *window){cancel_timer();layer_destroy(s_layer);s_layer=NULL;}
static void init(void){s_window=window_create();window_set_background_color(s_window,GColorBlack);
  window_set_window_handlers(s_window,(WindowHandlers){.load=load,.unload=unload});window_stack_push(s_window,false);
  tick_timer_service_subscribe(SECOND_UNIT,tick);accel_tap_service_subscribe(tap);app_focus_service_subscribe(focus);focus(true);}
static void deinit(void){s_focused=false;cancel_timer();tick_timer_service_unsubscribe();accel_tap_service_unsubscribe();app_focus_service_unsubscribe();window_destroy(s_window);}
int main(void){init();app_event_loop();deinit();}
