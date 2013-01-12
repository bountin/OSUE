/**
 * @file sortclient.c
 * @author Martin Prebio (1025737) <martin.prebio@students.tuwien.ac.at>
 * @date 31.12.2012
 *
 * @brief The client which reads integers from stdin, sends them to a server and writes the returned numbers to stdout.
 * The server's output is the sorted input
 *
 * Sortclient: Reads integers until ^D and writes them sorted to stdout.
 */

#include <stdio.h>
#include <stdlib.h>
#include "shared.h"

extern int shm_id;
extern int sem_id1;
extern int sem_id2;
extern int sem_id3;
extern int sem_id4;


/**
 * Program entry point
 * @brief This program reads integers from stout, sends them to a server and writes the returned numbers to stdout. The server's output is the sorted input.
 * @param argc The argument counter
 * @param argv The argument vector
 * @return EXIT_SUCCESS on success, EXIT_FAILURE in case of an error
 * @details global variables: program_name, shm_id and sem_id[1-4]
*/
int main(int argc, const char * argv[]) {
    program_name = argv[0];

    (void) init_signal_handling();
    
    // Initialize shm
    shm_id = shmget(SHM_KEY, sizeof(shared_struct), 0600);
    if (shm_id < 0) {
        (void) fprintf(stderr, "Could not access shared memory for communication with server.\nMaybe you should start it?\n\n");
        (void) bailout("shmget failed");
    }
    struct shared_struct * shared_mem = shmat(shm_id, NULL, 0);
    if (shared_mem == (struct shared_struct *)-1) {
        (void) bailout("shmat");
    }

    // Initialize sems
    // 1 and 2 are for writing
    // 3 and 4 are for reading
    sem_id1 = semgrab(SEM_KEY);
    if (sem_id1 < 0) {
        (void) bailout("semgrab (1) failed");
    }
    sem_id2 = semgrab(SEM_KEY+1);
    if (sem_id2 < 0) {
        (void) bailout("semgrab (2) failed");
    }
    sem_id3 = semgrab(SEM_KEY+2);
    if (sem_id3 < 0) {
        (void) bailout("semgrab (3) failed");
    }
    sem_id4 = semgrab(SEM_KEY+3);
    if (sem_id4 < 0) {
        (void) bailout("semgrab (4) failed");
    }

    long input = -1;
    char next_char;

    // Read input until EOF and send it to the server via shm
    while ((next_char = fgetc(stdin)) != EOF) {
        // XXX: Double ^D problem

        switch (next_char) {
            case '1': case '2': case '3':
            case '4': case '5': case '6':
            case '7': case '8': case '9':
            case '0':
                if (input < 0) {
                    input = 0;
                }
                input *= 10;
                input += (next_char - '0');
                DEBUG("read: Got %ld\n", input);
                break;
            case ' ':
            case '\n':
                // Skip processing if no number is read since last shm writing
                if (input < 0) {
                    break;
                }

                // Write the read number
                if (P(sem_id2) < 0) {
                    (void) bailout("P 2");
                }
                shared_mem->data = input;
                DEBUG("WROTE: %ld\n", input);
                // Reset input buffer
                input = -1;
                if (V(sem_id1) < 0) {
                    (void) bailout("V 1");
                }
                break;
            default:
                DEBUG("Invalid character: %c (%i)\n", next_char, next_char);
                bailout("You have written an unsupported character");
        }
    }

    if (ferror(stdin)) {
        (void) bailout("fgetc");
    }

    if (P(sem_id2) < 0) {
        (void) bailout("P 2");
    }

    shared_mem->end = 1;
    DEBUG("WROTE: KILL\n");

    if (V(sem_id1) < 0) {
        (void) bailout("V 1");
    }

    (void)fprintf(stdout, "\nThe sorted answer:\n");

    // Read the answer from the server
    do {
        if (P(sem_id3) < 0) {
            (void) bailout("P 3");
        }

        if (shared_mem->end) {
            DEBUG("GOT KILL\n");
            break;
        } else {
            DEBUG("Got: %ld\n", shared_mem->data);
            fprintf(stdout, "%ld\n", shared_mem->data);
        }

        if (V(sem_id4) < 0) {
            (void) bailout("V 4");
        }
    } while (shared_mem->end == 0);

    // No cleanup necessary

    exit(EXIT_SUCCESS);
}
