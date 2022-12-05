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
#include <errno.h>      // for perror()
#include <unistd.h>     // for getpid()
#include <mqueue.h>     // for mq-stuff
#include <time.h>       // for time()

#include "messages.h"
#include "request.h"

static void rsleep (int t);


int main (int argc, char * argv[])
{
    perror("client_init");
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the message queue (whose name is provided in the
    //    arguments)
    //  * repeatingly:
    //      - get the next job request 
    //      - send the request to the Req message queue
    //    until there are no more requests to send
    //  * close the message queue
    
    pid_t               processID;      /* Process ID from fork() */
    mqd_t               mq_fd_request;  /* Message queue file descriptor for requests*/
    MQ_REQUEST_MESSAGE  request;
    
    // Open the message queue
    mq_fd_request = mq_open (argv[0], O_WRONLY);
    perror("mq_fd_request");

    // Check if the message queue is opened
    if (mq_fd_request == -1)
    {
        perror ("mq_open() failed in client.c");
        exit (1);
    }

    // Get the process ID
    processID = getpid();
    //printf("Client process ID: %d\n", processID); // test

    // Get the next job request
    while (getNextRequest(&request.RequestID, &request.data, &request.ServiceID) >-1)
    {
        // Send the request to the Req message queue
        if (mq_send (mq_fd_request, (char *) &request, sizeof(request), NULL) == -1)
        {
            perror ("mq_send() failed in client.c");
            exit (1);
        }
    }

    // // Check if the message queue is closed
    // if (mq_close (mq_fd_request) == -1)
    // {
    //     perror ("mq_close() failed in client.c");
    //     exit (1);
    // }
    //printf("client done\n");
    return (0);
}