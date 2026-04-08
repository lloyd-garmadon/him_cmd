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
*       2026/04/02
*
* CHANGES:
*
***********************************************************************/

#define HIM_LIBRARY_COMPILE

#include "him_cmd.h"



//
// Build-in Commands
//

int HimCommand::cmd_version(char** params, int param_count, void* data, int cookie)
{
    int res = HIM_CMD_OK;

    HimCommand * self = reinterpret_cast<HimCommand*>(data);

    him_logd("\n");
    him_logd("$%d", res);
    if (cookie>0) him_logd(",%d", cookie);
    him_logd(":\"%s\"", HIM_CMD_VERSION);
    if (self->m_project_name) him_logd(",\"%s\"", self->m_project_name);
    if (self->m_project_name) him_logd(",\"%s\"", self->m_project_version);
    him_logd("\n");

    return res;
}

int HimCommand::cmd_info(char** params, int param_count, void* data, int cookie)
{
    int res = HIM_CMD_OK;

    HimCommand * self = reinterpret_cast<HimCommand*>(data);

    bool cmd_dump = 0;
    bool msg_dump = 0;
    if (param_count == 0) {
        cmd_dump = 1;
        msg_dump = 1;
    } else if(param_count == 1) {
        if (!strcmp(params[0], "cmd")) {
            cmd_dump = 1;
        } else if (!strcmp(params[0], "msg")) {
            msg_dump = 1;
        }
    }

    him_logd("\n");
    him_logd("$%d", res);
    if (cookie>0) him_logd(",%d", cookie);
    him_logd(":");
    him_logd("[");
    if (cmd_dump) {
        him_logd("[");
#if HIM_CMD_TABLE_MAX > 0
        for(unsigned int i=0; i<self->m_cmd_count; i++) {
            if (i>0) him_logd(",");
            him_logd("[%s,%d,\"'%s'\",\"'%s'\",\"'%s'\"]", self->m_cmd_table[i].name, self->m_cmd_table[i].id, self->m_cmd_table[i].description_params, self->m_cmd_table[i].description_general, self->m_cmd_table[i].description_response);
        }
#endif
        him_logd("]");
    }
    if (cmd_dump && msg_dump) him_logd(",");
    if (msg_dump) {
        him_logd("[");
#if HIM_MSG_TABLE_MAX > 0
        for(unsigned int i=0; i<self->m_msg_count; i++) {
            if (i>0) him_logd(",");
            him_logd("[%s,%d,\"'%s'\",\"'%s'\"]", self->m_msg_table[i].name, self->m_msg_table[i].id, self->m_msg_table[i].description_params, self->m_msg_table[i].description_general);
        }
#endif
        him_logd("]");
    }
    him_logd("]\n");

    return res;
}


int HimCommand::msg_info(char** params, int param_count, void* data)
{
    int res = HIM_CMD_OK;

    HimCommand * self = reinterpret_cast<HimCommand*>(data);

    bool cmd_dump = 0;
    bool msg_dump = 0;
    bool resp_dump = 0;
    if (param_count == 0) {
        cmd_dump = 1;
        msg_dump = 1;
    } else if(param_count == 1) {
        if (!strcmp(params[0], "cmd")) {
            cmd_dump = 1;
        } else if (!strcmp(params[0], "msg")) {
            msg_dump = 1;
        } else if (!strcmp(params[0], "resp")) {
            resp_dump = 1;
        }
    }

    if (cmd_dump) {
        him_logd("Command table\n");
#if HIM_CMD_TABLE_MAX <= 0
        him_logd("  not implemented\n");
#else
        for(unsigned int i=0; i<self->m_cmd_count; i++) {
            him_logd("  %s:\n", self->m_cmd_table[i].name);
            him_logd("    id:          %d\n", self->m_cmd_table[i].id);
            him_logd("    params:      %s\n", self->m_cmd_table[i].description_params);
            him_logd("    description: %s\n", self->m_cmd_table[i].description_general);
            him_logd("    reply:       %s\n", self->m_cmd_table[i].description_response);
        }
#endif
    }
    if (msg_dump) {
        him_logd("Message table\n");
#if HIM_MSG_TABLE_MAX <= 0
        him_logd("  not implemented\n");
#else
        for(unsigned int i=0; i<self->m_msg_count; i++) {
            him_logd("  %s:\n", self->m_msg_table[i].name);
            him_logd("    id:          %d\n", self->m_msg_table[i].id);
            him_logd("    params:      %s\n", self->m_msg_table[i].description_params);
            him_logd("    description: %s\n", self->m_msg_table[i].description_general);
        }
#endif
    }
    if (resp_dump) {
        him_logd("Response table\n");
#if HIM_RESP_TABLE_MAX <= 0
        him_logd("  not implemented\n");
#else
        for(unsigned int i=0; i<HIM_RESP_TABLE_MAX; i++) {
            him_logd("  Entry %d:\n", i );
            him_logd("    cookie:      %d\n", self->m_resp_table[i].cookie);
            him_logd("    timestamp:   %D\n", self->m_resp_table[i].timestamp);
        }
#endif
    }

    return res;
}



//
// HimCommand constructur/destructor
//

HimCommand::HimCommand()
{
    m_project_name = 0;
    m_project_version = 0;

    him_cmd_assign_cmd(
        "version",
        HimCommand::cmd_version,
        (void*)this,
        "",
        "Returns the project name and version string",
        "<project_name>[,project_version][,interpreter_version]"
    );

    him_cmd_assign_cmd(
        "info",
        HimCommand::cmd_info,
        (void*)this,
        "<cmd|msg>",
        "Returns a list of all registered commands/messages in JSON format",
        "[[cmd_name,id,params,description,response],...],[[msg_name,id,params,description],...]"
    );                        

    him_cmd_assign_msg(
        "info",
        HimCommand::msg_info,
        (void*)this,
        "<cmd|msg|resp>",
        "Dump a list of all registered commands/messages in human readable format"
    );                        

#if HIM_RESP_TABLE_MAX > 0
    memset(&m_resp_table, 0, sizeof(struct resp_table_s) * HIM_RESP_TABLE_MAX);
#endif

    cmd_line_clear();
}

HimCommand::~HimCommand()
{
}



//
// HimCommand functions
//
int HimCommand::assign_cmd(const char * name, cmd_func_t func, void * data, const char * description_params, const char * description_general, const char * description_response)
{
#if HIM_CMD_TABLE_MAX <= 0
    return HIM_CMD_ERROR_NOTIMPLEMENTED;
#else

    int res = HIM_CMD_OK;

    if( m_cmd_count >= HIM_CMD_TABLE_MAX ) {
        return HIM_CMD_ERROR_TABLE_FULL;
    } else if ( !name ) {
        return HIM_CMD_ERROR_PARAM;
    } else if ( !name[0] ) {
        return HIM_CMD_ERROR_PARAM;
    } else if ( !func ) {
        return HIM_CMD_ERROR_PARAM;
    } else {
        m_cmd_table[m_cmd_count].name = name;
        m_cmd_table[m_cmd_count].id   = m_cmd_count + 1;
        m_cmd_table[m_cmd_count].func = func;
        m_cmd_table[m_cmd_count].data = data;
        m_cmd_table[m_cmd_count].description_params   = "";
        m_cmd_table[m_cmd_count].description_general  = "";
        m_cmd_table[m_cmd_count].description_response = "";

        if ( description_params && description_params[0] ) {
            m_cmd_table[m_cmd_count].description_params = description_params;
        }
        if ( description_general && description_general[0] ) {
            m_cmd_table[m_cmd_count].description_general = description_general;
        }
        if ( description_response && description_response[0] ) {
            m_cmd_table[m_cmd_count].description_response = description_response;
        }

        m_cmd_count++;
    }

    return res;
#endif
}

int HimCommand::assign_msg(const char * name, msg_func_t func, void * data, const char * description_params, const char * description_general)
{
#if HIM_MSG_TABLE_MAX <= 0
    return HIM_CMD_ERROR_NOTIMPLEMENTED;
#else

    int res = HIM_CMD_OK;

    if( m_msg_count >= HIM_MSG_TABLE_MAX ) {
        return HIM_CMD_ERROR_TABLE_FULL;
    } else if ( !name ) {
        return HIM_CMD_ERROR_PARAM;
    } else if ( !name[0] ) {
        return HIM_CMD_ERROR_PARAM;
    } else if ( !func ) {
        return HIM_CMD_ERROR_PARAM;
    } else {
        m_msg_table[m_msg_count].name = name;
        m_msg_table[m_msg_count].id   = m_msg_count + 1;
        m_msg_table[m_msg_count].func = func;
        m_msg_table[m_msg_count].data = data;
        m_msg_table[m_msg_count].description_params   = "";
        m_msg_table[m_msg_count].description_general  = "";

        if ( description_params && description_params[0] ) {
            m_msg_table[m_msg_count].description_params = description_params;
        }
        if ( description_general && description_general[0] ) {
            m_msg_table[m_msg_count].description_general = description_general;
        }

        m_msg_count++;
    }

    return res;
#endif
}

int HimCommand::assign_resp(int cookie, resp_func_t func, void * data)
{
#if HIM_RESP_TABLE_MAX <= 0
    return HIM_CMD_ERROR_NOTIMPLEMENTED;
#else
    int res = HIM_CMD_OK;

    if (!func) {
        // no need to assing ressponse function when there is no function
        return HIM_CMD_OK;
    }

    int index = -1;
    for (int i=0; i<HIM_RESP_TABLE_MAX; i++) {
        if (m_resp_table[i].timestamp == 0) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        res = HIM_CMD_ERROR_TABLE_FULL;
    } else {
        m_resp_table[index].timestamp = millis();
        m_resp_table[index].cookie = cookie;
        m_resp_table[index].func = func;
        m_resp_table[index].data = data;
    }

    return res;
#endif
}



int HimCommand::send_msg(const char * name, const char * format, ...)
{
    va_list args;
    va_start(args, format);

    HimLog.log(false, "\n#:%s:", name);
    HimLog.logv(false, false, (const void*)format, args);
    HimLog.log(false, "\n");

    va_end(args);

    return HIM_CMD_OK;
}

int HimCommand::send_msg(int id, const char * format, ...)
{
    va_list args;
    va_start(args, format);

    HimLog.log(false, "\n#:%d:", id);
    HimLog.logv(false, false, (const void*)format, args);
    HimLog.log(false, "\n");

    va_end(args);

    return HIM_CMD_OK;
}

int HimCommand::send_cmd(const char * name, resp_func_t func, void * data, const char * format, ...)
{
    int cookie = get_cookie();

    int res = assign_resp(cookie, func, data);

    va_list args;
    va_start(args, format);

    HimLog.log(false, "\n!:%s,%d:", name, cookie);
    HimLog.logv(false, false, (const void*)format, args);
    HimLog.log(false, "\n");

    va_end(args);

    return res;
}

int HimCommand::send_cmd(int id, resp_func_t func, void * data, const char * format, ...)
{
    int cookie = get_cookie();

    int res = assign_resp(cookie, func, data);

    va_list args;
    va_start(args, format);

    HimLog.log(false, "\n!:%d,%d:", id, cookie);
    HimLog.logv(false, false, (const void*)format, args);
    HimLog.log(false, "\n");

    va_end(args);

    return HIM_CMD_OK;

    return res;
}



#define HIM_CMD_LINE_STATE_PARSE_START   0
#define HIM_CMD_LINE_STATE_PARSE_COOKIE  1
#define HIM_CMD_LINE_STATE_PARSE_ID      2
#define HIM_CMD_LINE_STATE_PARSE_ARGS    3
#define HIM_CMD_LINE_STATE_IGNORE       -1
#define HIM_CMD_LINE_STATE_ERROR        -2
#define HIM_CMD_LINE_STATE_OVERFLOW     -3

void HimCommand::cmd_line_init(int baudrate, bool echo, const char * project_name, const char * project_version, long timeout)
{

    if (project_name != NULL || project_name[0] != 0 ) {
        m_project_name = project_name;
    }

    if (project_version != NULL || project_version[0] != 0 ) {
        m_project_version = project_version;
    }

    m_interpreter.echo = echo;

    m_interpreter.resp_timeout = timeout;

    Serial.begin(baudrate);

    cmd_line_clear();
}

void HimCommand::cmd_line_clear()
{
    memset(m_interpreter.char_line, 0, sizeof(char) * HIM_CMD_LINE_LENGTH);
    m_interpreter.char_count = 0;

    m_interpreter.arg.name = 0;
    m_interpreter.arg.id = 0;
    m_interpreter.arg.cookie = 0;
    memset(m_interpreter.arg.param, 0, sizeof(char*) * HIM_CMD_LINE_PARAM_MAX);
    m_interpreter.arg.param_count = 0;

    m_interpreter.state = HIM_CMD_LINE_STATE_PARSE_START;

    if (m_interpreter.echo) {
        him_logd("> ");
    }   
}

void HimCommand::cmd_line_parse()
{
    int received_char = 0;
    
    while(Serial.available() > 0) {
        received_char = Serial.read();

        if (received_char == 27) {       
            // received ESC
            if (m_interpreter.echo) {
                him_logd("\n");
            }   
            cmd_line_clear();

        } else if ( (received_char == 10) || (received_char == 13) ) {   
            // received ENTER
            if (m_interpreter.echo) {
                him_logd("\n");
            }   
            if (m_interpreter.state == HIM_CMD_LINE_STATE_IGNORE) {
                him_logd_error("no format recognized - ignore\n");
            } else if (m_interpreter.state == HIM_CMD_LINE_STATE_ERROR) {
                him_logd_error("malformed format - ignore\n");
            } else if (m_interpreter.state == HIM_CMD_LINE_STATE_OVERFLOW) {
                him_logd_error("commandline buffer full - ignored\n");
            } else if (((m_interpreter.state == HIM_CMD_LINE_STATE_PARSE_ARGS)) ||
                ((m_interpreter.state == HIM_CMD_LINE_STATE_PARSE_ID) && (m_interpreter.arg.name || m_interpreter.arg.id)) ||
                ((m_interpreter.state == HIM_CMD_LINE_STATE_PARSE_COOKIE) && (m_interpreter.arg.name || m_interpreter.arg.id))
               ) {
                if (m_interpreter.state == HIM_CMD_LINE_STATE_PARSE_ARGS) {
                    // when parsing the args, check if the last arg already has content
                    char p = *m_interpreter.arg.param[m_interpreter.arg.param_count];
                    if (p) {
                        // add a zero at the end to be safe
                        m_interpreter.char_line[m_interpreter.char_count++] = 0;
                        // increment the param count
                        m_interpreter.arg.param_count++;
                    }
                }
                int res = cmd_line_exec(
                            m_interpreter.arg.type,
                            m_interpreter.arg.name,
                            m_interpreter.arg.id,
                            m_interpreter.arg.cookie,
                            m_interpreter.arg.param,
                            m_interpreter.arg.param_count);
                if (res && m_interpreter.echo) {
                    him_logd_error("execution failed - %d\n", res);
                }   
            }
            cmd_line_clear();

        } else if ((received_char == 9) || (received_char == 32) || ((received_char > 32) && (received_char < 127))) { 
            // received TAB, SPACE or REGULAR CHARACTER
            if (m_interpreter.echo) {
                him_logd("%c", received_char);
            }   

            if ( m_interpreter.state == HIM_CMD_LINE_STATE_PARSE_START) {
                if ( (received_char == 9) || (received_char == 32) ) { 
                    // ignore whitespace
                } else if ( received_char == HIM_CMD_TYPE_CMD) { 
                    m_interpreter.arg.type = HIM_CMD_TYPE_CMD; 
                    m_interpreter.state = HIM_CMD_LINE_STATE_PARSE_ID;
                } else if ( received_char == HIM_CMD_TYPE_RESP) { 
                    m_interpreter.arg.type = HIM_CMD_TYPE_RESP; 
                    m_interpreter.state = HIM_CMD_LINE_STATE_PARSE_ID;
                } else if ( received_char == HIM_CMD_TYPE_MSG) { 
                    m_interpreter.arg.type = HIM_CMD_TYPE_MSG; 
                    m_interpreter.state = HIM_CMD_LINE_STATE_PARSE_ID;
                } else {
                    m_interpreter.state = HIM_CMD_LINE_STATE_IGNORE; 
                }

            } else if ( m_interpreter.state == HIM_CMD_LINE_STATE_PARSE_ID) {
                if ((received_char == 9) || (received_char == 32)) { 
                    // ignore whitespace
                } else if ((received_char == ':') || (received_char == ',')) { 
                    if (m_interpreter.arg.name) { 
                        m_interpreter.char_line[m_interpreter.char_count++] = 0;
                        m_interpreter.state = (received_char == ',') ? HIM_CMD_LINE_STATE_PARSE_COOKIE : HIM_CMD_LINE_STATE_PARSE_ARGS;
                    } else if (m_interpreter.arg.id) { 
                        m_interpreter.state = (received_char == ',') ? HIM_CMD_LINE_STATE_PARSE_COOKIE : HIM_CMD_LINE_STATE_PARSE_ARGS;
                    } else {
                        m_interpreter.state = HIM_CMD_LINE_STATE_ERROR; 
                    }
                } else if ((m_interpreter.arg.type != HIM_CMD_TYPE_RESP)  && (received_char >= '0') && (received_char <= '9')) { 
                    if (!m_interpreter.arg.name) { 
                        m_interpreter.arg.id *= 10;
                        m_interpreter.arg.id += (received_char - '0');
                    } else {
                        m_interpreter.char_line[m_interpreter.char_count++] = received_char;
                    }
                } else {
                    if (m_interpreter.arg.id) {
                        m_interpreter.state = HIM_CMD_LINE_STATE_ERROR; 
                    } else {
                        if (!m_interpreter.arg.name) { 
                            m_interpreter.arg.name = &m_interpreter.char_line[m_interpreter.char_count];
                        }
                        m_interpreter.char_line[m_interpreter.char_count++] = received_char;
                    }
                }

            } else if ( m_interpreter.state == HIM_CMD_LINE_STATE_PARSE_COOKIE) {
                if ( (received_char == 9) || (received_char == 32) ) { 
                    // ignore whitespace
                } else if (received_char == ':') { 
                    m_interpreter.state = HIM_CMD_LINE_STATE_PARSE_ARGS;
                } else if ((received_char >= '0') && (received_char <= '9')) { 
                    m_interpreter.arg.cookie *= 10;
                    m_interpreter.arg.cookie += (received_char - '0');
                } else {
                    m_interpreter.state = HIM_CMD_LINE_STATE_ERROR; 
                }

            } else if ( m_interpreter.state == HIM_CMD_LINE_STATE_PARSE_ARGS) {
                if (m_interpreter.arg.param[m_interpreter.arg.param_count] == 0) {
                    m_interpreter.arg.param[m_interpreter.arg.param_count] = &m_interpreter.char_line[m_interpreter.char_count];
                };

                if (m_interpreter.flag_inquotes) {
                    if (received_char == 32) { 
                        m_interpreter.flag_inquotes = false;
                    } else {
                        m_interpreter.char_line[m_interpreter.char_count++] = received_char;
                    }
                } else {
                    if ((received_char == 9) || (received_char == 32)) { 
                        // ignore whitespace
                    } else if (received_char == 32) { 
                        m_interpreter.flag_inquotes = true;
                    } else if (received_char == ',') { 
                        m_interpreter.char_line[m_interpreter.char_count++] = 0;
                        m_interpreter.arg.param_count++;
                    } else {
                        m_interpreter.char_line[m_interpreter.char_count++] = received_char;
                    }
                }
            }
        }

        // finally some checks for the next round
        if (m_interpreter.arg.param_count >= HIM_CMD_LINE_PARAM_MAX) {
            m_interpreter.state = HIM_CMD_LINE_STATE_OVERFLOW;  
        }
        if (m_interpreter.char_count >= HIM_CMD_LINE_LENGTH) {
            m_interpreter.state = HIM_CMD_LINE_STATE_OVERFLOW; 
        }
    }

    // clean up the response table when timeout was set
#if HIM_RESP_TABLE_MAX > 0
    if (m_interpreter.resp_timeout > 0) {
        long time = millis();
        for (int i=0; i<HIM_RESP_TABLE_MAX; i++) {
            if (m_resp_table[i].timestamp && ((time - m_resp_table[i].timestamp) > m_interpreter.resp_timeout)) {
                m_resp_table[i].timestamp = 0;
                m_resp_table[i].cookie = 0;
                m_resp_table[i].func = 0;
                m_resp_table[i].data = 0;
            }
        }
    }
#endif
}

int HimCommand::cmd_line_exec(char type, char * name, int id, int cookie, char** params, int param_count)
{
    if(type == HIM_CMD_TYPE_MSG) {
        return cmd_line_exec_msg(name, id, params, param_count);
    } else if(type == HIM_CMD_TYPE_CMD) {
        return cmd_line_exec_cmd(name, id, params, param_count, cookie);
    } else if(type == HIM_CMD_TYPE_RESP) {
        // the name represents the errorcode in this case
        return cmd_line_exec_resp(cookie, name, params, param_count);
    } else {
        return HIM_CMD_ERROR_TYPE;
    }
}

int HimCommand::cmd_line_exec_msg(char * name, int id, char** params, int param_count)
{
#if HIM_MSG_TABLE_MAX <= 0
    return HIM_CMD_ERROR_NOTIMPLEMENTED;
#else

    int res = HIM_CMD_OK;

    int index = -1;
    if (name && name[0]) {
        for (unsigned int i=0; i<m_msg_count; i++) {
            if (!strcmp(name, m_msg_table[i].name)) {
                index = i;
                break;
            }
        }
    } else if (id > 0) {
        for (unsigned int i=0; i<m_msg_count; i++) {
            if (id == m_msg_table[i].id) {
                index = i;
                break;
            }
        }
    }
    
    if (index < 0) {
        res = HIM_CMD_ERROR_MSG;
    } else {
        res = m_msg_table[index].func(params, param_count, m_msg_table[index].data);
    }

    return res;
#endif
}

int HimCommand::cmd_line_exec_cmd(char * name, int id, char** params, int param_count, int cookie)
{
#if HIM_CMD_TABLE_MAX <= 0
    return HIM_CMD_ERROR_NOTIMPLEMENTED;
#else

    int res = HIM_CMD_OK;

    int index = -1;
    if (name && name[0]) {
        for (unsigned int i=0; i<m_cmd_count; i++) {
            if (!strcmp(name, m_cmd_table[i].name)) {
                index = i;
                break;
            }
        }
    } else if (id > 0) {
        for (unsigned int i=0; i<m_cmd_count; i++) {
            if (id == m_cmd_table[i].id) {
                index = i;
                break;
            }
        }
    }
    
    if (index < 0) {
        res = HIM_CMD_ERROR_CMD;
    } else {
        res = m_cmd_table[index].func(params, param_count, m_cmd_table[index].data, cookie);
    }

    return res;
#endif
}

int HimCommand::cmd_line_exec_resp(int cookie, char * error, char** params, int param_count)
{
#if HIM_RESP_TABLE_MAX <= 0
    return HIM_CMD_ERROR_NOTIMPLEMENTED;
#else

    int res = HIM_CMD_OK;

    char *errorend;
    int errorcode = strtol(error, &errorend, 10);

    if (*errorend) {
        res = HIM_CMD_ERROR_PARAM;
        errorcode = -1;
    }

    int index = -1;
    for (int i=0; i<HIM_RESP_TABLE_MAX; i++) {
        if (m_resp_table[i].timestamp && (m_resp_table[i].cookie == cookie)){
            index = i;
            break;
        }
    }

    if (index < 0) {
        res = HIM_CMD_ERROR_RESP;
    } else {
        // execute the respond function
        if(m_resp_table[index].func) {
            res = m_resp_table[index].func(cookie, errorcode, params, param_count, m_resp_table[index].data);
        }
        m_resp_table[index].timestamp = 0;
        m_resp_table[index].cookie = 0;
        m_resp_table[index].func = 0;
        m_resp_table[index].data = 0;
    }

    return res;
#endif 
}

int HimCommand::get_cookie()
{
    static int cookie = 0;

    if (++cookie > 999) cookie = 1;

    return cookie;
}



//
// global static HimCommand instance
//

HimCommand HimCmd = HimCommand();
