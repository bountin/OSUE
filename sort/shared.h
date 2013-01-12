#include <sys/shm.h>
#include <errno.h>
#include <sem182.h>

#define SHM_KEY 311290
#define SEM_KEY  30192

#ifdef ENDEBUG
#define DEBUG(...) do { fprintf(stderr, __VA_ARGS__); } while(0)
#else
#define DEBUG(...)
#endif

const char * program_name;

void shutdown(int signal);
void bailout(char *);
void init_signal_handling(void);

struct shared_struct {
    long data;
    int end;
} shared_struct;
