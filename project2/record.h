#ifndef RECORD_H
#define RECORD_H
struct record
{
    int                accountno;
    char               name[25];
    char               address[45];
    struct record*     next;
};
#endif
