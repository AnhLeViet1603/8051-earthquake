#ifndef DETECT_H
#define DETECT_H

void detect_init(void);
void detect_update(void);  /* Doc g_mag tu shared global */
unsigned char detect_is_quake(void);

#endif