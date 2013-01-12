/**
 * @file sortserv.c
 * @author Martin Prebio (1025737) <martin.prebio@students.tuwien.ac.at>
 * @date 31.12.2012
 *
 * @brief The server which receives integers from shaed memory, sorts them and write them back to the client.
 *
 * Sortserv: Return sorted integers
 */

#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include "shared.h"

extern int shm_id;
extern int sem_id1;
extern int sem_id2;
extern int sem_id3;
extern int sem_id4;

/**
 * @brief Compare two long integers
 * @see strcmp
 * @param x The first integer
 * @param y The second integer
 * @return -1 if x<y, 0 if x=y, 1 if x>y
*/
static int longcmp(const void* x, const void* y) {
    const long* a = (long *)x;
    const long* b = (long *)y;
    if (*a == *b) {
        return 0;
    } else if (*a < *b) {
        return -1;
    } else {
        return 1;
    }
}

/**
 * Program entry point
 * @brief This program reads integers from shared memory, sorts them and write the sorted ones back.
 * @param argc The argument counter
 * @param argv The argument vector
 * @return EXIT_SUCCESS on success, EXIT_FAILURE in case of an error
 * @details global variables: program_name, shm_id and sem_id[1-4]
*/
int main(int argc, const char * argv[]) {
    program_name = argv[0];

    (void) init_signal_handling();

    // Initialize shm
    shm_id = shmget(SHM_KEY, sizeof(shared_struct), IPC_CREAT | 0600);
    if (shm_id < 0) {
        (void) bailout("shmget failed");
    }
    struct shared_struct * shared_mem = shmat(shm_id, NULL, 0);
    if (shared_mem == (struct shared_struct *) -1) {
        (void) bailout("shmat");
    }

    // Initialize sem
    // 1 and 2 are for reading
    // 3 and 4 are for writing
    sem_id1 = seminit(SEM_KEY, 0600, 0);
    if (sem_id1 < 0) {
        (void) bailout("seminit (1) failed");
    }
    sem_id2 = seminit(SEM_KEY+1, 0600, 1);
    if (sem_id2 < 0) {
        (void) bailout("seminit (2) failed");
    }
    sem_id3 = seminit(SEM_KEY+2, 0600, 0);
    if (sem_id3 < 0) {
        (void) bailout("seminit (3) failed");
    }
    sem_id4 = seminit(SEM_KEY+3, 0600, 1);
    if (sem_id4 < 0) {
        (void) bailout("seminit (4) failed");
    }

    /**
     * Data is read via the shared memory and saved in a resizing local memory block.
     * Resizing doubles the allocated memory if needed.
     */
    long buffer_size = 1;
    long buffer_used = 0;
    long * buffer = malloc(sizeof(long));

    // Read values from client
    do {
        if (P(sem_id1) < 0) {
            (void) bailout("P 1");
        }

        if (shared_mem->end) {
            DEBUG("GOT KILL\n");
        } else {
            if (buffer_used == buffer_size) {
                buffer_size <<= 1;
                DEBUG("BUFFER FULL - Resizing to %ld\n", buffer_size);
                if (realloc(buffer, buffer_size) < 0) {
                    (void) bailout("realloc");
                }
            }

            DEBUG("Got:\t%ld\t %ld\n", buffer_used, shared_mem->data);
            buffer[buffer_used++] = shared_mem->data;
        }

        if (V(sem_id2) < 0) {
            (void) bailout("V 2");
        }
    } while (shared_mem->end == 0);

    // Sort the input
    qsort(buffer, buffer_used, sizeof(long), longcmp);

    // Write everything back
    shared_mem->end = 0;
    for (int i = 0; i < buffer_used; i++) {
        if (P(sem_id4) < 0) {
            (void) bailout("P 4");
        }

        DEBUG("Writing %ld\n", buffer[i]);

        shared_mem->data = buffer[i];

        if (V(sem_id3) < 0) {
            (void) bailout("V 3");
        }
    }

    if (P(sem_id4) < 0) {
        (void) bailout("P 2");
    }

    // Quit the client
    shared_mem->end = 1;

    if (V(sem_id3) < 0) {
        (void) bailout("V 1");
    }

    // Cleanup Sems
    if (semrm(sem_id1) < 0) {
        (void) bailout("semrm 1");
    }
    if (semrm(sem_id2) < 0) {
        (void) bailout("semrm 2");
    }
    if (semrm(sem_id3) < 0) {
        (void) bailout("semrm 3");
    }
    if (semrm(sem_id4) < 0) {
        (void) bailout("semrm 4");
    }

    // Remove shm
    if (shmctl(shm_id, IPC_RMID, NULL) < 0) {
        (void) bailout("terminating shm (shmctl)");
    }

    exit(EXIT_SUCCESS);
}
