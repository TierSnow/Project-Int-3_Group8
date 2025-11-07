//
// Created by USER on 22/10/2025.
//

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct cell {
    int arr;
    float proba;
    struct cell *next;
} cell;

typedef struct {
    cell *head;
} list;

typedef struct {
    list *array;
    int size;
} a_list;


cell *create_cell(int, float);

list create_list();

void add_cell(list *, int, float);

void display_list(const list *l);

a_list *create_a_list(int);

void display_a_list(const a_list *);

a_list readGraph(const char *);

int check_graph(const char *);

void export_graph(a_list *, const char *);

#endif //FUNCTIONS_H