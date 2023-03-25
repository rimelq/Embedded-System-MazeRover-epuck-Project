#ifndef PI_REGULATOR_H
#define PI_REGULATOR_H

//start the PI regulator thread
void pi_regulator_start(void);

//handle motors
void set_enabled_motors(bool enable);
void toogle_enabled_motors(void);

#endif /* PI_REGULATOR_H */