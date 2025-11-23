//
// Created by USER on 22/10/2025.
//

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Defines the type cell
typedef struct cell {
    int arr;
    float proba;
    struct cell *next;
} cell;

//Defines the type list
typedef struct {
    cell *head;
} list;

//Defines the type a_list that we'll mainly use in the project
typedef struct {
    list *array;
    int size;
} a_list;

typedef struct {
    int identifier;     // Node number in the graph
    int number;         // Temporary numbering for Tarjan
    int acc_number;     // Accessible number for grouping
    int indic;          // Boolean for stack membership (0 or 1)
} t_tarjan_vertex;

// Array for tracking state of ALL vertices in the graph
typedef struct {
    t_tarjan_vertex *vertices;  // Dynamic array
    int size;                   // Current number of vertices
    int capacity;               // Maximum capacity
} t_tarjan_array;

// Class to store grouped vertices
typedef struct {
    char *name;                 // Class name: "C1", "C2", etc.
    t_tarjan_vertex *vertices;  // Dynamic array of vertices in this class
    int size;                   // Current number of vertices
    int capacity;               // Maximum capacity
} t_class;

// Partition storing all classes
typedef struct {
    t_class *classes;           // Dynamic array of classes
    int size;                   // Current number of classes
    int capacity;               // Maximum capacity
} t_partition;

typedef struct {
    int *data;      // Array to store complete vertices
    int top;                    // Index of top element
    int capacity;               // Maximum capacity
} t_stack;

// Matrix structure definition
typedef struct {
    float **data;   // 2D array for matrix data
    int size;       // Matrix size (n x n)
} matrix;


cell *create_cell(int, float);

list create_list();

void add_cell(list *, int, float);

void display_list(const list *l);

a_list *create_a_list(int);

void display_a_list(const a_list *);

a_list readGraph(const char *);

int check_graph(const char *);

void export_graph(a_list *, const char *);

t_tarjan_array* init_tarjan_vertices(a_list *);

// Push a vertex identifier onto the stack
void push(t_stack *, int);

// Pop a vertex identifier from the stack
int pop(t_stack *);

t_stack* create_stack(int);

void parcours(int , a_list *, t_tarjan_array *, t_stack *, int *, t_partition *);

void tarjan(a_list *);

t_partition compute_partition(a_list *);

int** Hasse(a_list *, t_partition *);

void analyze_graph_characteristics(t_partition *, int **, int);

matrix* create_transition_matrix(a_list *);

matrix* create_zero_matrix(int);

void copy_matrix(matrix *, matrix *);

matrix* multiply_matrices(matrix *, matrix *);

float matrix_difference(matrix *, matrix *);

matrix* subMatrix(matrix *, t_partition *, int);
#endif //FUNCTIONS_H
