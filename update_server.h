#ifndef UPDATE_SERVER_H
#define UPDATE_SERVER_H

#include <WebServer.h>
#include <Update.h>

void update_begin(WebServer *server);
void update_ask();
void update_progress();
void update_end();

#endif
