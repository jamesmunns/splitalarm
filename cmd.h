#ifndef _CMD_H
#define _CMD_H

void cmd_init(void);
void cmd_periodic_cont(void);

#include "utils/uartstdio.h"
#define cmd_printf UARTprintf

#endif