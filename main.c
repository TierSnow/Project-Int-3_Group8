#include <stdio.h>
#include "functions.h"


int main() {
        a_list list;
        list = readGraph("C:/Users/USER/Downloads/TI_301_PRJ_STUDENTS-master/data/exemple1.txt");
        display_a_list(&list);
        int n;
        n=check_graph("C:/Users/USER/Downloads/TI_301_PRJ_STUDENTS-master/data/exemple1.txt");
        printf("%d\n",n);
        export_graph(&list, "C:/Users/USER/Downloads/TI_301_PRJ_STUDENTS-master/ex.txt");
        return 0;
}

