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
    int                     RequestID;
    int                     ServiceID;
    int                     data;
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // a data structure with 2 members
    int                     RequestID;
    int                     result;
} MQ_RESPONSE_MESSAGE;
#endif

