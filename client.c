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
    mqd_t               mq_fd_request;  /* Message queue file descriptor */
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  request;
    MQ_RESPONSE_MESSAGE response;
    struct mq_attr      attr;

    // Open the message queue
    mq_fd_request = mq_open (argv[1], O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

    // Check if the message queue is opened
    if (mq_fd_request == -1)
    {
        perror ("mq_open() failed");
        exit (1);
    }

    // // Open the response queue
    // mq_fd_response = mq_open (argv[2], O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

    // // Check if the response queue is opened
    // if (mq_fd_response == -1)
    // {
    //     perror ("mq_open() failed");
    //     exit (1);
    // }

    // Get the process ID
    processID = getpid();

    // Get the next job request
    while (get_next_request(&request))
    {
        // Send the request to the Req message queue
        if (mq_send (mq_fd_request, (char *) &request, sizeof(request), 0) == -1)
        {
            perror ("mq_send() failed");
            exit (1);
        }

        // Receive the response from the Rsp message queue
        if (mq_receive (mq_fd_response, (char *) &response, sizeof(response), NULL) == -1)
        {
            perror ("mq_receive() failed");
            exit (1);
        }

        // Print the response
        printf ("Client %d: %s", processID, response.text);
    }

    // Close the message queue
    mq_fd_request = mq_close (mq_fd_request);

    // Check if the message queue is closed
    if (mq_close (mq_fd_request) == -1)
    {
        perror ("mq_close() failed");
        exit (1);
    }
    
    return (0);
}