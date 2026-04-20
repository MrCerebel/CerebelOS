#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

/* SFO */
/* Spark Launcher - Forces root then executes Spark */

int main() {
    setreuid(0, 0);
    setregid(0, 0);
    execl("/sbin/spark-real", "spark-real", NULL);
    return 1;
}
