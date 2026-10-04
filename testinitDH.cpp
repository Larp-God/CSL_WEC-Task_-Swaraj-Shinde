#include "../headers.h"
#include "../common.cpp"
using namespace std;

int main(){
    int testinitdh = initDH();
    if(!testinitdh){
        cout << "DH initialisation succesfully working" << endl;
    }else{
        cout << "DH initialisation failed" << endl;
    }
    return 0;
}