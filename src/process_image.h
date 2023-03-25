#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

// Specify the 2 consecutive lines used for tracking the black line
// The line number starts from 0 and ending to PO8030_MAX_HEIGHT - 1. Consult camera/po8030.h
// But as 2 lines will be used, the value of the first line can be higher than PO8030_MAX_HEIGHT - 2
#define USED_LINE 200   // Must be inside [0..478], according to the above explanations

//List of detection color
typedef enum {
	RED_COLOR,
	GREEN_COLOR,
	BLUE_COLOR,
} color_detection_t;


float get_distance_cm(void);
uint16_t get_line_position(void);
void process_image_start(void);
void select_color_detection(color_detection_t choice_detect_color);

#endif /* PROCESS_IMAGE_H */