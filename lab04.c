#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define n 1024
unsigned char color[n][n][3];

void sierpinski(int row, int col, int level){
    int size = 1024/pow(2,level);
    int offSet = size/2;
    for (int i = 0; i < size; i++)
    {
        // vertical line
        color[i + row][col][0] = 255;
        color[i + row][col][1] = 255;
        color[i + row][col][2] = 255;
        // horizontal line
        color[row + size-1][col + i][0] = 255;        
        color[row + size-1][col + i][1] = 255;        
        color[row + size-1][col + i][2] = 255;        
        // diagonal line
        color[i + row][i + col][0] = 255;
        color[i + row][i + col][1] = 255;
        color[i + row][i + col][2] = 255;
    }
    if(level == 7)
        return;

    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinski(row + offSet, col + offSet, level+1);

    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinski(row + offSet, col, level+1);
    
    #pragma omp task firstprivate(row, col, offSet, level)
    sierpinski(row, col, level+1);

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
    
    omp_set_dynamic(0);
    omp_set_num_threads(4);
    #pragma omp parallel
    {
        #pragma omp single
        sierpinski(0, 0, 0);
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