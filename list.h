#ifndef LIST_H
#define LIST_H

// macros for handling errors
#define handle_error_en(en, msg)    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg)           do { perror(msg); exit(EXIT_FAILURE); } while (0)

/* Configuration parameters */
#define DEBUG           1   // display debug messages
#define MAX_CONN_QUEUE  3   // max number of connections the server can queue
#define SERVER_ADDRESS  "127.0.0.1"
#define SERVER_COMMAND  "QUIT\n"
#define SERVER_PORT     2015

struct list_node;

typedef int (*list_node_print) (struct list_node*);
typedef int (*list_node_destroy) (struct list_node*);

typedef struct ops {
    list_node_print print_fn;
    list_node_destroy destroy_fn;
} list_node_ops;

typedef struct list_node {
    struct list_node * prev;
    struct list_node * next;
    list_node_ops * ops;
    int idx;
} list_node_t;


typedef struct list {
    struct list_node* head;
    struct list_node* tail;
    int items;
} list_t;


/**
 * Metodi della Lista
*/
 
int init_list( list_t * list ); // inizializza una lista vuota
void print_list( list_t * list ); // scorre la lista e stampa tutti i nodi

/**
 * Metodi del Nodo
*/
int init_list_node ( list_node_t * node, list_node_ops * ops ); // inizializza il nodo
int append_list_node( list_t * list, list_node_t* node); // aggancia un nodo alla lista
int detach_list_node( list_t * list, list_node_t * node, int reorder ); // sgancia il nodo dalla lista
list_node_t* get_node_by_idx( list_t * list , int index ); // scorri la lista fino all'indice in questione e ritorna il puntatore a quel nodo
void print_list_node( list_node_t * node ); // stampa il contenuto del 
void destroy_list_node( list_node_t * node );









#endif