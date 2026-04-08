#include "Arduino.h"

#include <him_cmd.h>


bool section_dump = true;
int section_current = 1;
const int section_min = 1;
const int section_max = 8;

//
// custom command and message functions
//
int msg_next(char** params, int param_count, void* data)
{
    int res = HIM_CMD_OK;

    int next_section = *(int *)data;
    next_section++;
    if(next_section>section_max) next_section = section_max;
    *(int *)data = next_section;

    section_dump = true;

    return res;
}

int msg_back(char** params, int param_count, void* data)
{
    int res = HIM_CMD_OK;

    int next_section = *(int *)data;
    next_section--;
    if(next_section<section_min) next_section = section_min;
    *(int *)data = next_section;

    section_dump = true;

    return res;
}

int msg_echo(char** params, int param_count, void* data)
{
    int res = HIM_CMD_OK;

    him_logd("command msg_echo:\n");
    for (int i=0; i<param_count; i++) {
        him_logd("  param %d: %s\n", i, params[i]);
    }

    return res;
}

int cmd_echo(char** params, int param_count, void* data, int cookie)
{
    int res = HIM_CMD_OK;

    // send the response
    him_logd("\n");
    him_logd("$%d", res);
    if (cookie>0) him_logd(",%d", cookie);
    for (int i=0; i<param_count; i++) {
        him_logd("%c", (i==0) ? ':' : ',');
        him_logd("%s", params[param_count-i-1]);
    }
    him_logd("\n");

    return res;
}

int resp_dump(int cookie, int errorcode, char** params, int param_count, void* data)
{
    int res = HIM_CMD_OK;

    him_logd("received response:\n");
    him_logd("  cookie: %d\n", cookie);
    him_logd("  errorcode: %d\n", errorcode);
    for (int i=0; i<param_count; i++) {
        him_logd("  param %d: %s\n", i, params[i]);
    }

    return res;
}


int msg_send(char** params, int param_count, void* data)
{
    int res = HIM_CMD_OK;

    static int cookie = 0;

    HimCommand * self = reinterpret_cast<HimCommand*>(data);

    him_logd("send a command\n");
    if (param_count == 0) {
        him_logd("  no command name parameter given\n");
    } else {
        him_logd("  command name: %s\n", params[0]);
        for (int i=1; i<param_count; i++) {
            him_logd("  param %d: %s\n", i-1, params[i]);
        }

        if (++cookie > 999) cookie = 1;

        res = self->assign_resp(cookie, resp_dump, data);

        him_logd("!%s,%d", params[0], cookie);
        for (int i=1; i<param_count; i++) {
            him_logd("%c", (i>1)?',':':' );
            him_logd("%s", params[i]);
        }
        him_logd("\n");
    }

    return res;
}



void setup() {
    him_cmd_interpreter_init(9600, true, "commandline example", "1.2.3", HIM_CMD_RESP_TIMEOUT_NONE);

    him_cmd_assign_msg(
        "next",
        msg_next,
        (void*)&section_current,
        "",
        "continue to the next example section"
    );                        

    him_cmd_assign_msg(
        "back",
        msg_back,
        (void*)&section_current,
        "",
        "jump back to the previous example section"
    );                        

    him_cmd_assign_msg(
        "echo",
        msg_echo,
        NULL,
        "[params]..",
        "Dumps a list of all paramters in human readable format"
    );                        

    him_cmd_assign_cmd(
        "echo",
        cmd_echo,
        NULL,
        "[params]..",
        "Responds OK and all given parameters",
        "[params].."
    );                        

    him_cmd_assign_msg(
        "send",
        msg_send,
        &HimCmd,
        "<command>,[params]..",
        "Send a command to the host"
    );
}



//
// main loop function
//
void loop() {

    him_cmd_interpreter_parse();


    if (section_dump  && section_current == 1) {
        him_logd("\n");
        him_logd("Section %d - Introduction\n", section_current);
        him_logd("========================\n");
        him_logd("The most of my projects are implementing some kind of funtionality within an Arduino Board.\n");
        him_logd("But the host almost always needs to communicate to Arduino Board via the serial USB interface\n");
        him_logd("to control the functionality and vice versa.\n");
        him_logd("Using the simple logging library him_log and and some manual parsing can accomplish this task.\n");
        him_logd("But since this is a common use case there is a need to have an easy to use generic command interface library.\n");
        him_logd("\n");
        him_logd("The basic requirements are:\n");
        him_logd("- commands to the Arduino should be send in a human readable format that they can be typed manually in the commandline\n");
        him_logd("- generic functions shall be called when the according command was sent\n");
        him_logd("- the result of a command shall generate a human readable reply message\n");
        him_logd("- the host may also provide a similar interface to enable the Arduino to pass similar commands and information to the host\n");
        him_logd("- the format strings shall also provide options for simple parsability for non human communication between Arduino and host (and vice versa)\n");
        him_logd("\n");
        him_logd("type '#next' to continue\n");
        him_logd("type '#back' for the previous section\n");
        him_logd("\n> ");
        section_dump = false;

    } else if (section_dump  && section_current == 2) {
        him_logd("\n");
        him_logd("Section %d - messages\n", section_current);
        him_logd("====================\n");
        him_logd("Messages shall be interpreted by the Arduino board and call a previously assigned callback function.\n");
        him_logd("The interpreter shall detect the correct function and forward all paramters accordingly.\n");
        him_logd(" - messages shall start with a the character '#'\n");
        him_logd(" - followed by a colon and comman separated parameters\n");
        him_logd(" - all parameters are treated as strings\n");
        him_logd("\n");
        him_logd(" - #<message>[:param][,param]\n");
        him_logd("\n");
        him_logd(" - example:\n");
        him_logd(" - #echo:one,two,3\n");
        him_logd(" -   'echo' dumps a list of all paramters in human readable format\n");
        him_logd("\n");
        him_logd("Give it a try :-)\n");
        him_logd("Just send the 'echo' message\n");
        him_logd("\n");
        him_logd("type '#next' to continue\n");
        him_logd("type '#back' for the previous section\n");
        him_logd("\n> ");
        section_dump = false;

    } else if (section_dump  && section_current == 3) {
        him_logd("\n");
        him_logd("Section %d - commands\n", section_current);
        him_logd("====================\n");
        him_logd("Commands shall be interpreted by the Arduino board and call a previously assigned callback function.\n");
        him_logd("In contrast to the messages a command shall return a formated response to the host.\n");
        him_logd(" - commands shall start with a the character '!'\n");
        him_logd(" - followed by colon and comman separated parameters\n");
        him_logd(" - all parameters are treated as strings\n");
        him_logd("\n");
        him_logd(" - !<command>[:param][,param]\n");
        him_logd("\n");
        him_logd(" - the response shall start with a the character '$'\n");
        him_logd(" - followed by colon and comman separated parameters\n");
        him_logd("\n");
        him_logd(" - $<errorcode>[:param][,param]\n");
        him_logd("\n");
        him_logd(" - example\n");
        him_logd(" - !echo:one,two,3\n");
        him_logd(" -   responds the errorcode 0 (OK) and the list of all paramters in reverse order\n");
        him_logd(" - $0:3,two,one\n");
        him_logd("\n");
        him_logd("Give it a try :-)\n");
        him_logd("Just send the 'echo' command\n");
        him_logd("\n");
        him_logd("type '#next' to continue\n");
        him_logd("type '#back' for the previous section\n");
        him_logd("\n> ");
        section_dump = false;

    } else if (section_dump  && section_current == 4) {
        him_logd("\n");
        him_logd("Section %d - cookies\n", section_current);
        him_logd("===================\n");
        him_logd("Assuming the host sends multiple commands to the Arduino board and the Arduino responds in arbitrary order.\n");
        him_logd("The host needs to distinguish between the different responses. Therefore an optional identificator can be added.\n");
        him_logd("The so called cookie, can be an arbitrary positive integer number which is also included in the response format.\n");
        him_logd("The intention for this optional feature is to enable reliable non-human communication.\n");
        him_logd(" - The complete command and response format finally looks like this\n");
        him_logd("\n");
        him_logd(" - !<command>[,cookie][:param][,param]\n");
        him_logd(" - $<errorcode>[,cookie][:param][,param]\n");
        him_logd("\n");
        him_logd("Give it a try :-)\n");
        him_logd("Just add a cookie to the 'echo' command and have a closer look at the response\n");
        him_logd("\n");
        him_logd("type '#next' to continue\n");
        him_logd("type '#back' for the previous section\n");
        him_logd("\n> ");
        section_dump = false;

    } else if (section_dump  && section_current == 5) {
        him_logd("\n");
        him_logd("Section %d - Build-in functions and cmd/msg IDs\n", section_current);
        him_logd("==============================================\n");
        him_logd("The him_cmd library stores the assigned function and messages within two internal tables. Both table are storing the name,\n");
        him_logd("the assigned function, a detailed functional and parameter description as well as a individual command/message ID.\n");
        him_logd("Since the command/message ID is usually shorter that the name the ID can be used to call commands/messages.\n");
        him_logd("This is usegfull especially when the functions are not called manuallty by the host.\n");
        him_logd("\n");
        him_logd("To provide this table information internal buld-in functions can be called.\n");
        him_logd(" - !version[,cookie]\n");
        him_logd("     Returns the interpreter version as well as project name and version\n");
        him_logd("     Return parameter: <interpreter_version>[,project_name],[,project_version]\n");
        him_logd("\n");
        him_logd(" - !info[,cookie]:[msg|cmd]\n");
        him_logd("     Returns a list of all registered commands/messages in JSON format\n");
        him_logd("\n");
        him_logd(" - #info:[msg|cmd]\n");
        him_logd("     Dumps a list of all registered commands/messages in human readable format\n");
        him_logd("\n");
        him_logd("Give it a try :-)\n");
        him_logd("Have a look at the command and message table and call 'echo' by its id\n");
        him_logd("\n");
        him_logd("type '#next' to continue\n");
        him_logd("type '#back' for the previous section\n");
        him_logd("\n> ");
        section_dump = false;

    } else if (section_dump  && section_current == 6) {
        him_logd("\n");
        him_logd("Section %d - Arduino to Host communication\n", section_current);
        him_logd("=========================================\n");
        him_logd("To send a command to the host a small helper message function called 'send' was implemented in this example.\n");
        him_logd("This message function generates a command that is sent to the host. The first message parameter is treated as command string.\n");
        him_logd("All following parameter are added to the command.\n");
        him_logd("\n");
        him_logd("- message \n");
        him_logd("-   #send:test,one,two,3\n");
        him_logd("- generates command\n");
        him_logd("-   !test,<cookie>:one,two,3\n");
        him_logd("- and sends this command towards the host\n");
        him_logd("\n");
        him_logd("- Internally the cookie and a function pointer to a response function are stored in an internal lookup table.\n");
        him_logd("- The Arduino expects to receive a response from the host including the according cookie to call the response function.\n");
        him_logd("- When this happens the stored response callback function is executed with the parameters of the response message.\n");
        him_logd("\n");
        him_logd("- response example\n");
        him_logd("-   $1,<cookie>:four,five,6\n");
        him_logd("\n");
        him_logd("- In this example a simple function was registerd to the send command which just dumps the received response parameters in human readable format.\n");
        him_logd("\n");
        him_logd("Give it a try :-)\n");
        him_logd("Use the send message and inspect the command that is generated towards the host.\n");
        him_logd("You have to mimic the response of the host and type it manually - YOU are currently the host ;-)\n");
        him_logd("When your response was correct you should see the output of the response function\n");
        him_logd("\n");
        him_logd("type '#next' to continue\n");
        him_logd("type '#back' for the previous section\n");
        him_logd("\n> ");
        section_dump = false;

    } else if (section_dump  && section_current == 7) {
        him_logd("\n");
        him_logd("Section %d - Implementation caveats\n", section_current);
        him_logd("==================================\n");
        him_logd("The him_cmd library internally instanciates a global static object\n");
        him_logd("which implements the generic functionality and the described build-in functions.\n");
        him_logd("\n");
        him_logd("The function interface is encapsulated in just some simple defines:\n");
        him_logd("To be used within the setup() function\n");
        him_logd(" - Initalize the him_cmd instance\n");
        him_logd("     him_cmd_interpreter_init(baudrate, echo, name, version, timeout)\n");
        him_logd(" - Assign your commands and messages to the him_cmd interpreter\n");
        him_logd("     him_cmd_assign_msg(name, msg_func, data, description_params, description_general)\n");
        him_logd("     him_cmd_assign_cmd(name, cmd_func, data, description_params, description_general, description_response)\n");
        him_logd("To be used within the loop() function\n");
        him_logd(" - Call the parse functions periodically\n");
        him_logd("     him_cmd_interpreter_parse()\n");
        him_logd(" - Functions to send functions to the host\n");
        him_logd("     him_cmd_send_msg(cmd, param_format, ... )\n");
        him_logd("     him_cmd_send_cmd(msg, msg_func, msg_data, param_format, ... )\n");
        him_logd("\n");
        him_logd("Please also inspect this example to get an impression how to use the him_log library.\n");
        him_logd("\n");
        him_logd("Also regard the him_cmd_config.h header which defines internal static tables to tweak your compilation size.\n");
        him_logd("\n");
        him_logd("type '#next' to continue\n");
        him_logd("type '#back' for the previous section\n");
        him_logd("\n> ");
        section_dump = false;

    } else if (section_dump  && section_current == 8) {
        him_logd("\n");
        him_logd("That's all Folks\n");
        him_logd("================\n");
        him_logd("\n");
        him_logd("Have fun using this him_cmd library.\n");
        him_logd(":-)\n");
        him_logd("\n");
        him_logd("Bests\n");
        him_logd("\n");
        him_logd("Sven\n");
        him_logd("\n");
        him_logd("\n> ");
        section_dump = false;

    } else {
        delay(100);
    }
}
