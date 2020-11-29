#include <iostream>
using namespace std;

int main(){
    cout<<"Enter the client IP which you would like to talk to: ";
    string friendIP;
    getline(cin, friendIP, '\n');

    cout<<endl;

    string message;
    do{
    cout<<"YOU: ";
    cin.ignore();
    getline(cin, message, '\n');
    }    while (message != "exit");
}