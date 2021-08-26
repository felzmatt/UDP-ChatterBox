#include <stdio.h>
#include <stdlib.h>

#include "list.h"


int init_list_node ( list_node_t * node , list_node_ops * ops)
{
    if (node == NULL || ops == NULL ) {
        return 1;
    }
    node -> prev = NULL;
    node -> next = NULL;
    node -> ops = ops;
    node -> idx = 0;

    return 0;
} // inizializza il nodo

int append_list_node( list_t * list, list_node_t * node) 
{ 
    // aggancia node alla list
    if ( list == NULL || node == NULL ) {
        return 1;
    }
    
    if (list -> items == 0) 
    {
        list -> head = node;
        node -> prev = NULL;
    
    } else {
        
        list -> tail -> next = node;
        node -> prev = list -> tail;
    }
    
    list -> tail = node;
    node -> next = NULL;

    node -> idx = list -> items++;

    return 0;
}

int init_list( list_t * list ) 
{
    // inizializza una lista vuota
    if ( list == NULL ) {
        return 1;
    }
    
    list -> head = NULL;
    list -> tail = NULL;
    list -> items = 0;

    return 0;

}

list_node_t * get_node_by_idx( list_t * list, int index ) 
{
    if (index >= list -> items) {
        printf("Errore, fuori dalla lista\n");
        return NULL;
    }
    
    int i = 0;
    int items = list -> items;
    
    list_node_t * node = list -> head;
    
    while ( i < items ) {
        if ( i == index ) break;
        node = node -> next;
        i++;
    }
    return node;

} // scorri la lista fino all'indice in questione e ritorna il puntatore a quel nodo

void print_list_node( list_node_t * node ) 
{
    if ( node == NULL || node -> ops == NULL || node -> ops -> print_fn == NULL ) {
        handle_error("Errore nella standa dell'oggetto list_node_t");
    }
    (*node -> ops -> print_fn)( node );

} // stampa il contenuto del nodo

void destroy_list_node( list_node_t * node )
{
    if ( node == NULL || node -> ops == NULL || node -> ops -> destroy_fn == NULL ) {
        handle_error("Errore nella distruzione dell'oggetto list_node_t");
    }
    (*node -> ops -> destroy_fn )( node );
}
void print_list ( list_t * list ) 
{
    if ( list == NULL ) {
        handle_error("Errore nella stampa della lista");
    }
    printf("\n\n");
    int i;
    int n = list -> items;
    list_node_t * node = list -> head;
    for ( i = 0; i < n; i++ )
    {
        print_list_node( node );
        node = node -> next;
    } 
    printf("\n\n");
} // scorre la lista e stampa tutti i nodi

int detach_list_node( list_t * list, list_node_t * node, int reorder )
{
    int ret;
    if ( list == NULL || node == NULL )
    {
        return -1;
    }
    if ( list -> head == node) {
        // rimuovere la testa della lista
        list_node_t * new_head= node -> next;
        list -> head = new_head;
        new_head -> prev = NULL;
    
    } else if ( list -> tail == node ) {
        // rimuovere nodo in coda
        list_node_t * new_tail = node -> prev;
        list -> tail = new_tail;
        new_tail -> next = NULL;
    } else {
        // rimuovere nodo al centro
        list_node_t * prev_node = node -> prev;
        list_node_t * next_node = node -> next;
        prev_node -> next = next_node;
        next_node -> prev = prev_node;
    }

    node -> prev = NULL;
    node -> next = NULL;
    ret = node -> idx;
    list -> items--;

    // sistemare indici
    if ( reorder != 0 ) {
        int i = 0;
        int items = list -> items;

        list_node_t * node = list -> head;

        while ( i < items ) {
            node -> idx = i++;
            node = node -> next;
        }
    }

    return ret;

} // sgancia il nodo dalla lista
