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
*       2026/04/02
*
* CHANGES:
*
***********************************************************************/

#ifndef _HIM_CMD_H_
#define _HIM_CMD_H_


#include "Arduino.h"

#include "him_log.h"

#include "him_cmd_config.h"

#define HIM_CMD_VERSION         "1.1.0"

#define HIM_CMD_TYPE_NONE        '-'
#define HIM_CMD_TYPE_CMD         '!'
#define HIM_CMD_TYPE_RESP        '$'
#define HIM_CMD_TYPE_MSG         '#'

#define HIM_CMD_OK               0
#define HIM_CMD_ERROR_TYPE       1
#define HIM_CMD_ERROR_MSG        2
#define HIM_CMD_ERROR_CMD        3
#define HIM_CMD_ERROR_RESP       4
#define HIM_CMD_ERROR_PARAM      5
#define HIM_CMD_ERROR_TABLE_FULL 6
#define HIM_CMD_ERROR_NOTIMPLEMENTED 7

typedef int (*cmd_func_t)(char**, int, void*, int);
typedef int (*msg_func_t)(char**, int, void*);
typedef int (*resp_func_t)(int, int, char**, int, void*);

class HimCommand
{
public:
    HimCommand();
    ~HimCommand();

    int assign_cmd(
        const char * name,
        cmd_func_t func,
        void * data, 
        const char * description_params,
        const char * description_general,
        const char * description_response
    );

    int assign_msg(
        const char * name,
        msg_func_t func,
        void * data, 
        const char * description_params,
        const char * description_general
    );

    int assign_resp(
        int cookie,
        resp_func_t func,
        void * data
    );

    int send_msg(const char * name, const char * format, ... );
    int send_msg(int id, const char * format, ... );

    int send_cmd(const char * name, resp_func_t func, void * data, const char * format, ... );
    int send_cmd(int id, resp_func_t func, void * data, const char * format, ... );

    void cmd_line_init(int baudrate, bool echo, const char * project_name, const char * project_version, long timout);
    void cmd_line_parse();

public:
    static int cmd_version(char** params, int param_count, void* data, int cookie);
    static int cmd_info(char** params, int param_count, void* data, int cookie);
    static int msg_info(char** params, int param_count, void* data);

private:

    void cmd_line_clear();

    int cmd_line_exec(char type, char * name, int id, int cookie, char** params, int param_count);
    int cmd_line_exec_msg(char * name, int id, char** params, int param_count);
    int cmd_line_exec_cmd(char * name, int id, char** params, int param_count, int cookie);
    int cmd_line_exec_resp(int cookie, char * error, char** params, int param_count);

    int get_cookie();

public:
    const char * m_project_name;
    const char * m_project_version = "x.x.x";

private:

    struct {
        char char_line[HIM_CMD_LINE_LENGTH];
        unsigned int  char_count;

        bool echo;
        long resp_timeout;
        int  state;
        bool flag_inquotes;

        struct {
            char type;
            int cookie;
            char * name;
            int id;
            char * param[HIM_CMD_LINE_PARAM_MAX];
            unsigned int param_count;
        } arg;
    } m_interpreter;

#if HIM_CMD_TABLE_MAX > 0
    struct cmd_table_s{
        const char * name;
        int id;
        cmd_func_t func;
        void * data;
        const char * description_params; 
        const char * description_general;  
        const char * description_response;
    } m_cmd_table[HIM_CMD_TABLE_MAX];
    unsigned int m_cmd_count;
#endif

#if HIM_MSG_TABLE_MAX > 0
    struct msg_table_s{
        const char * name;
        int id;
        msg_func_t func;
        void * data;
        const char * description_params; 
        const char * description_general;  
    } m_msg_table[HIM_MSG_TABLE_MAX];
    unsigned int m_msg_count;
#endif

#if HIM_RESP_TABLE_MAX > 0
    struct resp_table_s{
        long timestamp;
        int cookie;
        resp_func_t func;
        void * data;
    } m_resp_table[HIM_RESP_TABLE_MAX];
#endif

};

extern HimCommand HimCmd;


// public macros and function interface 
#define him_cmd_interpreter_init(baudrate,echo,name,version,timeout)    HimCmd.cmd_line_init(baudrate,echo,name,version,timeout)
#define him_cmd_interpreter_parse()                                     HimCmd.cmd_line_parse()

#if HIM_CMD_COMPILE_NO_DESCRIPTION
#define him_cmd_assign_msg(name, msg_func, data,  description_params, description_general)                         HimCmd.assign_msg(name, msg_func, data,  NULL, NULL)
#define him_cmd_assign_cmd(name, cmd_func, data, description_params, description_general, description_response )   HimCmd.assign_cmd(name, cmd_func, data, NULL, NULL, NULL )
#else
#define him_cmd_assign_msg(name, msg_func, data, description_params, description_general)                          HimCmd.assign_msg(name, msg_func, data,  description_params, description_general)
#define him_cmd_assign_cmd(name, cmd_func, data, description_params, description_general, description_response )   HimCmd.assign_cmd(name, cmd_func, data, description_params, description_general, description_response )
#endif 

#define him_cmd_send_msg(cmd, param_format, ... )                       HimCmd.send_msg(cmd, param_format, __VA_ARGS__ )
#define him_cmd_send_cmd(msg, msg_func, msg_data, param_format, ... )   HimCmd.send_cmd(msg, msg_func, msg_data, param_format, __VA_ARGS__ )

#endif /* _HIM_CMD_H_ */
