#include <iostream>
#include "FPGrowth.hpp"
using namespace std;

int main() 
{
    cout << "============================= program start =============================" 
         << endl << endl << endl;

    int minSupportCount;

    minSupportCount = 2;
    FPGrowth pattern("Transactions.csv",minSupportCount);




    return 0;
}