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

    //get the name of the request and response queues
    char mq_name1 = (char *) argv[0];
    char mq_name2 = (char *) argv[1];
    
    //open both of the queues
    mq_fd_request = mq_open(mq_name1, O_RDONLY);
    mq_fd_response = mq_open(mq_name2, O_WRONLY); 

    //while there are messages in the queue retrieve them
    while (mq_receive (mq_name1,(char *)&req,sizeof(req),0)> -1){
        //sleep for 10000 ms
        rsleep(10000);
        //calculate the result of the service
        int result = service(req.data);
        //create the response message
        rsp.RequestID = req.RequestID;
        rsp.result = result;
        //send the response message
        mq_send(mq_fd_response, (char *)&rsp, sizeof(rsp),0);
    };
    //close the message queue
    mq_close(mq_fd_response);
    mq_close(mq_fd_request);
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the two message queues (whose names are provided in the
    //    arguments)
    //  * repeatingly:
    //      - read from the SX message queue the new job to do
    //      - wait a random amount of time (e.g. rsleep(10000);)
    //      - do the job 
    //      - write the results to the Rep message queue
    //    until there are no more tasks to do
    //  * close the message queues
    
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
