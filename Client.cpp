#include <iostream>
using namespace std;

int main(){
    bool exitBool = false;
    do{
        cout<<"Enter the client IP which you would like to talk to: ";
        string friendIP;
        getline(cin, friendIP, '\n');

        cout<<endl;

        string message;
        do{
            cout<<"YOU: ";    
            getline(cin, message, '\n');
            if (message == "exit")
                exitBool = true;
        }    while (message != "close" or exitBool);
    } while (exitBool);    
}