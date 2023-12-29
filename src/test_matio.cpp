#include <iostream>

#include "matio_private.h"


int main() {
    mat_t *matfp;
    printf("size of matfp: %lu\n", sizeof(mat_t));

    matfp = Mat_CreateVer("test.mat", NULL, MAT_FT_MAT5);
    if (matfp) {
        std::cout << "Matio compilation test successful!" << std::endl;
        Mat_Close(matfp);
    } else {
        std::cout << "Matio compilation test failed!" << std::endl;
    }
    
    return 0;
}
