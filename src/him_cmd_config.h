/***********************************************************************
* FILENAME:
*       him_cmd_config.h
*
* DESCRIPTION:
*       Him Arduino Serial Commandline Parser Routines.
*
* NOTES:
*       This library was written just for fun.
*
* AUTHOR:
*       Sven Himstedt
*
* COPYRIGHT: 
*       Sven Himstedt 2021, All rights reserved.
*
* LICENSE:
*       This library is free software.
*       You can redistribute it and/or modify it.
*       But give credit or you are an asshole.
*
* START DATE:
*       2026/04/02
*
* CHANGES:
*
***********************************************************************/

#ifndef _HIM_CMD_CONFIG_H_
#define _HIM_CMD_CONFIG_H_



#define HIM_CMD_LINE_LENGTH      32     // max command line length
#define HIM_CMD_LINE_PARAM_MAX   8      // max number of arguments in commandline 

#define HIM_CMD_TABLE_MAX        8      // maximal size of internal table
#define HIM_MSG_TABLE_MAX        8      // maximal size of internal table
#define HIM_RESP_TABLE_MAX       8      // maximal size of internal table

#define HIM_CMD_RESP_TIMEOUT_NONE       -1  // maximal time in ms to store send command cookies

#define HIM_CMD_NO_COMPILE_DESCRIPTION   0  // do NOT add description to the command and message table



#endif /* _HIM_CMD_CONFIG_H_ */
