#include <iostream>

#include "matio_private.h"
// #include "matio.h"

class Test: public mat_t {
    public:
        Test() {
            std::cout << "Test class created!" << std::endl;
        }
        ~Test() {
            std::cout << "Test class destroyed!" << std::endl;
        }
};


int main() {
    mat_t *matfp;
    printf("size of matfp: %lu\n", sizeof(Test));

    matfp = Mat_CreateVer("test.mat", NULL, MAT_FT_MAT5);
    if (matfp) {
        std::cout << "Matio compilation test successful!" << std::endl;
        Mat_Close(matfp);
    } else {
        std::cout << "Matio compilation test failed!" << std::endl;
    }
    
    return 0;
}
