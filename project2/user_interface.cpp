/*****************************************************************
//
//  NAME:        Jiayi
//
//  HOMEWORK:    Project1
//
//  CLASS:       ICS 212
//
//  INSTRUCTOR:  Ravi Narayan
//
//  DATE:        Dec 3, 2024
//
//  FILE:        user_interface.cpp
//
//  DESCRIPTION:
//   This program manages a simple record system with options to
//   add, print, find, delete, and quit the program.
//
****************************************************************/

#include <iostream>
#include <cstring>
#include "llist.h"

using namespace std;
void getaddress(char address[], int size);

/*****************************************************************
//
//  Function name: main
//
//  DESCRIPTION:   Entry point for the Bank Record Management System.
//                 Provides an interactive menu for the user to manage 
//                 bank records, supporting commands such as adding,
//                 printing, finding, and deleting records. If executed
//                 with the "debug" argument, debug messages will be enabled.
//
//  Parameters:    argc (int)     : Argument count, should be 1 or 2
//                 argv (char*[]) : Argument values, optional second
//                                  argument to enable debug mode
//
//  Return values:  0 : Program executed successfully
//
****************************************************************/

int main(int argc, char* argv[])
{
    char filename[] = "output0.txt";
    llist *start = new llist(filename); //create a list object pointer to store address of list object on heap
    int quit = 0;

    #ifdef DEBUG
        printf(">>> DEBUG MODE\n");
    #endif

    cout << "\nWelcome to the Bank Record Management System!" << endl;
    cout << "Here are your available options:" << endl;
    cout << "--------------------------------------------------------" << endl;
    cout << "add       : Add a new record to the database" << endl;
    cout << "printall  : Display all records currently in the database" << endl;
    cout << "find      : Search for a record using a specific account number" << endl;
    cout << "delete    : Remove a record from the database using the account number as a key" << endl;
    cout << "quit      : Exit the program" << endl;
    cout << "--------------------------------------------------------" << endl;

    while (!quit)
    {
        int accountnum = 0;
        char name[25];
        char address[45];
        char option[100]; 

        cout << "Please enter your choice (add, printall, find, delete, quit): " << endl;
        cin.getline(option, sizeof(option));

        if (strlen(option) != 0 && strncmp(option, "add", strlen(option)) == 0)
        {
            int validInputCount = 0; 
            int maxValidInputs = 6;  
            int iquit1 = 0;          

            while (!iquit1 && validInputCount < maxValidInputs)
            {
                cout << "Enter account number (up to 6 digits): " << endl;

                if (!(cin >> accountnum)) 
                {
                    cout << "Error, enter valid numbers." << endl;;
                    cin.clear(); 
                    cin.ignore(1000, '\n'); // clear input buffer
                }
                else if (accountnum < 0 || accountnum > 999999) 
                {
                    cout << "Error, enter a positive integer up to 6 digits." << endl;
                } 
                else 
                {
                    validInputCount++; 
                    cout << "Account number accepted: " << accountnum << endl;

                    iquit1 = 1; 
                }
            }

            cin.ignore(); // Clear newline from input buffer

            int condition = 0;
            while (condition == 0)
            {
                cout << "Enter name (maximum 20 characters): " << endl;
                char buffer[100];
                cin.getline(buffer, 100, '\n');
                if (strlen(buffer) == 0)
                {
                    cout << "Error, Invalid Input. Enter a character" << endl;
                    cin.clear();
                }
                else
                {
                    strncpy(name, buffer, 20);
                    condition = 1;
                }
            }

            getaddress(address, sizeof(address)); // Get address input
            cin.ignore(1000, '\n'); // clear input buffer
            start->addRecord(accountnum, name, address);
        }
        else if (strncmp(option, "printall", strlen(option)) == 0 && strlen(option) != 0)
        {
            start->printAllRecords();
        }
        else if (strncmp(option, "find", strlen(option)) == 0 && strlen(option) != 0)
        {
            int validInputCount = 0; 
            int maxValidInputs = 6;  
            int iquit1 = 0;          

            while (!iquit1 && validInputCount < maxValidInputs)
            {
                cout << "Enter account number (up to 6 digits): " << endl;

                if (!(cin >> accountnum)) 
                {
                    cout << "Error, enter valid numbers. " << endl;
                    cin.clear();
                    cin.ignore(1000, '\n'); // clear input buffer
                }
                else if (accountnum < 0 || accountnum > 999999) 
                {
                    cout << "Error, enter a positive integer up to 6 digits." << endl;
                } 
                else 
                {
                    validInputCount++; 
                    cout << "Account number accepted: " << accountnum << endl;

                    iquit1 = 1; 
                }
            }
 
            cin.ignore(); // Clear newline from input buffer
            start->findRecord(accountnum);
        }
        else if (strncmp(option, "delete", strlen(option)) == 0 && strlen(option) != 0)
        {
            int validInputCount = 0; 
            int maxValidInputs = 6;  
            int iquit1 = 0;          

            while (!iquit1 && validInputCount < maxValidInputs)
            {
                cout << "Enter account number (up to 6 digits): " << endl;

                if (!(cin >> accountnum)) 
                {
                    cout << "Error, enter valid numbers." << endl;
                    cin.clear();
                    cin.ignore(1000, '\n'); // clear input buffer
                }
                else if (accountnum < 0 || accountnum > 999999) 
                {
                    cout << "Error, enter a positive integer up to 6 digits." << endl;
                } 
                else 
                {
                    validInputCount++; 
                    cout << "Account number accepted: " << accountnum << endl;

                    iquit1 = 1; 
                }
            }
            
            cin.ignore(); // Clear newline from input buffer
            start->deleteRecord(accountnum);
        }
        else if (strncmp(option, "quit", strlen(option)) == 0 && strlen(option) != 0)
        {
            quit = 1;
            cout << "Exiting program." << endl;
        }
        else
        {
            cout << "Invalid option. Please try again." << endl;
            cout << "Option entered: [" << option << "]" << endl;
        }
    }
    
    delete start;
    return 0;
}

/*****************************************************************
//
//  Function name: getaddress
//
//  DESCRIPTION:   This function prompts the user to enter an
//                 address and stores it in a character array.
//
//  Parameters:    address (char[]) : Buffer to store the address
//                 size (int) : Size of the buffer
//
//  Return values:  None
//
****************************************************************/

void getaddress(char address[], int size)
{
    int i = 0;
    char ch = 0;

    cout << "Please enter the address:" << endl;;
    cout << "When finished typing, add a | at the end." << endl;;

    while (i < size - 1 && ch != '|')
    {        
        ch = cin.get(); // Use cin.get() to get characters one by one
        if (ch != '|')
        {
            address[i++] = ch;
        }
    }
    address[i] = '\0';
}


