/* 
 * Operating Systems  (2INCO)  Practical Assignment
 * Interprocess Communication
 *
 * Jip van Hoef (1555960)
 * Thymo Fieten (1528157)
 * Koert Schoenmakers (1534963)
 * 
 * Grading:
 * Your work will be evaluated based on the following criteria:
 * - Satisfaction of all the specifications
 * - Correctness of the program
 * - Coding style
 * - Report quality
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>    
#include <unistd.h>    // for execlp
#include <mqueue.h>    // for mq


#include "settings.h"  
#include "messages.h"

static char Req_queue_33[80];
static char S1_queue_33[80];
static char S2_queue_33[80];
static char Rsp_queue_33[80];


static void
getattr(mqd_t mq_fd)
{
    struct mq_attr attribute;
    int            returnvalue;

    returnvalue = mq_getattr(mq_fd,&attribute);
    if (returnvalue == -1)
    {
        perror ("mq_getattr() failed");
        exit (1);
    }
    fprintf (stderr, "%d: mqdes=%d max=%ld size=%ld nrof=%ld\n",
                getpid(), 
                mq_fd, attribute.mq_maxmsg, attribute.mq_msgsize, attribute.mq_curmsgs);
}



int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }
        
    // TODO:
    //  * create the message queues (see message_queue_test() in
    //    interprocess_basic.c)
    pid_t               processID;      /* Process ID from fork() */
    //message queue descriptors
    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    mqd_t               mq_s1_request;
    mqd_t               mq_s2_request;
    //create the request and response messages
    MQ_REQUEST_MESSAGE  request;
    MQ_RESPONSE_MESSAGE response;
    //create the message queue attribute
    struct mq_attr      attribute;

    //set the maximum number of messages that can be stored in the queue to 10
    attribute.mq_maxmsg = 10;
    //set the maximum size of the message queue to the size of the request message
    
    attribute.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
    //open the 3 request queues with their name and the write only property
    mq_fd_request = mq_open(Req_queue_33,O_WRONLY | O_CREAT | O_EXCL,&attribute);

    mq_s1_request = mq_open(S1_queue_33,O_WRONLY | O_CREAT | O_EXCL,&attribute);

    mq_s2_request = mq_open(S2_queue_33,O_WRONLY | O_CREAT | O_EXCL,&attribute);
    
    //set the maximum number of messages that can be stored in the queue to 10
    attribute.mq_maxmsg = 10;
     //set the maximum size of the message queue to the size of the request message
    attribute.mq_msgsize = sizeof (MQ_RESPONSE_MESSAGE);

    //open the response message queue that is read only
    mq_fd_response = mq_open(Rsp_queue_33,O_RDONLY | O_CREAT | O_EXCL,&attribute);
    

    //call the function that gets the attributes(not sure what it does or why we need it)
    getattr(mq_fd_request);
    getattr(mq_s1_request);
    getattr(mq_s2_request);
    getattr(mq_fd_response);

    //TODO
    //  * create the child processes (see process_test() and
    //    message_queue_test())
    //  * keep a dictionarry mapping clients' PIDs to a clientID
    //  * read requests from the Req queue and transfer them to services
    //  * read answers from services in the Rep queue and print them
    //  * wait until the clients have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues
    // contain your goup number (to ensure uniqueness during testing)
    
    return (0);
}
