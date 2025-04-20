#ifndef LLIST_H
#define LLIST_H
#include <ostream>
#include "record.h"

class llist
{
    private:
        record *    start;
        char        filename[20];
        int         readfile();
        int         writefile();
        void        cleanup();

    public:
        llist();
        llist(char[]);
        llist(const llist&);
        ~llist();
        llist& operator=(const llist&);
        friend std::ostream& operator<<(std::ostream&, const llist&);
        int addRecord(int, char [ ],char [ ]);
        int findRecord(int);
        void printAllRecords();
        int deleteRecord(int);
};

#endif

