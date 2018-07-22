/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>

using namespace std;

int main() {
    int foo [] = { 0, 1, 2, 3, 4, 5 }; 
    int bar [] = { 6, 7, 8, 9, 10, 11 };
    int baz[12] = {0};
    for (int i = 0; i < 6; i++) {
        
        baz[(i<<1)+1]= (baz[(i<<1)] = foo[i]) == 4 ? 99 : bar[i];
        // printf("%d, %d\n", (i<<1), (i<<1)+1);
    }
    
    cout << endl;
    for (int i = 0; i < 12; i++) cout << baz[i] << ", ";
}
