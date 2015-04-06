#ifndef _ALERT_RGB_H
#define _ALERT_RGB_H
#include "misc.h"

void alert_rgb_init(void);
void alert_rgb_periodic_msec();
void alert_rgb_alarm(bool alm);
void alert_rgb_xbee_notify(bool xbee_good);

#endif