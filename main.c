#include <stdio.h>
#include "functions.h"


int main() {
        // Test the functions of part 1 with the file exemple1.txt
        //Note that, as your path will likely be different from the one here, it is necessary to change "C:/Users/USER/Downloads" to where you stored the project.
        //Absolute path is necessary for the project to work
        a_list list;
        // Load, display and verify graph from file
        list = readGraph("C:/Users/USER/Downloads/TI_301_PRJ_STUDENTS-master/data/exemple1.txt");
        display_a_list(&list);
        int n;
        n=check_graph("C:/Users/USER/Downloads/TI_301_PRJ_STUDENTS-master/data/exemple1.txt");  // Validate probability sums
        // Print validation result (1=valid, 0=invalid)
        if (n==1) {
                printf("\nThe graph is valid\n");
        }
        else {
                printf("\nThe graph is not valid\n");
        }
        export_graph(&list, "C:/Users/USER/Downloads/TI_301_PRJ_STUDENTS-master/ex.txt");  // Export for visualization
        printf("\nTarjan Algorithm - Strongly Connected Components:\n");
        tarjan(&list);  // find and display strongly connected components
        t_partition partition = compute_partition(&list);  // stor component partition
        printf("\n");
        int **class_links = Hasse(&list, &partition);  // build Hasse diagram of components
        analyze_graph_characteristics(&partition, class_links, partition.size);  // analyze transient/persistent states
        printf("\nMatrix Functions Test:\n");
        matrix *transition_mat = create_transition_matrix(&list);  // Create probability matrix
        matrix *zero_mat = create_zero_matrix(list.size);  // Create zero matrix
        matrix *squared_mat = multiply_matrices(transition_mat, transition_mat);  // compute P²
        float diff1 = matrix_difference(transition_mat, zero_mat);  // compare P vs zero matrix
        float diff2 = matrix_difference(transition_mat, squared_mat);  // Compare P vs P^2
        printf("Difference between transition and zero matrix: %.4f\n", diff1);
        printf("Difference between transition and squared matrix: %.4f\n", diff2);
        matrix *copied_mat = create_zero_matrix(list.size);  // create target for copy
        copy_matrix(copied_mat, transition_mat);  // test matrix copying
        float diff3 = matrix_difference(transition_mat, copied_mat);  // verify the copy's accuracy
        printf("Difference after copy (should be 0): %.4f\n", diff3);
        printf("\nSubMatrix Test:\n");
        // extract and display the component submatrixes
        for (int i = 0; i < partition.size; i++) {
                matrix *submat = subMatrix(transition_mat, &partition, i);
                if (submat != NULL) {
                        printf("Component %s: submatrix size %dx%d\n",
                               partition.classes[i].name, submat->size, submat->size);
                }
        }
        return 0;
}
