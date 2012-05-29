/*
 * types.h
 *
 *  Created on: 2011-11-6
 *      Author: poseidon
 */

#ifndef TYPES_H_
#define TYPES_H_

typedef int					bool;
typedef char					int8;
typedef short					int16;
typedef int 					int32;
typedef long long				int64;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int			uint32;
typedef unsigned long long	uint64;

typedef void * 				pvoid;
typedef int					handle;

#ifndef true
	#define true				1
#endif

#ifndef false
	#define false				0
#endif

#ifndef invalid_value
	#define invalid_value	-1
#endif

#endif /* TYPES_H_ */
