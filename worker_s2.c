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
#include "service2.h"

static void rsleep (int t);


int main (int argc, char * argv[])
{
    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;
    
    // Open the request message queue
    mq_fd_request = mq_open(argv[0], O_RDONLY);
    //perror("mq_fd_request failed to open in worker_1")

    // Check if the request message queue is opened
    if (mq_fd_request == -1)
    {
        // perror ("mq_open() for request failed in worker_s1.c");
        exit (0);
    }

    // Open the response message queue
    mq_fd_response = mq_open(argv[1], O_WRONLY); 

    // Check if the response message queue is opened
    if (mq_fd_response == -1)
    {
        // perror ("mq_open() for response queue failed in worker_s1.c");
        exit (0);
    }

    //while there are messages in the queue retrieve them
    while (true)
    {
        if (mq_receive (mq_fd_request, (char *)&req, sizeof(req), NULL) > -1){
            if(req.data == -1 && req.RequestID == -1 && req.ServiceID == -1){
                //close the message queue
                mq_close(mq_fd_response);
                mq_close(mq_fd_request);
                return(0);
            }
            //perror("mq_receive worker 2");
            //sleep for 10000 ms
            rsleep(10000);
            //calculate the result of the service
            int result = service(req.data);
            //printf("w1 result: %u\n", result);
            //create the response message
            rsp.RequestID = req.RequestID;
            rsp.result = result;
            //send the response message
            mq_send(mq_fd_response, (char *)&rsp, sizeof(rsp), NULL);
            //perror("sending w1");

            //perror("Worker trying to send in worker_2");    
        }
    };
    return (0);
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;
    
    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}
