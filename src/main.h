#ifndef MAIN_H
#define MAIN_H


#include <msgbus/messagebus.h>
#include <parameter/parameter.h>
#include <camera/dcmi_camera.h>

/** Robot wide IPC bus. */
extern messagebus_t bus;
extern parameter_namespace_t parameter_root;


#endif
