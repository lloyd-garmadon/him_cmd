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

#define HIM_CMD_LINE_LENGTH     32          // max command line length

#define HIM_CMD_ARG_MAX          8          // max number of arguments in commandline 
#define HIM_CMD_TABLE_MAX        8
#define HIM_MSG_TABLE_MAX        8

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

    void set_name(const char * name_string, const char * version_string);
    void set_echo(bool value);

    int  assign_cmd(const char * cmd_string, cmd_func_t func, void * data, const char * params_string, const char * response_string, const char * description_string);
    int  assign_msg(const char * msg_string, const char * response_string, const char * description_string);

    void update();

    void response_cmd(int cookie, int res, const char* format, ...);
    void response_msg(int msg_id, int res, bool use_tag, const char* format, ...);
    void response_msg(const char * msg_string, int res, bool use_tag, const char* format, ...);

    unsigned int getarg_count();
    bool getarg_int(int index, int &value);
    bool getarg_uint(int index, unsigned int &value);
    bool getarg_char(int index, char &value, unsigned int pos);
    bool getarg_string(int index, char * value, int &length);

    void dump_cmd_if();
    void dump_msg_if();

public:
    static bool version(int cookie, void * data);
    static bool info(int cookie, void * data);

private:

    void clear_cmd_line();
    bool string2int(char * str, int &value);
    bool string2uint(char * str, unsigned int &value);

private:

    const char * m_name;
    const char * m_version;

    char m_cmd_line[HIM_CMD_LINE_LENGTH];
    unsigned int  m_cmd_line_char_count;

    int   m_state;
    bool  m_echo;

    char * m_arg_value[HIM_CMD_ARG_MAX];
    unsigned int m_arg_count;

    struct {
        const char * cmd;
        const char * id;
        const char * params;
        const char * response;
        const char * descr;
        cmd_func_t func;
        void*  data;
    } m_cmd_table[HIM_CMD_TABLE_MAX];
    unsigned int m_cmd_count;

    struct {
        const char * msg;
        const char * id;
        const char * response;
        const char * descr;
    } m_msg_table[HIM_MSG_TABLE_MAX];
    unsigned int m_msg_count;
};

extern HimCommand HimCmd;
extern int cookie;
extern int res;

// public macro and function interface 
#define him_cmd_init(baudrate)                      Serial.begin(baudrate)
#ifndef him_serial_init
#  define him_serial_init(baudrate)                 Serial.begin(baudrate)
#endif

void inline him_cmd_update()                                                { return HimCmd.update(); };

inline void him_cmd_set_echo(bool value)                                    { return HimCmd.set_echo(value); };
inline void him_cmd_set_name(const char * name_string,
                             const char * version_string)                   { return HimCmd.set_name(name_string, version_string); };

inline int him_cmd_assign_cmd(  const char * cmd_string, 
                                cmd_func_t func, void * data, 
                                const char * params_string = NULL, 
                                const char * response_string = NULL, 
                                const char * description_string = NULL)     { return HimCmd.assign_cmd(cmd_string, func, data, params_string, response_string, description_string); };
inline int him_cmd_assign_msg(  const char * msg_string, 
                                const char * response_string = NULL, 
                                const char * description_string = NULL)     { return HimCmd.assign_msg(msg_string, response_string, description_string); };

#define him_cmd_response_cmd(   cookie, res, format, ...)                   HimCmd.response_cmd(cookie, res, format, ##__VA_ARGS__);
#define him_cmd_response_msg(   msg, res, use_tag, format, ...)             HimCmd.response_msg(msg, res, use_tag, format, ##__VA_ARGS__);

inline unsigned int him_cmd_getarg_count()                                  { return HimCmd.getarg_count(); };
inline bool him_cmd_getarg_int(int index, int &value)                       { return HimCmd.getarg_int(index, value); };
inline bool him_cmd_getarg_uint(int index, unsigned int &value)             { return HimCmd.getarg_uint(index, value); };
inline bool him_cmd_getarg_char(int index, char &value, unsigned int pos)   { return HimCmd.getarg_char(index, value, pos); };
inline bool him_cmd_getarg_string(int index, char * value, int &length)     { return HimCmd.getarg_string(index, value, length); };


#endif /* _HIM_CMD_H_ */
