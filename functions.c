//
// Created by USER on 22/10/2025.
//

#include "functions.h"
#include "utils.c"

cell *create_cell(int arr, float proba) {
    cell *new_cell = malloc(sizeof(cell));
    if (new_cell == NULL) {
        fprintf(stderr, "Memory allocation error (create_cell)\n");
        exit(EXIT_FAILURE);
    }
    new_cell->arr = arr;
    new_cell->proba = proba;
    new_cell->next = NULL;
    return new_cell;
}

list create_list() {
    list l;
    l.head = NULL;
    return l;
}

void add_cell(list *l, int arr, float proba) {
    cell *new_cell = create_cell(arr, proba);
    new_cell->next = l->head;
    l->head = new_cell;
}

void display_list(const list *l) {
    cell *current = l->head;
    while (current != NULL) {
        printf(" -> (%d, %.2f)", current->arr, current->proba);
        current = current->next;
    }
    printf("\n");
}

a_list *create_a_list(int size) {
    a_list *graph = malloc(sizeof(a_list));
    if (graph == NULL) {
        fprintf(stderr, "Memory allocation error (create_a_list)\n");
        exit(EXIT_FAILURE);
    }

    graph->size = size;
    graph->array = malloc(size * sizeof(list));
    if (graph->array == NULL) {
        fprintf(stderr, "Memory allocation error (create_a_list array)\n");
        free(graph);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        graph->array[i] = create_list();
    }

    return graph;
}

void display_a_list(const a_list *graph) {
    for (int i = 0; i < graph->size; i++) {
        printf("Vertex %d:", i+1);
        display_list(&graph->array[i]);
    }
}

a_list readGraph(const char *filename) {
    FILE *file = fopen(filename, "r");
    int nbvert, start, end;
    float proba;
    a_list *list;

    if (file == NULL) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }

    if (fscanf(file, "%d", &nbvert) != 1) {
        perror("Could not read number of vertices");
        exit(EXIT_FAILURE);
    }
    list = create_a_list(nbvert);
    while (fscanf(file, "%d %d %f", &start, &end, &proba) == 3) {
        if (start < 1 || start > nbvert || end < 1 || end > nbvert) {
            fprintf(stderr, "Warning: ignoring out-of-range edge %d -> %d\n", start, end);
            continue;
        }
        int s = start - 1;
        cell *new_cell = create_cell(end, proba);
        new_cell->next = list->array[s].head;
        list->array[s].head = new_cell;
    }

    fclose(file);
    return *list;
}

int check_graph(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (file == NULL) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }

    int nbvert;
    if (fscanf(file, "%d", &nbvert) != 1) {
        perror("Could not read number of vertices");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    float *sums = calloc(nbvert + 1, sizeof(float));
    if (sums == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    int start, end;
    float proba;
    int valid = 1;

    while (fscanf(file, "%d %d %f", &start, &end, &proba) == 3) {
        if (start < 1 || start > nbvert) {
            fprintf(stderr, "Invalid start vertex: %d (expected 1-%d)\n", start, nbvert);
            valid = 0;
            break;
        }
        sums[start] += proba;
    }
    if (valid) {
        for (int i = 1; i <= nbvert; i++) {
            if (sums[i] > 0 && (sums[i] < 0.99 || sums[i] > 1.01)) {
                printf("Vertex %d probabilities sum to %f (should be ~1.0)\n", i, sums[i]);
                valid = 0;
                break;
            }
        }
    }

    free(sums);
    fclose(file);
    return valid;
}


void export_graph(a_list *graph, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Could not open file for writing");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "---\n");
    fprintf(f, "config:\n");
    fprintf(f, " layout: elk\n");
    fprintf(f, " theme: neo\n");
    fprintf(f, " look: neo\n");
    fprintf(f, "---\n");
    fprintf(f, "flowchart LR\n");
    for (int i = 0; i < graph->size; i++) {
        fprintf(f, "%s((%d))\n", getID(i + 1), i + 1);
    }
    fprintf(f, "\n");
    for (int i = 0; i < graph->size; i++) {
        cell *curr = graph->array[i].head;
        while (curr != NULL) {
            fprintf(f, "%s -->|%.2f|%s\n",
                    getID(i + 1), curr->proba, getID(curr->arr));
            curr = curr->next;
        }
    }
    fclose(f);
}

