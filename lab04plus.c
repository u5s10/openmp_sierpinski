#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define n 2187
unsigned char color[n][n][3];

void drawRectangle(int row, int col, int level){
    int size = n/pow(3,level);
    int offSet = (size/2);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            color[row-offSet+i][col-offSet+j][0] = 255;
            color[row-offSet+i][col-offSet+j][1] = 255;
            color[row-offSet+i][col-offSet+j][2] = 255;
        }
        
    }
}

void sierpinskiCarpet(int row, int col, int level){
    int size = n/pow(3,level);
    int offSet = size/2;

    drawRectangle(row, col, level);
    if(level == 6)
        return;

    #pragma omp task firstprivate(row, col, offSet, level)        
    sierpinskiCarpet(row, col-offSet*2,level+1);

    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinskiCarpet(row,col+offSet*2,level+1);

    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinskiCarpet(row + offSet*2, col,level+1);

    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinskiCarpet(row - offSet*2,col,level+1);

    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinskiCarpet(row - offSet*2,col + offSet*2,level+1);

    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinskiCarpet(row - offSet*2,col - offSet*2,level+1);

    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinskiCarpet(row + offSet*2,col + offSet*2,level+1);

    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinskiCarpet(row + offSet*2,col - offSet*2,level+1);

    #pragma omp taskwait
    return;
}

int main()
{

    FILE * fp;
    char *filename="new1.ppm";
    char *comment="# ";
    fp = fopen(filename,"wb");
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, n, n, 255);

    int level = 1;
    int size = n/pow(3,level);
    int offSet = size/2;

    omp_set_dynamic(0);
    omp_set_num_threads(4);
    #pragma omp parallel
    {
        #pragma omp single
        sierpinskiCarpet(size + offSet,size + offSet,level);
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fwrite(color[i][j],1,3,fp);
        }
        
    }
    fclose(fp);
    return 0;
}