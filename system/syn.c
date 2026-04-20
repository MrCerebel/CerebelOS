#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* SFO */
/* Syn - CerebelOS Privilege Escalation Tool */
/* Version: 0.0.1a */

void log_attempt(const char *user, int success) {
    FILE *f = fopen("/var/log/syn.log", "a");
    if (!f) return;
    if (success)
        fprintf(f, "SUCCESS: %s escalated to root\n", user);
    else
        fprintf(f, "FAILED: %s attempted to escalate to root\n", user);
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: syn <command> [args...]\n");
        return 1;
    }

    char *user = getenv("USER");
    if (!user) user = "unknown";

    /* Build command to run as root via su */
    char cmd[2048] = "su -c '";
    for (int i = 1; i < argc; i++) {
        strcat(cmd, argv[i]);
        if (i < argc - 1) strcat(cmd, " ");
    }
    strcat(cmd, "' root");

    int ret = system(cmd);

    if (ret == 0)
        log_attempt(user, 1);
    else
        log_attempt(user, 0);

    return ret;
}
