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

#include "him_log.h"
#include "him_cmd.h"



HimCommand::HimCommand()
{
    clear_cmd_line();
    m_echo = false;
}

HimCommand::~HimCommand()
{
}

void HimCommand::update()
{
    int incomingByte = 0;
    
    while(Serial.available() > 0) {
        incomingByte = Serial.read();

        if ( (incomingByte > 32) && (incomingByte < 127) ) {
            // NORMAL CHARACTER
            if(m_echo) him_logd("%c", incomingByte);
            if(m_cmd_line_char_count < HIM_CMD_LINE_MAX) {
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

        } else if ( (incomingByte == 32) || (incomingByte == 9) || (incomingByte == 13)) {
            // SPACE or TAB ot ENTER
            if(m_echo) {
                if(incomingByte == 13) {
                    him_logd("\n");
                } else {
                    him_logd(" ");
                }
            }
            if ( (m_cmd_line_char_count != 0) && (m_cmd_line[m_cmd_line_char_count - 1] != 0) ) {
                if ( m_cmd_line_char_count < HIM_CMD_LINE_MAX ) {
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

        if(m_cmd_line_char_count == HIM_CMD_LINE_MAX) {
            m_cmd_line[HIM_CMD_LINE_MAX - 1] = 0;
            m_cmd_line_char_count = HIM_CMD_LINE_MAX;
        }

        if (incomingByte == 13) {
            // ENTER
            if ( (m_state = HIM_CMD_STATE_READING) && (m_arg_count > 0)) {
                // parse the command table
                int found = -1;
                for (int f=0; (found < 0) && (f<m_cmd_func_count); f++) {
                    for (int c=0; c<HIM_CMD_STRING_MAX; c++) {
                        if( m_cmd_func_table[f].cmd[c] != m_arg_value[0][c] ) {
                            break;
                        } else if( m_cmd_func_table[f].cmd[c] == 0 ) {
                            found = f;
                            break;
                        }
                    }
                }
                if (found >= 0) {
                    m_state = HIM_CMD_STATE_EXECUTING;
                    m_cmd_func_table[found].cmd_func(m_cmd_func_table[found].p_data);
                }
            }
            clear_cmd_line();
        }
    }
}

int HimCommand::register_func(char * cmd_string, cmd_func_t cmd_func, void * p_data)
{
    if( m_cmd_func_count >= HIM_CMD_FUNC_MAX ) {
        return 0;
    } else if ( !cmd_string ) {
        return 0;
    } else if ( !cmd_string[0] ) {
        return 0;
    } else if ( !cmd_func ) {
        return 0;
    } else {
        bool inserted = false;
        for ( int i=0; i<HIM_CMD_STRING_MAX; i++) {
            m_cmd_func_table[m_cmd_func_count].cmd[i] = cmd_string[i];
            if(m_cmd_func_table[m_cmd_func_count].cmd[i] == 0) {
                inserted = true;
                break;
            }
        }
        if ( inserted ) {
            m_cmd_func_table[m_cmd_func_count].cmd_func = cmd_func;
            m_cmd_func_table[m_cmd_func_count].p_data = p_data;
            m_cmd_func_count++;
            return m_cmd_func_count;
        } else {
            return 0;
        }
    }
}

void HimCommand::set_echo(bool value)
{
    m_echo = value;    
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
        for ( int i=0; i<length && i<HIM_CMD_STRING_MAX; i++) {
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
    memset(m_cmd_line, 0, HIM_CMD_LINE_MAX);
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

