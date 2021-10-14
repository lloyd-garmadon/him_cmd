/***********************************************************************
* FILENAME:
*       him_cmd.cpp
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

#define HIM_LIBRARY_COMPILE

#include "him_cmd.h"



bool HimCommand::version(int cookie, void * data)
{
    HimCommand * self = reinterpret_cast<HimCommand*>(data);

    him_logd("\n");
    if ( cookie > 0) {
        him_logd("#%02d:%02d:", cookie, 0);
    }
    him_logd("%s %s\n", self->m_name, self->m_version);

    return 0;
}

bool HimCommand::cmd_if(int cookie, void * data)
{
    HimCommand * self = reinterpret_cast<HimCommand*>(data);

    int res = 0;

    him_logd("\n");
    if ( cookie > 0) {
        him_logd("#%02d:%02d:", cookie, res);
    }
    him_logd("[");
    for (int i=0; i<self->m_cmd_count; i++) {
        if(i>0) him_logd(",");
        him_logd("[\"%s\",\"%d\",\"%s\",\"%s\",\"%s\"]", self->m_cmd_table[i].cmd, self->m_cmd_table[i].id, self->m_cmd_table[i].params, self->m_cmd_table[i].response, self->m_cmd_table[i].descr);
    }
    him_logd("]\n");

    return 0;
}

bool HimCommand::msg_if(int cookie, void * data)
{
    HimCommand * self = reinterpret_cast<HimCommand*>(data);

    int res = 0;

    him_logd("\n");
    if ( cookie > 0) {
        him_logd("#%02d:%02d:", cookie, res);
    }
    him_logd("[");
    for (int i=0; i<self->m_msg_count; i++) {
        if(i>0) him_logd(",");
        him_logd("[\"%s\",\"%d\",\"%s\",\"%s\"]", self->m_msg_table[i].msg, self->m_msg_table[i].id, self->m_msg_table[i].response, self->m_msg_table[i].descr);
    }
    him_logd("]\n");

    return 0;
}



HimCommand::HimCommand()
{
    clear_cmd_line();
    m_echo = false;
    m_cmd_count = 0;
    m_msg_count = 0;
    m_name = "noname";
    m_version = "";
    him_cmd_assign_cmd( "version", HimCommand::version, (void*)this,
                        "",
                        "<name> <version>",
                        "returns the project name and version string");
    him_cmd_assign_cmd( "cmd_if", HimCommand::cmd_if, (void*)this, 
                        "",
                        "[ [name,params,response,description], ... ]",
                        "returns a list of all registered command functions");
    him_cmd_assign_cmd( "msg_if", HimCommand::msg_if, (void*)this, 
                        "",
                        "[ [name,response,description], ... ]",
                        "returns a list of all registered message functions");
}

HimCommand::~HimCommand()
{
}



void HimCommand::set_echo(bool value)
{
    m_echo = value;    
}

void HimCommand::set_name(const char * name_string, const char * version_string)
{
    if (name_string != NULL || name_string[0] != 0 ) {
        m_name = name_string;
    }
    if (version_string != NULL || version_string[0] != 0 ) {
        m_version = version_string;
    }
}

int HimCommand::assign_cmd(const char * cmd_string, cmd_func_t func, void * data, const char * params_string, const char * response_string, const char * description_string)
{
    if( m_cmd_count >= HIM_CMD_TABLE_MAX ) {
        return 0;
    } else if ( !cmd_string ) {
        return 0;
    } else if ( !cmd_string[0] ) {
        return 0;
    } else if ( !func ) {
        return 0;
    } else {
        m_cmd_table[m_cmd_count].cmd = cmd_string;
        m_cmd_table[m_cmd_count].id = m_cmd_count + 1;
        m_cmd_table[m_cmd_count].func = func;
        m_cmd_table[m_cmd_count].data = data;

        m_cmd_table[m_cmd_count].params = m_cmd_table[m_cmd_count].response = m_cmd_table[m_cmd_count].descr = "";
        if ( params_string && params_string[0] ) {
            m_cmd_table[m_cmd_count].params = params_string;
        }
        if ( response_string && response_string[0] ) {
            m_cmd_table[m_cmd_count].response = response_string;
        }
        if ( description_string && description_string[0] ) {
            m_cmd_table[m_cmd_count].descr = description_string;
        }

        m_cmd_count++;
        return m_cmd_count;
    }
}

int HimCommand::assign_msg(const char * msg_string, const char * response_string, const char * description_string)
{
    if( m_msg_count >= HIM_MSG_TABLE_MAX ) {
        return 0;
    } else if ( !msg_string ) {
        return 0;
    } else if ( !msg_string[0] ) {
        return 0;
    } else {
        m_msg_table[m_msg_count].msg = msg_string;
        m_msg_table[m_msg_count].id = m_msg_count + 1;

        m_msg_table[m_msg_count].response = m_msg_table[m_msg_count].descr = "";
        if ( response_string && response_string[0] ) {
            m_msg_table[m_msg_count].response = response_string;
        }
        if ( description_string && description_string[0] ) {
            m_msg_table[m_msg_count].descr = description_string;
        }

        m_msg_count++;
        return m_msg_count;
    }
}

void HimCommand::update()
{
    int incomingByte = 0;
    
    while(Serial.available() > 0) {
        incomingByte = Serial.read();

        if ( (incomingByte > 32) && (incomingByte < 127) ) {
            // NORMAL CHARACTER
            if(m_echo) him_logd("%c", incomingByte);
            if(m_cmd_line_char_count < HIM_CMD_LINE_LENGTH) {
                m_state = HIM_CMD_STATE_READING;
                if ( m_cmd_line_char_count == 0 || m_cmd_line[m_cmd_line_char_count - 1] == 0 ) {
                    if( m_arg_count < HIM_CMD_ARG_MAX) {
                        m_arg_value[m_arg_count] = &m_cmd_line[m_cmd_line_char_count];
                    } else {
                        m_state = HIM_CMD_STATE_ERROR;
                    }
                }
                m_cmd_line[m_cmd_line_char_count] = incomingByte;
                m_cmd_line_char_count++;
            } else {
                m_state = HIM_CMD_STATE_ERROR;
            }

        } else if ( (incomingByte == 32) || (incomingByte == 9) ||    // SPACE or TAB ot 
                    (incomingByte == 10) || (incomingByte == 13) ) {  // ENTER
            if(m_echo && (incomingByte == 32) || (incomingByte ==  9)) him_logd(" ");
            if(m_echo && (incomingByte == 10) || (incomingByte == 13)) him_logd("\n");
            if ( (m_cmd_line_char_count != 0) && (m_cmd_line[m_cmd_line_char_count - 1] != 0) ) {
                if ( m_cmd_line_char_count < HIM_CMD_LINE_LENGTH ) {
                    m_cmd_line[m_cmd_line_char_count] = 0;
                    m_cmd_line_char_count++;
                    m_arg_count++;
                } else {
                    m_state = HIM_CMD_STATE_ERROR;
                }
            }

        } else if (incomingByte == 27) {
            // ESC
            if(m_echo) him_logd("\r");
            clear_cmd_line();
        }

        if(m_cmd_line_char_count >= HIM_CMD_LINE_LENGTH) {
            m_cmd_line[HIM_CMD_LINE_LENGTH - 1] = 0;
            m_cmd_line_char_count = HIM_CMD_LINE_LENGTH;
        }

        if ( (incomingByte == 10) || (incomingByte == 13) ) {
            // ENTER
            if ( (m_state = HIM_CMD_STATE_READING) && (m_arg_count > 0)) {
                char * cmd_value = m_arg_value[0];
                int cookie = -1;
                int cmd_id = -1;

                // check for a cookie
                bool cookie_found = false;
                if (cmd_value[0] == '#') {
                    cookie_found = true;
                    cmd_value = &cmd_value[1];
                }
                for (int i=0; cookie_found && cmd_value[i]; i++) {
                    if ( (cmd_value[i] < '0') || (cmd_value[i] > '9')) {
                        if (i > 0 && cmd_value[i] == ':' ) {
                            cmd_value[i]  = 0;
                            string2int(cmd_value, cookie);
                            cmd_value = &cmd_value[i+1];
                            break;
                        }
                        cookie_found = false;
                        cmd_value = m_arg_value[0];
                    }
                }

                // check for a command id
                bool cmd_id_found = cookie_found;
                for (int i=0; cookie_found && cmd_id_found && cmd_value[i]; i++) {
                    if ( (cmd_value[i] < '0') || (cmd_value[i] > '9')) {
                        if (cmd_value[i] == ':' ) {
                            cmd_value[i]  = 0;
                            string2int(cmd_value, cmd_id);
                            cmd_value = &cmd_value[i+1];
                            break;
                        }
                        cmd_id_found = false;
                    }
                }

                // parse the command table
                int found = -1;
                for (int f=0; (found < 0) && (f<m_cmd_count); f++) {
                    if (cmd_id_found && cmd_id == m_cmd_table[f].id) {
                        found = f;
                        break;
                    } else {
                        int i=0,ii=0;
                        for (; cmd_value[i]; i++ ) {
                            if( cmd_value[i] == m_cmd_table[f].cmd[i] ) {
                                ii++;
                            } else {
                                break;
                            }
                        }
                        if( i=ii && cmd_value[i] == 0 && m_cmd_table[f].cmd[i] == 0 ) {
                            found = f;
                            break;
                        }
                    }
                }
                if (found < 0) {
                    him_cmd_response_cmd(cookie, 1, "unknown command\n");
                } else {
                    if ( cmd_id_found ) {
                        if (cmd_value[0] == 0) {
                            // assign the command name to the commandline arg 0
                            m_arg_value[0] = m_cmd_table[found].cmd;
                        } else {
                            // there are still characters after the cookie/cmd header without space
                            // this must be the first argument - move the complete arglist up
                            m_arg_count++;
                            if(m_arg_count >= HIM_CMD_ARG_MAX) {
                                found = -1;
                                m_state = HIM_CMD_STATE_ERROR;
                            } else {
                                for(int i=m_arg_count; i>=2; i--) {
                                    m_arg_value[i] = m_arg_value[i-1];
                                }
                                m_arg_value[1] = cmd_value;
                                m_arg_value[0] = m_cmd_table[found].cmd;
                            }
                        }
                    }
                    if (found >= 0) {
                        m_state = HIM_CMD_STATE_EXECUTING;
                        m_cmd_table[found].func(cookie, m_cmd_table[found].data);
                    }
                }
            }
            clear_cmd_line();
        }
    }
}



void HimCommand::response_cmd(int cookie, int res, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    HimLog.log(false, "\n"); 
    if (cookie >= 0) { 
        HimLog.log(false, "#%02d:%02d:", cookie, res);
    }
    HimLog.logv(false, false, (const void*)format, args);
    HimLog.log(false, "\n"); 

    va_end(args);
}

void HimCommand::response_msg(int msg_id, int res, bool use_tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    for (int i; i<HIM_MSG_TABLE_MAX; i++) {
        if (msg_id == m_msg_table[i].id) {
            if (use_tag) {
                HimLog.log(false, "\n"); 
                HimLog.log(false, "#%02d:%02d:", m_msg_table[i].id, res);
            } else {
                HimLog.log(false, "%s:%02d:", m_msg_table[i].msg, res);
            }
            HimLog.logv(false, false, (const void*)format, args);
            HimLog.log(false, "\n"); 
        }
    }

    va_end(args);
}

void HimCommand::response_msg(const char * msg_string, int res, bool use_tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    for (int i; i<HIM_MSG_TABLE_MAX; i++) {
        int c=0,cc=0;
        for (; msg_string[c]; c++ ) {
            if( msg_string[c] == m_msg_table[i].msg[c] ) {
                cc++;
            } else {
                break;
            }
        }
        if( c=cc && msg_string[c] == 0 && m_msg_table[i].msg[c] == 0 ) {
            if (use_tag) {
                HimLog.log(false, "\n"); 
                HimLog.log(false, "#%02d:%02d:", m_msg_table[i].id, res);
            } else {
                HimLog.log(false, "%s:%02d:", m_msg_table[i].msg, res);
            }
            HimLog.logv(false, false, (const void*)format, args);
            HimLog.log(false, "\n"); 
        }
    }

    va_end(args);
}



unsigned int HimCommand::getarg_count()
{
    if ( m_state != HIM_CMD_STATE_EXECUTING) {
        return 0;
    } else {
        return m_arg_count;
    }
}

bool HimCommand::getarg_int(int index, int &value)
{
    if ( m_state != HIM_CMD_STATE_EXECUTING) {
        return false;
    } else if (index >= m_arg_count) {
        return false;
    } else {
        return string2int(m_arg_value[index], value);
    }
}

bool HimCommand::getarg_uint(int index, unsigned int &value)
{
    if ( m_state != HIM_CMD_STATE_EXECUTING) {
        return false;
    } else if (index >= m_arg_count) {
        return false;
    } else {
        m_arg_value[index];

        return false;
    }
}

bool HimCommand::getarg_char(int index, char &value, unsigned int pos)
{
    if ( m_state != HIM_CMD_STATE_EXECUTING) {
        return false;
    } else if (index >= m_arg_count) {
        return false;
    } else {
        unsigned int length;
        for (length = 0; m_arg_value[index][length]; length++);
        if ( pos >= length ) {
            return false;
        } else {
            value = m_arg_value[index][pos];
            return true;
        }
    }
}

bool HimCommand::getarg_string(int index, char * value, int &length)
{
    if ( m_state != HIM_CMD_STATE_EXECUTING) {
        return false;
    } else if (index >= m_arg_count) {
        return false;
    } else {
        for ( int i=0; i<length; i++) {
            value[i] = m_arg_value[index][i];
            if ( value[i] == 0 ) {
                length = i;
                return true;
            }
        }

        value[length-1] = 0;
        return false;
    }
}

void HimCommand::clear_cmd_line()
{
    memset(m_cmd_line, 0, HIM_CMD_LINE_LENGTH);
    m_cmd_line_char_count = 0;

    m_state = HIM_CMD_STATE_READY;

    memset(m_arg_value, 0, HIM_CMD_ARG_MAX * sizeof(char*));
    m_arg_count = 0;
}

bool HimCommand::string2int(char * str, int &value)
{
    bool res = false;
    unsigned int v;

    if( str[0] == '-' ) {
        res = string2uint(&str[1], v);
        if ( res) {
            value = -v;
        }
    } else {
        res = string2uint(&str[0], v);
        if ( res) {
            value = v;
        }
    }

    return res;
}

bool HimCommand::string2uint(char * str, unsigned int &value)
{
    #define string2uint_STATE_SCAN_NONE 0
    #define string2uint_STATE_SCAN_DEC  1
    #define string2uint_STATE_SCAN_HEX  2
    #define string2uint_STATE_ERROR     3

    unsigned int v = 0;
    unsigned int state = string2uint_STATE_SCAN_NONE;
    for ( int i=0; str[i]; i++) {
        if ( state == string2uint_STATE_SCAN_NONE ) {
            if ( (str[i] == ' ')  ||  (str[i] == '\t') ) {
                state = string2uint_STATE_SCAN_NONE;
            } else if ( (str[i] >= '1') && (str[i] <= '9') ) {
                state = string2uint_STATE_SCAN_DEC;
            } else if ( (str[i] == 'x')  ||  (str[i] == 'X')  &&  (i>0)  &&  (str[i-1] == '0')) {
                state = string2uint_STATE_SCAN_HEX;
            } else if ( str[i] == '0' ) {
                state = string2uint_STATE_SCAN_NONE;
            } else {
                state = string2uint_STATE_ERROR;
            }
        }
        if ( state == string2uint_STATE_SCAN_DEC ) {
            if ( (str[i] >= '0') && (str[i] <= '9') ) {
                v *= 10;
                v += (str[i] - '0');
            } else {
                state = string2uint_STATE_ERROR;
            }
        } else if ( state == string2uint_STATE_SCAN_HEX ) {
            if ( (str[i] >= '0') && (str[i] <= '9') ) {
                v *= 10; v += (str[i] - '0');
            } else switch (str[i]) {
                case 'a':
                case 'A':
                    v *= 10; v += 10;
                    break;
                case 'b':
                case 'B':
                    v *= 10; v += 11;
                    break;
                case 'c':
                case 'C':
                    v *= 10; v += 12;
                    break;
                case 'd':
                case 'D':
                    v *= 10; v += 13;
                    break;
                case 'e':
                case 'E':
                    v *= 10; v += 14;
                    break;
                case 'f':
                case 'F':
                    v *= 10; v += 15;
                    break;
                default:
                    state = string2uint_STATE_ERROR;
            }
        } else if ( state == string2uint_STATE_ERROR ) {
            value = 0;
            return false;
        }
    }
    value = v;
    return true;
}

HimCommand HimCmd = HimCommand();
int cookie = 0;
int res = 1;

