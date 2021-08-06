#include "OpenT12.h"

/**
 * Function to read data from serial port
 * Functions to read from physical media should use this prototype:
 * int my_reader_function(char * data)
 */
int shell_reader(char* data)
{
    // Wrapper for Serial.read() method
    if (Serial.available()) {
        *data = Serial.read();
        return 1;
    }
    return 0;
}

/**
 * Function to write data to serial port
 * Functions to write to physical media should use this prototype:
 * void my_writer_function(char data)
 */
void shell_writer(char data)
{
    // Wrapper for Serial.write() method
    Serial.write(data);
}

int command_test(int argc, char** argv)
{
    int i;

    shell_println("-----------------------------------------------");
    shell_println("SHELL DEBUG / TEST UTILITY");
    shell_println("-----------------------------------------------");
    shell_println("");
    shell_printf("Received %d arguments for test command\r\n", argc);

    // Print each argument with string lenghts
    for (i = 0; i < argc; i++)
    {
        // Print formatted text to terminal
        shell_printf("%d - \"%s\" [len:%d]\r\n", i, argv[i], strlen(argv[i]));
    }

    return SHELL_RET_SUCCESS;
}

void shellInit(void) {
    shell_init(shell_reader, shell_writer, 0);
    shell_register(command_test, PSTR("test"));
}
