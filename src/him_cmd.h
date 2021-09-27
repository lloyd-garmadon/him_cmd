/***********************************************************************
* FILENAME:
*       him_cmd.h
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
*       2021/09/11
*
* CHANGES:
*
***********************************************************************/

#ifndef _HIM_CMD_H_
#define _HIM_CMD_H_


#include "Arduino.h"

#include "him_log.h"

#define HIM_CMD_LINE_MAX        32
#define HIM_CMD_COOKIE_MAX       4
#define HIM_CMD_STRING_MAX       8
#define HIM_CMD_ARG_MAX         12
#define HIM_CMD_FUNC_MAX         8

#define HIM_CMD_STATE_READY      0
#define HIM_CMD_STATE_READING    1
#define HIM_CMD_STATE_EXECUTING  2
#define HIM_CMD_STATE_ERROR     -1

typedef bool (*cmd_func_t)(int, void*);

class HimCommand
{
public:
    HimCommand();
    ~HimCommand();

    void update();

    int register_func(char * cmd_string, cmd_func_t cmd_func, void * p_data);

    void set_echo(bool value);

    unsigned int getarg_count();

    bool getarg_int(int index, int &value);
    bool getarg_uint(int index, unsigned int &value);
    bool getarg_char(int index, char &value, unsigned int pos);
    bool getarg_string(int index, char * value, int &length);

private:

    char m_cmd_line[HIM_CMD_LINE_MAX];
    unsigned int  m_cmd_line_char_count;

    int   m_state;
    bool  m_echo;

    char * m_arg_value[HIM_CMD_ARG_MAX];
    unsigned int m_arg_count;

    struct {
        char cmd[HIM_CMD_STRING_MAX];
        cmd_func_t cmd_func;
        void*  p_data;
    } m_cmd_func_table[HIM_CMD_FUNC_MAX];
    unsigned int m_cmd_func_count;

private:
    void clear_cmd_line();
    bool string2int(char * str, int &value);
    bool string2uint(char * str, unsigned int &value);
};

extern HimCommand HimCmd;
extern int cookie;
extern int res;

// log macros
#define him_cmd_init(baudrate)                      Serial.begin(baudrate)
#define him_cmd_update()                            HimCmd.update()
#define him_cmd_register(cmd, func, data)           HimCmd.register_func(cmd, func, data)
#define him_cmd_set_echo(value)                     HimCmd.set_echo(value)

#define him_cmd_getarg_count()                      HimCmd.getarg_count()
#define him_cmd_getarg_int(index, value)            HimCmd.getarg_int(index, value)
#define him_cmd_getarg_uint(index, value)           HimCmd.getarg_uint(index, value)
#define him_cmd_getarg_char(index, value, pos)      HimCmd.getarg_char(index, value, pos)
#define him_cmd_getarg_string(index, value, length) HimCmd.getarg_string(index, value, length)

#define him_cmd_response(format, ...)               him_logd("\n"); if (cookie >= 0) him_logd("#%02d:%02d:", cookie, res); him_logd(format, ##__VA_ARGS__); him_logd("\n"); 

#ifndef him_serial_init
#  define him_serial_init(baudrate)                 Serial.begin(baudrate)
#endif


#endif /* _HIM_CMD_H_ */
