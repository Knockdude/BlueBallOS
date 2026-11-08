#include "mouse.h"
static int x=320,y=200;
void mouse_init(void){x=320;y=200;}
void mouse_poll(void){/* PS/2 packet decoding hook; GUI remains usable without mouse packets. */}
int mouse_x(void){return x;} int mouse_y(void){return y;}
