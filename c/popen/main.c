#include <stdlib.h>
#include <stdio.h>


int main()
{
    FILE* cmd_output = popen("ls -l", "r");

    if (!cmd_output)
    {
        return -1;
    }

    char buffer[1024];

    while(fgets(buffer, sizeof(buffer), cmd_output) != NULL)
    {
        printf("=> %s", buffer);
        buffer[0] = '\0';
    }

    pclose(cmd_output);

    return 0;
}