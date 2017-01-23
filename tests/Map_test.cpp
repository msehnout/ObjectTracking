#include <iostream>
#include <complex>
#include <fstream>
#include "../modules/Map.hpp"

using namespace std;

int main(int argc, char **argv)
{
    complex<double> x[20];
    int a[3];

    cout << "test nacitani ze souboru" << endl;

    ifstream file("../tests/test.text");
    string str;
    istringstream is;

    if(file.good())
    {
        cout << "ctu" << endl;
        for (int i =0; i < 3; ++i)
        {
            getline(file, str, ';');

            a[i] = stoi(str);
            cout << a[i] << endl;
        }
        for (int i =0; i < 20; ++i)
        {
            getline(file, str, ';');
            is.str(str);
            is >> x[i];
            cout << x[i] << endl;
        }
        file.close();
    }

    //cout << a << ", " << b << ", " << c << endl;



    return(0);
}