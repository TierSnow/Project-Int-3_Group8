//
// Created by USER on 22/10/2025.
//

#include "functions.h"
#include "utils.c"
#include "hasse.h"


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
    // declare the variable for the adjacency list
    a_list *list;
    if (file == NULL) {
        perror("Could not open file for reading (kindly reminder not to try this function with an empty file /:)");
        exit(EXIT_FAILURE);
    }
    // first line contains number of vertices
    if (fscanf(file, "%d", &nbvert) != 1) {
        perror("Could not read number of vertices (Typo, typo, go away, verifying your file goes a long way)");
        exit(EXIT_FAILURE);
    }
    list = create_a_list(nbvert); // Initialise an empty adjacency list using the number of vertices

    while (fscanf(file, "%d %d %f", &start, &end, &proba) == 3) {
        // check vertex range
        // we obtain, for each line of the file, the values
        // start, end and proba
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



// Function to initialize tarjan vertices from adjacency list
t_tarjan_array* init_tarjan_vertices(a_list *graph) {
    if (graph == NULL || graph->array == NULL) {
        return NULL;
    }
    // Create the tarjan array structure
    t_tarjan_array *tar = malloc(sizeof(t_tarjan_array));
    if (tar == NULL) {
        return NULL;
    }
    // Allocate memory for vertices
    tar->size = graph->size;
    tar->capacity = graph->size;
    tar->vertices = malloc(graph->size * sizeof(t_tarjan_vertex));
    if (tar->vertices == NULL) {
        free(tar);
        return NULL;
    }
    // Initialize each vertex
    for (int i = 0; i < graph->size; i++) {
        tar->vertices[i].identifier = i; // Using index as identifier
        tar->vertices[i].number = -1;
        tar->vertices[i].acc_number = -1;
        tar->vertices[i].indic = 0; // Not in stack
    }
    return tar;
}


// Push a vertex identifier onto the stack
void push(t_stack *stack, int vertex_id) {
    if (stack->top < stack->capacity - 1) {
        stack->data[++stack->top] = vertex_id;
    }
    // Optional: add error handling for stack overflow
}

// Pop a vertex identifier from the stack
int pop(t_stack *stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top--];
    }
    return -1; // Error value indicating empty stack
}

t_stack* create_stack(int capacity) {
    t_stack *stack = malloc(sizeof(t_stack));
    if (stack == NULL) return NULL;
    stack->capacity = capacity;
    stack->top = -1;
    stack->data = malloc(capacity * sizeof(int));
    if (stack->data == NULL) {
        free(stack);
        return NULL;
    }
    return stack;
}

void parcours(int v, a_list *graph, t_tarjan_array *tarjan_array,
              t_stack *stack, int *num, t_partition *partition) {
    // Initialize current vertex (v is already 0-based)
    t_tarjan_vertex *current = &tarjan_array->vertices[v];
    current->number = *num;
    current->acc_number = *num;
    (*num)++;
    push(stack, v);
    current->indic = 1;
    // Process all successors
    cell *c = graph->array[v].head;
    while (c != NULL) {
        int w = c->arr - 1;  // Convert c from 1-based to 0-based
        t_tarjan_vertex *successor = &tarjan_array->vertices[w];
        if (successor->number == -1) {
            // Successor not visited
            parcours(w, graph, tarjan_array, stack, num, partition);
            current->acc_number = (current->acc_number < successor->acc_number) ?
                                 current->acc_number : successor->acc_number;
        } else if (successor->indic == 1) {
            // Successor is in stack
            current->acc_number = (current->acc_number < successor->number) ?
                                 current->acc_number : successor->number;
        }
        c = c->next;
    }
    // Check if current vertex is a root
    if (current->acc_number == current->number) {
        // Create new class for strongly connected component
        t_class new_class;
        new_class.name = malloc(10 * sizeof(char));
        sprintf(new_class.name, "C%d", partition->size + 1);
        new_class.vertices = malloc(tarjan_array->size * sizeof(t_tarjan_vertex));
        new_class.size = 0;
        new_class.capacity = tarjan_array->size;
        // Pop vertices until we reach current vertex
        int w_vertex;
        while (w_vertex != v) {
            w_vertex = pop(stack);
            t_tarjan_vertex *w = &tarjan_array->vertices[w_vertex];
            w->indic = 0;
            new_class.vertices[new_class.size++] = *w;
        }
        // Add class to partition
        if (partition->size >= partition->capacity) {
            // Resize partition if needed (basic implementation)
            partition->capacity *= 2;
            partition->classes = realloc(partition->classes,
                                       partition->capacity * sizeof(t_class));
        }
        partition->classes[partition->size++] = new_class;
    }
}


void tarjan(a_list *graph) {
    // Initialize partition
    t_partition partition;
    partition.classes = malloc(graph->size * sizeof(t_class));
    partition.size = 0;
    partition.capacity = graph->size;
    // Initialize tarjan vertices array
    t_tarjan_array *tarjan_array = init_tarjan_vertices(graph);
    // Initialize stack
    t_stack *stack = create_stack(graph->size);
    // Initialize num counter
    int num = 0;
    // Perform DFS for all unvisited vertices
    for (int i = 0; i < graph->size; i++) {
        if (tarjan_array->vertices[i].number == -1) {
            parcours(i, graph, tarjan_array, stack, &num, &partition);
        }
    }
    // Output the components in the required format
    for (int i = 0; i < partition.size; i++) {
        printf("Component %s: {", partition.classes[i].name);
        for (int j = 0; j < partition.classes[i].size; j++) {
            printf("%d", partition.classes[i].vertices[j].identifier + 1);  // Convert to 1-based for display
            if (j < partition.classes[i].size - 1) {
                printf(",");
            }
        }
        printf("}\n");
    }
    // Free allocated memory
    for (int i = 0; i < partition.size; i++) {
        free(partition.classes[i].name);
        free(partition.classes[i].vertices);
    }
    free(partition.classes);
}

int** Hasse(a_list *graph, t_partition *partition) {
    int num_classes = partition->size;
    // Create vertex-to-class mapping
    // This array maps each vertex ID to its corresponding class index in the partition
    int *vertex_to_class = malloc(graph->size * sizeof(int));
    for (int c = 0; c < partition->size; c++) {
        for (int v = 0; v < partition->classes[c].size; v++) {
            int vertex_id = partition->classes[c].vertices[v].identifier;
            vertex_to_class[vertex_id] = c;  // Store class index for this vertex
        }
    }
    // Create class links matrix (adjacency matrix for classes)
    // This is a num_classes x num_classes matrix where:
    // class_links[i][j] = 1 if there's an edge from class i to class j, 0 otherwise
    int **class_links = malloc(num_classes * sizeof(int *));
    for (int i = 0; i < num_classes; i++) {
        class_links[i] = calloc(num_classes, sizeof(int));  // Initialize all to 0
    }
    printf("Hasse Diagram:\n");
    // Process all edges in the original graph to build class relationships
    for (int i = 0; i < graph->size; i++) {
        int Ci = vertex_to_class[i];  // Get class of source vertex
        cell *cell = graph->array[i].head;
        while (cell != NULL) {
            int j = cell->arr - 1; // Convert to 0-based (file uses 1-based indexing)
            int Cj = vertex_to_class[j];  // Get class of destination vertex
            // Only record edges between DIFFERENT classes that haven't been recorded yet
            if (Ci != Cj && class_links[Ci][Cj] == 0) {
                class_links[Ci][Cj] = 1;  // Mark that there's an edge from class Ci to Cj
                printf("Class %s -> Class %s\n", partition->classes[Ci].name, partition->classes[Cj].name);
            }
            cell = cell->next;
        }
    }
    free(vertex_to_class);  // Free the temporary mapping array
    // Return the class adjacency matrix
    return class_links;
}

t_partition compute_partition(a_list *graph) {
    // Initialize partition
    t_partition partition;
    partition.classes = malloc(graph->size * sizeof(t_class));
    partition.size = 0;
    partition.capacity = graph->size;
    // Initialize tarjan vertices array
    t_tarjan_array *tarjan_array = init_tarjan_vertices(graph);
    // Initialize stack
    t_stack *stack = create_stack(graph->size);
    // Initialize num counter
    int num = 0;
    // Perform DFS for all unvisited vertices
    for (int i = 0; i < graph->size; i++) {
        if (tarjan_array->vertices[i].number == -1) {
            parcours(i, graph, tarjan_array, stack, &num, &partition);
        }
    }
    return partition;
}

void analyze_graph_characteristics(t_partition *partition, int **class_links, int num_classes) {
    int *has_outgoing = calloc(num_classes, sizeof(int));
    int absorbing_states = 0;
    int irreducible = (num_classes == 1) ? 1 : 0;
    printf("\nGraph Characteristics:\n");
    // Check for outgoing edges from each class
    for (int i = 0; i < num_classes; i++) {
        for (int j = 0; j < num_classes; j++) {
            if (class_links[i][j] == 1 && i != j) {
                has_outgoing[i] = 1;
                break;
            }
        }
    }
    // Display class characteristics
    for (int i = 0; i < num_classes; i++) {
        if (has_outgoing[i]) {
            printf("Class %s is transient - states {", partition->classes[i].name);
            for (int j = 0; j < partition->classes[i].size; j++) {
                printf("%d", partition->classes[i].vertices[j].identifier + 1);
                if (j < partition->classes[i].size - 1) printf(",");
            }
            printf("} are transient\n");
        } else {
            printf("Class %s is persistent - states {", partition->classes[i].name);
            for (int j = 0; j < partition->classes[i].size; j++) {
                printf("%d", partition->classes[i].vertices[j].identifier + 1);
                if (j < partition->classes[i].size - 1) printf(",");
            }
            printf("} are persistent\n");
            // Check for absorbing states (persistent class with only one state)
            if (partition->classes[i].size == 1) {
                printf("  State %d is ABSORBING\n", partition->classes[i].vertices[0].identifier + 1);
                absorbing_states++;
            }
        }
    }
    // Display global characteristics
    printf("\nGlobal characteristics:\n");
    if (irreducible) {
        printf("- The Markov graph is IRREDUCIBLE (only one class)\n");
    } else {
        printf("- The Markov graph is NOT IRREDUCIBLE (%d classes)\n", num_classes);
    }
    if (absorbing_states > 0) {
        printf("- There are %d absorbing state(s)\n", absorbing_states);
    } else {
        printf("- There are NO absorbing states\n");
    }
    free(has_outgoing);
}

// Function that creates an n x n matrix from adjacency list with transition probabilities
matrix* create_transition_matrix(a_list *graph) {
    if (graph == NULL || graph->array == NULL) {
        fprintf(stderr, "Error: Invalid graph input (sorry ;()\n");
        return NULL;
    }
    // Allocate memory for matrix structure
    matrix *mat = malloc(sizeof(matrix));
    if (mat == NULL) {
        fprintf(stderr, "Memory allocation error for matrix structure (sorry ;()\n");
        return NULL;
    }
    mat->size = graph->size;
    // Allocate memory for matrix data (2D array)
    mat->data = malloc(graph->size * sizeof(float *));
    if (mat->data == NULL) {
        fprintf(stderr, "Memory allocation error for matrix rows (sorry ;()\n");
        free(mat);
        return NULL;
    }
    // Initialize all probabilities to 0 first
    for (int i = 0; i < graph->size; i++) {
        mat->data[i] = calloc(graph->size, sizeof(float));
        if (mat->data[i] == NULL) {
            fprintf(stderr, "Memory allocation error for matrix columns (sorry ;()\n");
            // Free previously allocated memory before returning
            for (int j = 0; j < i; j++) {
                free(mat->data[j]);
            }
            free(mat->data);
            free(mat);
            return NULL;
        }
    }
    // Fill matrix with transition probabilities from adjacency list
    for (int i = 0; i < graph->size; i++) {
        cell *current = graph->array[i].head;
        while (current != NULL) {
            int j = current->arr - 1; // Convert to 0-based indexing
            if (j >= 0 && j < graph->size) {
                mat->data[i][j] += current->proba;
            }
            current = current->next;
        }
    }
    return mat;
}

// Function that creates an n x n matrix filled with zeros
matrix* create_zero_matrix(int size) {
    if (size <= 0) {
        fprintf(stderr, "Error: Matrix size must be positive (maybe? I think? Honestly there might as well be negative complexe matrixes next semester, but I don't think anybody wants to verify that)\n");
        return NULL;
    }
    //initialize the matrix
    matrix *mat = malloc(sizeof(matrix));
    if (mat == NULL) {
        fprintf(stderr, "Memory allocation error for matrix structure  (sorry ;()\n");
        return NULL;
    }
    mat->size = size;
    mat->data = malloc(size * sizeof(float *));
    if (mat->data == NULL) {
        fprintf(stderr, "Memory allocation error for matrix rows  (sorry ;()\n");
        free(mat);
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        mat->data[i] = calloc(size, sizeof(float));  // Allocate and zero-initialize each row
        if (mat->data[i] == NULL) {
            fprintf(stderr, "Memory allocation error for matrix columns  (sorry ;()\n");
            // Free previously allocated memory
            for (int j = 0; j < i; j++) {
                free(mat->data[j]);  // Clean up successfully allocated rows
            }
            free(mat->data);
            free(mat);
            return NULL;
        }
    }
    return mat;  // Return initialized zero matrix
}

// Function that copies values from one matrix to another of the same size
void copy_matrix(matrix *dest, matrix *src) {
    if (dest == NULL || src == NULL) {
        fprintf(stderr, "Error: Cannot copy NULL matrices\n");
        return;
    }
    if (dest->size != src->size) {  // Verify matrices have same dimensions
        fprintf(stderr, "Error: Matrices must be of the same size for copying\n");
        return;
    }
    for (int i = 0; i < src->size; i++) {
        for (int j = 0; j < src->size; j++) {
            dest->data[i][j] = src->data[i][j];  // Copy each element individually
        }
    }
}

// Function for multiplying two n x n matrices
matrix* multiply_matrices(matrix *a, matrix *b) {
    if (a == NULL || b == NULL) {
        fprintf(stderr, "Error: Cannot multiply NULL matrices\n");
        return NULL;
    }
    if (a->size != b->size) {  // Check compatibility for multiplication
        fprintf(stderr, "Error: Matrices must be of the same size for multiplication\n");
        return NULL;
    }
    int size = a->size;
    matrix *result = create_zero_matrix(size);  // Initialize result matrix
    if (result == NULL) {
        return NULL;
    }
    // Perform matrix multiplication
    for (int i = 0; i < size; i++) {  // Iterate through rows of first matrix
        for (int j = 0; j < size; j++) {  // Iterate through columns of second matrix
            result->data[i][j] = 0.0;  // Initialize result cell
            for (int k = 0; k < size; k++) {  // Dot product calculation
                result->data[i][j] += a->data[i][k] * b->data[k][j];
            }
        }
    }
    return result;  // Return product matrix
}

// Function that calculates the difference between two matrices: diff(M,N) = sum(sum(|m_ij - n_ij|))
float matrix_difference(matrix *m, matrix *n) {
    if (m == NULL || n == NULL) {
        fprintf(stderr, "Error: Cannot compute difference for NULL matrices\n");
        return -1.0;
    }
    if (m->size != n->size) {  // Ensure matrices are comparable
        fprintf(stderr, "Error: Matrices must be of the same size for difference calculation\n");
        return -1.0;
    }
    float diff = 0.0;
    for (int i = 0; i < m->size; i++) {
        for (int j = 0; j < m->size; j++) {
            float difference = m->data[i][j] - n->data[i][j];  // Calculate element difference
            // Manual absolute value calculation without fabsf
            if (difference < 0) {
                diff -= difference;  // Equivalent to diff += -difference (positive value)
            } else {
                diff += difference;  // Add positive difference directly
            }
        }
    }
    return diff;  // Return total absolute difference
}

// Function to extract a submatrix for a specific component
matrix* subMatrix(matrix *original_mat, t_partition *part, int compo_index) {
    // Check if component index is valid
    if (compo_index < 0 || compo_index >= part->size) {
        fprintf(stderr, "Error: Invalid component index %d (valid range: 0 to %d)\n",
                compo_index, part->size - 1);
        return NULL;
    }
    if (original_mat == NULL || original_mat->data == NULL) {  // Validate input matrix
        fprintf(stderr, "Error: Invalid input matrix\n");
        return NULL;
    }
    t_class *component = &part->classes[compo_index];  // Get target component
    int sub_size = component->size;  // Determine submatrix size
    // Allocate memory for submatrix structure
    matrix *submat = malloc(sizeof(matrix));
    if (submat == NULL) {
        fprintf(stderr, "Memory allocation error for submatrix structure\n");
        return NULL;
    }
    submat->size = sub_size;
    submat->data = malloc(sub_size * sizeof(float *));  // Allocate row pointers
    if (submat->data == NULL) {
        fprintf(stderr, "Memory allocation error for submatrix rows\n");
        free(submat);
        return NULL;
    }
    // Allocate and initialize submatrix data
    for (int i = 0; i < sub_size; i++) {
        submat->data[i] = malloc(sub_size * sizeof(float));  // Allocate each row
        if (submat->data[i] == NULL) {
            fprintf(stderr, "Memory allocation error for submatrix columns\n");
            // Free previously allocated memory
            for (int j = 0; j < i; j++) {
                free(submat->data[j]);  // Clean up successful allocations
            }
            free(submat->data);
            free(submat);
            return NULL;
        }
    }
    // Fill the submatrix with transition probabilities from the original matrix
    for (int i = 0; i < sub_size; i++) {
        int global_i = component->vertices[i].identifier;  // Map to global index

        for (int j = 0; j < sub_size; j++) {
            int global_j = component->vertices[j].identifier;  // Map to global index

            // Get the transition probability from the original matrix
            submat->data[i][j] = original_mat->data[global_i][global_j];  // Copy relevant data
        }
    }
    return submat;  // Return extracted submatrix
}
