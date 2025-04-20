/*****************************************************************
//
//  NAME:        Jiayi
//
//  PROJECT:     Project1
//
//  CLASS:       ICS 212
//
//  INSTRUCTOR:  Ravi Narayan
//
//  DATE:        November 3, 2024
//
//  FILE:        database.c
//
//  DESCRIPTION:
//    This file contains the function implementations for managing a 
//    bank record database. The functionality includes adding, finding,
//    printing, and deleting records, as well as reading from and writing 
//    to an external file to maintain database persistence. Additionally,
//    memory management functions ensure the proper allocation and 
//    deallocation of resources used by the records.
//
//    Functions:
//    - addRecord     : Adds a new record to the database
//    - findRecord    : Searches for a record by account number
//    - printAll      : Prints all records in the database
//    - deleteRecord  : Deletes a record using account number as a key
//    - writefile     : Saves the database to an external file
//    - readfile      : Loads records from an external file to the database
//    - cleanup       : Frees all dynamically allocated memory for records
//
****************************************************************/

using namespace std;

#include <iostream>
#include <cstring>
#include <fstream>
#include "llist.h"

/*****************************************************************
//
//  Function name: llist
//
//  DESCRIPTION:    Default constructor for the llist class. Initializes
//                  the linked list and reads data from the default 
//                  database file ("store.txt") to restore the data.
//
//  Parameters:     None
//
//  Return values:  None
//
****************************************************************/

llist::llist() //constructor
{
    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: list() called" << endl;
    cout << "filename: " << "store.txt" << endl;
    #endif

    (*this).start = NULL; //initializes first record
    strncpy((*this).filename, "store.txt", 19);
    (*this).readfile();

    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: list() end" << endl;
    #endif
}

/*****************************************************************
//
//  Function name: llist
//
//  DESCRIPTION:    Initializes the linked list and reads the database
//                  file from the disk to restore the data.
//
//  Parameters:     filename (char[]) : The name of the file to read the 
//                                      data from.
//
//  Return values:  None
//
****************************************************************/

llist::llist(char filename[])
{
    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: llist(char[]) called" << endl;
    cout << "filename: " << filename << endl;
    #endif

    (*this).start = NULL;
    strncpy((*this).filename, filename, 19);
    (*this).readfile();

    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: llist(char[]) end" << endl;
    #endif
}

/*****************************************************************
//
//  Function name:  llist
//
//  DESCRIPTION:    This copy constructor creates a new list by copying
//                  the data from an existing list. It is used when
//                  initializing a new list with the contents of another.
//
//  Parameters:     llist (const llist&) : The list whose data is to be copied.
//
//  Return values:  None
//
****************************************************************/

llist::llist(const llist &llist) 
{
    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: llist(const llist &)  called" << endl;
    cout << "filename: " << llist.filename << endl;
    #endif

    struct record * loop_pointer = llist.start;

    this->start = NULL;

    strncpy((*this).filename, "store_copy.txt", 19);

    while (loop_pointer != NULL)
    {
        (*this).addRecord(loop_pointer->accountno, loop_pointer->name, loop_pointer->address);
        loop_pointer = loop_pointer->next;
    }

    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: llist(const llist &) end" << endl;
    #endif
}

/*****************************************************************
//
//  Function name: ~llist
//
//  DESCRIPTION:   This destructor saves the list data to disk and
//                 deallocates the memory used by the list on the heap.
//
//  Parameters:     None
//
//  Return values:  None
//
****************************************************************/

llist::~llist()
{
    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: ~llist() called" << endl;
    #endif

    writefile();
    cleanup();

    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: ~llist() end" << endl;
    #endif
}

/*****************************************************************
//
//  Function name: operator=
//
//  DESCRIPTION:    The assignment operator overwrites the existing data
//                  with the data from the provided list. This is used
//                  when assigning a new list to an already initialized list.
//
//  Parameters:     list (const llist&) : The list from which data is copied.
//
//  Return values:  llist& : The list that receives the copied data
//
****************************************************************/

llist& llist::operator=(const llist& llist)
{
    struct record *loop_pointer = llist.start;

    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: operator=(const llist& llist) called" << endl;
    #endif

    strncpy(this->filename, "assign-db.txt", 19);

    this->cleanup();

    while (loop_pointer != NULL)
    {
        this->addRecord(loop_pointer->accountno, loop_pointer->name, loop_pointer->address);
        loop_pointer = loop_pointer->next;
    }

    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: operator=(const llist& llist) end" << endl;
    #endif

    return *this;
}

/*****************************************************************
//
//  Function name: operator<<
//
//  DESCRIPTION:   This insertion operator enables direct output of
//                 the list class, printing all the records currently
//                 stored in the list.
//
//  Parameters:    stream (ostream&) : The output stream
//                 llist (const llist&) : The list whose data is to be printed.
//
//  Return values:  ostream& : The output stream containing the printed data.
//
****************************************************************/

ostream& operator<<(ostream &stream, const llist &llist)
{
    struct record *loop_pointer = llist.start;

    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: operator<<(ostream &stream, const llist &llist) called" << endl;
    #endif
    if (loop_pointer == NULL)
    {
        stream << "No records in database" << endl;
    }
    else
    {
        while (loop_pointer != NULL)
        {
            stream << "#  Account: " << loop_pointer->accountno << endl;
            stream << "#>    Name: " << loop_pointer->name << endl;
            stream << "#> Address: " << loop_pointer->address << endl;
            loop_pointer = loop_pointer->next;
        }
    }

    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE: operator<<(ostream &stream, const llist &llist) end" << endl;
    #endif

    return stream;
}

/*****************************************************************
//
//  Function name: addRecord
//
//  DESCRIPTION:   Adds a new record to the linked list of records. 
//                 This function takes the pointer to the head of the 
//                 list, an account number, a name, and an address, 
//                 and it creates a new record to be added.
//
//  Parameters:    
//                 accountno (int)         : the account number to be added
//                 name (char[])           : the name associated with the account
//                 address (char[])        : the address associated with the account
//
//  Return values:  0  : success
//                  -1 : failure
//
****************************************************************/

int llist::addRecord(int uaccountno, char uname[], char uaddress[])
{
    int valueToReturn = 0;
    int accountExists = 0;  /* Flag to track if the account already exists */
    struct record* searchTemp;
    struct record* prevPointer = NULL;

    #ifdef DEBUG
    cout << "\nDEBUG MESSAGE:" << endl;
    cout << "The account number received is: " << uaccountno << endl;
    cout << "The account name received is: " << uname << endl;
    cout << "The account address received is: " << uaddress << endl;
    #endif

    searchTemp = this->start; /* Check if account already exists */
    while (searchTemp != NULL)
    {
        if (searchTemp->accountno == uaccountno)
        {
            cout << "Account already exists" << endl;
            accountExists = 1;
        }
        searchTemp = searchTemp->next;
    }

    if (accountExists == 0)  /* create new record only if no duplicate account was found */
    {
        struct record * newRecordPointer = new struct record;
        newRecordPointer->accountno = uaccountno;
        strncpy(newRecordPointer->name, uname, 25);
        strncpy(newRecordPointer->address, uaddress, 45);
        newRecordPointer->next = NULL;

        if (this->start == NULL || uaccountno < (this->start)->accountno) /* Insert at the beginning if the list is empty or uaccountno is smallest */
        {
            newRecordPointer->next = this->start;
            this->start = newRecordPointer;
        }
        else
        {
            prevPointer = this->start; /* Find the correct position to insert in sorted order */
            searchTemp = (this->start)->next;

            while (searchTemp != NULL && uaccountno > searchTemp->accountno)
            {
                prevPointer = searchTemp;
                searchTemp = searchTemp->next;
            }

            prevPointer->next = newRecordPointer;
            newRecordPointer->next = searchTemp;
        }
    }
    else
    {
        valueToReturn = -1;  /* Indicate failure if account already exists */
    }

    #ifdef DEBUG
        cout << "DEBUG MESSAGE: addRecord complete." << endl;
    #endif

    return valueToReturn;
}

/*****************************************************************
//
//  Function name: printAllRecords
//
//  DESCRIPTION:   Prints all records in the linked list. This function 
//                 traverses through the linked list of records and 
//                 prints the details of each record.
//
//  Parameters: none
//
//  Return values:  void
//
****************************************************************/

void llist::printAllRecords()
{
    struct record* current = this->start;

    #ifdef DEBUG
        cout << "\nDEBUG MESSAGE: printAllRecords called, printing started" << endl;
    #endif

        // Check if the list is empty
        if (current == NULL)
        {
            cout << "No records available." << endl;
            return;
        }

        // Print all records
        while (current != NULL)
        {
            cout << "\nAccount Number: " << current->accountno << endl;
            cout << "Name: " << current->name << endl;
            cout << "Address: " << current->address << endl;
            cout << "------------------------" << endl;
            current = current->next;
        }

    #ifdef DEBUG
        cout << "DEBUG MESSAGE: printAllRecords complete." << endl << endl;
    #endif

}

/*****************************************************************
//
//  Function name: findRecord
//
//  DESCRIPTION:   Searches for a record with a given account number. 
//                 If the record is found, the account details are printed.
//
//  Parameters:    
//                 accountno (int)         : the account number to search for
//
//  Return values:  0  : success (record found)
//                  -1  : failure (record not found)
//
****************************************************************/


int llist::findRecord(int accountno)
{
    struct record* current = this->start;
    int found = -1; 

    #ifdef DEBUG
        cout << "\nDEBUG MESSAGE: findRecord called, searching for account number " << accountno << endl;
    #endif

    while (current != NULL && found == -1)
    {
        if (current->accountno == accountno)
        {
            cout << "Record found:" << endl;
            cout << "Account Number: " << current->accountno<< endl;
            cout << "Name: " << current->name<< endl;
            cout << "Address: " << current->address<< endl;
            found = 0;
        }
        else
        {
            current = current->next;
        }
    }

    if (found == -1)
    {
        cout << "No record found with account number: " << accountno << endl;
    }

    if (found == 0)
    {
        cout << "Record successfully found" << endl;
    }

    #ifdef DEBUG
        cout << "DEBUG MESSAGE: findRecord complete." << endl << endl;
    #endif

    return found; 
}

/*****************************************************************
//
//  Function name: deleteRecord
//
//  DESCRIPTION:   Removes a record with a specified account number 
//                 from the linked list. The record is deallocated 
//                 and the list is updated accordingly.
//
//  Parameters:    
//                 accountno (int)         : the account number of the record to delete
//
//  Return values:  0  : success
//                  -1  : failure (record not found or could not be deleted)
//
****************************************************************/

int llist::deleteRecord(int uaccountno)
{
    struct record *searchTemp = this->start;
    struct record *prevPointer = NULL;
    int valueToReturn = -1;  /* Assume no match will be found */

    #ifdef DEBUG
        cout << "\nDEBUG MESSAGE: Attempting to delete record with account number " << uaccountno << "..." << endl;
    #endif


    if (this->start == NULL)
    {
        cout << "Error: The list is empty. No records to delete.\n";
        valueToReturn = -1;
    }
    else if ((this->start)->accountno == uaccountno)  /* Check if the first record matches */
    {
        this->start = (this->start)->next;  /* Update the start pointer to skip the first record */
        delete searchTemp;         /* Free the deleted record */
        cout << "Record with account number " << uaccountno << " deleted successfully (first record).\n";
        valueToReturn = 0;         /* Indicate success */
    }
    else
    {
        /* Traverse the list to find the matching record */
        while (searchTemp != NULL)
        {
            if (searchTemp->accountno == uaccountno)
            {
                prevPointer->next = searchTemp->next;  /* Bypass the record to delete */
                delete searchTemp;                    
                cout << "Record with account number " << uaccountno << " deleted successfully.\n";
                valueToReturn = 0;                     
                searchTemp = NULL;                     
            }
            else
            {
                prevPointer = searchTemp;
                searchTemp = searchTemp->next;
            }
        }

        /* If no match was found */
        if (valueToReturn == -1)
        {
            cout << "Error: No record found with account number " << uaccountno << ".\n";
        }
    }

    #ifdef DEBUG
        cout << "DEBUG MESSAGE: Deletion process complete." << endl << endl;
    #endif


    return valueToReturn;
}

/*****************************************************************
 *
 *  Function name: writefile
 *
 *  DESCRIPTION:   Writes all records from the linked list to a
 *                 specified file.
 *
 *  Parameters:   none
 *
 *  Return values:  0  : success
 *                  -1 : failure (could not open the file)
 *
 ****************************************************************/
int llist::writefile()
{
    int returnValue = -1;
    struct record *current;

    #ifdef DEBUG
        cout << "\nDEBUG MESSAGE: writing to file with the filename " << filename << endl;
    #endif

    ofstream outfile(this->filename);  // Open the file for writing

    if (outfile.is_open())
    {
        returnValue = 0;
        current = start;

        while (current != NULL)
        {
            outfile << current->accountno << "\n";
            outfile << current->name << "\n";
            outfile << current->address << "|";
            current = current->next;
        }

        outfile.close();  // Close the file
    }
    else
    {
        cout << "Error: Unable to open file " << this->filename << " for writing.\n";
    }

    #ifdef DEBUG
        cout << "DEBUG MESSAGE: File writing complete." << endl << endl;
    #endif

    return returnValue;
}

/*****************************************************************
//
//  Function name: cleanup
//
//  DESCRIPTION:   This function deallocates all nodes in the linked
//                 list, freeing the memory associated with each 
//                 record and setting the head pointer to NULL.
//
//  Parameters:    none
//
//  Return values: None
//
****************************************************************/

void llist::cleanup()
{
    struct record *current = this->start;
    struct record *next;

    #ifdef DEBUG
        cout << "\nDEBUG MESSAGE: starting cleanup" << endl;
    #endif


    while (current != NULL)
    {
        next = current->next;
        delete current;
        current = next;
    }

    this->start = NULL;

    #ifdef DEBUG
        cout << "DEBUG MESSAGE: finished cleanup" << endl << endl;
    #endif
}


/*****************************************************************
//
//  Function name: readfile
//
//  DESCRIPTION:   reads from the file written by writefile and 
//                 stores the records into a linked list to be manipulated
//                 by user interface options
//
//  Parameters:    none
//
//  Return values:  0 : file read successfully
//                  -1 : file was not read successfully
//
 ****************************************************************/

int llist::readfile()
{
    int returnValue = -1;
    int accountnum;
    ifstream infile(this->filename, ifstream::in);  // Open the file for reading

    #ifdef DEBUG
        printf("\nDEBUG MESSAGE: reading from file with the filename %s\n", filename);
    #endif


    if (infile.is_open())
    {
        while (infile >> accountnum)  // Read account number
        {
            infile.ignore();  // Ignore the newline after the account number

            char name[25];
            char address[45];
            char c;
            size_t i = 0;

            infile.getline(name, sizeof(name));  // Read the name
            name[strcspn(name, "\n")] = 0;       // Remove newline character if present

            // Read the address until '|' is encountered
            while (infile.get(c) && c != '|' && i < sizeof(address) - 1)
            {
                address[i++] = c;
            }
            address[i] = '\0';  // Null-terminate the address

            // Add the record to the list
            addRecord(accountnum, name, address);

            returnValue = 0;  // Indicate success
        }

        infile.close();  // Close the file
    }
    else
    {
        cout << "Error: Unable to open file " << this->filename << " for reading.\n";
    }

    #ifdef DEBUG
        printf("\nDEBUG MESSAGE: finished reading from file\n\n");
    #endif

    return returnValue;
}



