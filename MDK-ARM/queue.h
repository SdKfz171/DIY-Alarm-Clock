#define QUEUE_MAXSIZE 5

int queue[QUEUE_MAXSIZE];
int front, rear;

void init_queue(void);
void clear_queue(void);
int put(int k);
int get(void);
void print_queue(void);