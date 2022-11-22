/* 
 * Operating Systems {2INCO} Practical Assignment
 * Interprocess Communication
 *
 * Contains definitions of message data structures
 *
 */

#ifndef MESSAGES_H
#define MESSAGES_H

// define the data structures for your messages here
typedef struct
{
    // a data structure with 3 members
    int                     a;
    int                     b;
    char                    c;
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // a data structure with 3 members
    int                     e;
    char                    f[20];
    char                    g[20];
} MQ_RESPONSE_MESSAGE;
#endif

