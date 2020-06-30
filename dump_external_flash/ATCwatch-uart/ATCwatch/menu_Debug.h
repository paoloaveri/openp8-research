
#pragma once
#include "Arduino.h"
#include "class.h"
#include "images.h"
#include "menu.h"
#include "display.h"
#include "ble.h"
#include "time.h"
#include "battery.h"
#include "sleep.h"
#include "accl.h"
#include "push.h"
#include "heartrate.h"
#include "backlight.h"

class DebugScreen : public Screen
{
  public:
    virtual void pre()
    {
      label = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label, "Debug");
      lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

      label_millis = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_millis, "Millis:");
      lv_obj_align(label_millis, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 25);

      label_uptime = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_uptime, "Uptime:");
      lv_obj_align(label_uptime, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 45);

      label_reset = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_reset, "Reset:");
      lv_obj_align(label_reset, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 65);
      lv_label_set_text_fmt(label_reset, "Reset: %i", NRF_POWER->RESETREAS);

      label_wakeup = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_wakeup, "Wakeup:");
      lv_obj_align(label_wakeup, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 85);
      char wakeup_string[14];
      sprintf(wakeup_string, "Wakeup: %s", wakeup_reason[get_wakeup_reason()]);
      lv_label_set_text_fmt(label_wakeup, wakeup_string);

      lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
      lv_img_set_src(img1, &IsymbolDebug);
      lv_obj_align(img1, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    }

    virtual void main()
    {
      long days = 0;
      long hours = 0;
      long mins = 0;
      long secs = 0;
      secs = millis() / 1000;
      mins = secs / 60;
      hours = mins / 60;
      days = hours / 24;
      secs = secs - (mins * 60);
      mins = mins - (hours * 60);
      hours = hours - (days * 24);

      char time_string[14];
      sprintf(time_string, "%i %02i:%02i:%02i", days, hours, mins, secs);

      lv_label_set_text_fmt(label_millis, "millis: %d", millis());
      lv_label_set_text_fmt(label_uptime, "Uptime: %s", time_string);
    }

    virtual void right()
    {
      set_last_menu();
    }
    
  private:
    lv_obj_t *label, *label_millis, *label_uptime, *label_reset, *label_wakeup;
    char * wakeup_reason[10] = {"Unset", "Push", "Connect", "Disconnect", "Charged", "Charge", "Button", "Touch", "Accl", "AcclINT"};

};