// CPP program to illustrate
// file system access
#include <iostream>
#include <fstream>
using namespace std;

extern "C" void fsroot(void);

int main()
{
    cout << "calling fsroot" << endl;
    fsroot();

    char *fname = (char *) malloc(100);
    sprintf(fname, "/root/myfile.txt", 0);

    cout << "About to write data to a file" << endl;

    ofstream myfile;
    myfile.open(fname);
    myfile << "1\n2\n3\n\n4\n5\n60";
    myfile.close();

    cout << "Writing complete" << endl;
    cout << "About to read local file" << endl;

    ifstream infile;
    infile.open(fname);

    uint32_t i;
    uint32_t num = 6;
    uint16_t * input = (uint16_t*) malloc(num * sizeof(uint16_t));

    for(i = 0; i < num; i++) {
        infile >> input[i] ;
        if(infile.eof()) {break;}
    }

    infile.close();
    cout << "Local file read, values: " << endl;
    for(i = 0; i < num; i++) {
        cout << input[i]  << endl;
    }

    cout << "Done" <<endl;

    return 0;
}
