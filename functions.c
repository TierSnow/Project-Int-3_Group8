//
// Created by USER on 22/10/2025.
//

#include "functions.h"
#include "utils.c"


// Create a new cell for a linked list
cell *create_cell(int arr, float proba) {
    // Allocate memory for a new cell
    cell *new_cell = malloc(sizeof(cell));
    if (new_cell == NULL) {
        fprintf(stderr, "Memory allocation error (Never sure how that happens honestly. Unfreed memory? Problem with the engine? Take your guess :p)\n");
        exit(EXIT_FAILURE);
    }
    // Assign the cell's data
    new_cell->arr = arr;      // destination vertex
    new_cell->proba = proba;  // edge probability
    new_cell->next = NULL;    // no next element yet
    return new_cell;
}


// Create an empty linked list (used for each vertex)
list create_list() {
    list l;
    l.head = NULL; // initialize with no elements
    return l;
}


// Add a new cell to the beginning of a linked list
void add_cell(list *l, int arr, float proba) {
    cell *new_cell = create_cell(arr, proba); // create the new cell
    new_cell->next = l->head;                 // link to current head
    l->head = new_cell;                       // update list head
}


// Display one linked list
void display_list(const list *l) {
    cell *current = l->head;
    while (current != NULL) { // traverse the list
        printf(" -> (%d, %.2f)", current->arr, current->proba);
        current = current->next;
    }
    printf("\n");
}

// Create a full adjacency list structure
a_list *create_a_list(int size) {
    // allocate memory for the adjacency list container
    a_list *graph = malloc(sizeof(a_list));
    if (graph == NULL) {
        fprintf(stderr, "Memory allocation error (Never sure how that happens honestly. Unfreed memory? Problem with the engine? Take your guess :p)\n");
        exit(EXIT_FAILURE);
    }
    // store the number of vertices
    graph->size = size;
    // allocate the array that will hold one 'list' per vertex
    graph->array = malloc(size * sizeof(list));
    if (graph->array == NULL) {
        // if allocation fails, free previously allocated memory and exit
        fprintf(stderr, "Memory allocation error (Never sure how that happens honestly. Unfreed memory? Problem with the engine? Take your guess :p)\n");
        free(graph);
        exit(EXIT_FAILURE);
    }
    // initialize each element of the array as an empty list
    for (int i = 0; i < size; i++) graph->array[i] = create_list(); // sets head = NULL
    // return the initialized adjacency-list structure
    return graph;
}

// Display the full adjacency list (graph)
void display_a_list(const a_list *graph) {
    for (int i = 0; i < graph->size; i++) {
        printf("Vertex %d:", i + 1);
        display_list(&graph->array[i]);
    }
}


// Read a graph from a text file into adjacency list form
a_list readGraph(const char *filename) {
    FILE *file = fopen(filename, "r");
    int nbvert, start, end;
    float proba;
    a_list *list;
    if (file == NULL) {
        perror("Could not open file for reading (kindly reminder not to try this function with an empty file /:)");
        exit(EXIT_FAILURE);
    }
    if (fscanf(file, "%d", &nbvert) != 1) {
        perror("Could not read number of vertices (Typo, typo, go away, verifying your file goes a long way)");
        exit(EXIT_FAILURE);
    }
    list = create_a_list(nbvert); // allocate graph
    while (fscanf(file, "%d %d %f", &start, &end, &proba) == 3) {
        // check vertex range
        if (start < 1 || start > nbvert || end < 1 || end > nbvert) {
            fprintf(stderr, "Warning: ignoring out-of-range edge %d -> %d\n", start, end);
            continue;
        }
        int s = start - 1; // convert to 0-based
        cell *new_cell = create_cell(end, proba);
        new_cell->next = list->array[s].head; // insert at head
        list->array[s].head = new_cell;
    }
    fclose(file);
    return *list;
}

// Check if the graph probabilities are valid (~1 per vertex)
int check_graph(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (file == NULL) {
        perror("Could not open file for reading (kindly reminder not to try this function with an empty file /:)");
        exit(EXIT_FAILURE);
    }
    int nbvert;
    if (fscanf(file, "%d", &nbvert) != 1) {
        perror("Could not read number of vertices (Typo, typo, go away, verifying your file goes a long way)");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    float *sums = calloc(nbvert + 1, sizeof(float));
    if (sums == NULL) {
        perror("Memory allocation error (Never sure how that happens honestly. Unfreed memory? Problem with the engine? Take your guess :p)\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    //Define variables for the next step
    int start, end;
    float proba;
    int valid = 1;
    // accumulate probabilities for each start vertex
    while (fscanf(file, "%d %d %f", &start, &end, &proba) == 3) {
        if (start < 1 || start > nbvert) {
            fprintf(stderr, "Invalid start vertex: %d (expected 1-%d)\n", start, nbvert);
            valid = 0;
            break;
        }
        sums[start] += proba;
    }
    // verify each vertex sums to ~1
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


// Export graph to a flowchart format for visualization
void export_graph(a_list *graph, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Could not open file for reading (kindly reminder not to try this function with an empty file /:)");
        exit(EXIT_FAILURE);
    }
    // write header
    fprintf(file, "---\nconfig:\n layout: elk\n theme: neo\n look: neo\n---\nflowchart LR\n");
    // write vertices
    for (int i = 0; i < graph->size; i++)
        fprintf(file, "%s((%d))\n", getID(i + 1), i + 1);
    fprintf(file, "\n");
    // write edges
    for (int i = 0; i < graph->size; i++) {
        cell *curr = graph->array[i].head;
        while (curr != NULL) {
            fprintf(file, "%s -->|%.2f|%s\n", getID(i + 1), curr->proba, getID(curr->arr));
            curr = curr->next;
        }
    }
    fclose(file);
}
