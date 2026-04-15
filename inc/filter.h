#ifndef FILTER_H
#define FILTER_H

void filter_init(void);
void filter_update(void);  /* Loc tin hieu tu g_ax/g_ay/g_az, ghi g_mag */
/* filter_get_magnitude() da duoc gop vao filter_update() -> g_mag */

#endif