/*
 * controls.h
 *
 *  Created on: 2012-2-13
 *      Author: poseidon
 */

#ifndef CONTROLS_H_
#define CONTROLS_H_


uint16 control_turn(char * buffer, int32 fd);
uint16 control_curtain(char * buffer, int32 fd);
uint16 control_television(char * buffer, int32 fd);
uint16 control_air(char * buffer, int32 fd);
uint16 control_sound(char * buffer, int32 fd);
uint16 control_alarm(char * buffer, int32 fd);
uint16 control_getalarm(char * buffer, int32 fd);
uint16 control_music(char * buffer, int32 fd);
uint16 control_curlight(char * buffer, int32 fd);

#endif /* CONTROLS_H_ */
