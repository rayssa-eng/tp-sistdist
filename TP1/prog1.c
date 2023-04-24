#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#define SIGHUP  1   /* Hangup the process */
#define SIGINT  2   /* Interrupt the process */
#define SIGQUIT 3   /* Quit the process */
#define SIGILL  4   /* Illegal instruction. */
#define SIGTRAP 5   /* Trace trap. */
#define SIGABRT 6   /* Abort. */

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <process_id> <signal_number>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    int signal_num = atoi(argv[2]);

    int result = kill(pid, signal_num);

    if (result == 0) {
        printf("Signal %d sent to process with ID %d\n", signal_num, pid);
    } else {
        printf("Failed to send signal %d to process with ID %d\n", signal_num, pid);
    }

    return 0;
}