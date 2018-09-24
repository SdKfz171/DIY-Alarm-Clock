#include "queue.h"

char queue[QUEUE_MAXSIZE];
char buf[4] = { 0 };
int front, rear;

void init_queue(void){
    front = rear = 0;
	put('0');
	put('0');
	put('0');
	put('0');
}

void clear_queue(void){
    front = rear;
}

int put(int k){
    // ?? ????? ??
    if ((rear + 1) % QUEUE_MAXSIZE == front){
        //printf("\n   Queue overflow.");
        return -1;
    }

    queue[rear] = k;
    rear = ++rear % QUEUE_MAXSIZE;
    return k;
}

int get(void){
    int i;
    if (front == rear){
        //printf("\n  Queue underflow.");
        return -1;
    }

    i = queue[front];
    front = ++front % QUEUE_MAXSIZE;
    return i;
}

void print_queue(void){
    int i;
    printf("\n Queue contents : Front ----------> Rear \n");
    for (i = front; i != rear; i = ++i%QUEUE_MAXSIZE)
        printf("%-6c", queue[i]);
	printf("\r\n");
}

char * get_queue(void){
	int i, j;
	
	for (i = front, j = 0; i != rear, j < 4; i = ++i%QUEUE_MAXSIZE, j++)
        buf[j] = queue[i]; 
	return (char *)buf;
}

//int main(void){

//    int i;
//    init_queue();

//    printf("\nPut 0, 0, 0, 0");
//    put(0);
//    put(0);
//    put(0);
//    put(0);
//    print_queue();
//    
//    while(1){
//        int a;
//        //scanf("%d", &a);

//        i = get();
//        put(a);
//        printf("%d OUT %d IN\r\n", i, a);

//        print_queue();
//    }

//    return 0;
//}