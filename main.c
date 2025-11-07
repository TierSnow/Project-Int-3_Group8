#include <stdio.h>
#include "functions.h"


int main() {
        //test the functions of part 1 with the file exemple1.txt.
        //Note that, as your path will likely be different from the one here, it is necessary to change "C:/Users/USER/Downloads" to where you stored the project.
        //Absolute path is necessary for the project to work
        a_list list;
        list = readGraph("C:/Users/USER/Downloads/TI_301_PRJ_STUDENTS-master/data/exemple1.txt");
        display_a_list(&list);
        int n;
        n=check_graph("C:/Users/USER/Downloads/TI_301_PRJ_STUDENTS-master/data/exemple1.txt");
        printf("%d\n",n);
        export_graph(&list, "C:/Users/USER/Downloads/TI_301_PRJ_STUDENTS-master/ex.txt");
        return 0;
}

