/**
 * @file shared.c
 * @author Martin Prebio (1025737) <martin.prebio@students.tuwien.ac.at>
 * @date 31.12.2012
 *
 * @brief Shared stuff for client and server
 *
 * Shared: Shared stuff for client and server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "shared.h"

int shm_id = -1;
int sem_id1 = -1;
int sem_id2 = -1;
int sem_id3 = -1;
int sem_id4 = -1;

/**
 * Shutdown function
 * @brief Force clean all shared resources
 * @param signal Caught signal or <0 if called directly
 * @return Exit if called via signal handling, void otherwise
 * @details global variables: shm_id and sem_id[1-4]
*/
void shutdown(int signal) {
    (void) shmctl(shm_id, IPC_RMID, NULL);

    (void) semrm(sem_id1);
    (void) semrm(sem_id2);
    (void) semrm(sem_id3);
    (void) semrm(sem_id4);

    if (signal >= 0) {
        exit(EXIT_FAILURE);
    }
}

/**
 * Error function
 * @brief Stopping program after an error in a non chaotic way with freeing resources and displaying occoured  error.
 * @param argc The argument counter
 * @param argv The argument vector
 * @return EXIT_SUCCESS on success, EXIT_FAILURE in case of an error
 * @details global variables: program_name, shm_id and sem_id[1-4]
*/
void bailout(char* message) {
    int error = errno;

    (void) shutdown(-1);

    (void) fprintf(stderr, "%s: %s: %s\n", program_name, message, strerror(error));
    exit(EXIT_FAILURE);
}

/**
 * Signal handling intialization
 * @brief Register the shutdown function for SIGINT and SIGQUIT
*/
void init_signal_handling(void) {
    // Init signal handling
    struct sigaction action;
    action.sa_handler = shutdown;
    if (sigaction(SIGQUIT, &action, NULL) < 0) {
        (void) bailout("sigaction of SIGQUIT");
    }
    if (sigaction(SIGINT, &action, NULL) < 0) {
        (void) bailout("sigaction of SIGQUIT");
    }
}
