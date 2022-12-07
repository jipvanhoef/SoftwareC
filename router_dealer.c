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
#include <time.h>


#include "settings.h"  
#include "messages.h"

#define NAMES        "Fieten_Schoenmakers_Van_hoef"


static char Req_queue_33[80];
static char S1_queue_33[80];
static char S2_queue_33[80];
static char Rsp_queue_33[80];

static void rsleep (int t);

int Children_Id[1+N_SERV1+N_SERV2];
int waiting = 0;
int messages_received = 0;
int responses_received = 0;
int error_check = 0;

int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }
        
    // TODO:
    //  * create the message queues (see message_queue_test() in
    //    interprocess_basic.c)
    pid_t               c;      /* Process ID from fork() */

    //message queue descriptors
    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    mqd_t               mq_s1_request;
    mqd_t               mq_s2_request;

    //create the request and response messages
    MQ_REQUEST_MESSAGE  request;
    MQ_RESPONSE_MESSAGE response;
    
    sprintf (Req_queue_33, "/mq_request_%s_%d", NAMES, getpid());
    sprintf (S1_queue_33, "/mq_s1_request_%s_%d", NAMES, getpid());
    sprintf (S2_queue_33, "/mq_s2_request_%s_%d", NAMES, getpid());
    sprintf (Rsp_queue_33, "/mq_response_%s_%d", NAMES, getpid());


    //create the message queue attribute
    struct mq_attr      attribute;

    //set the maximum number of messages that can be stored in the queue to MQ_MAX_MESSAGES
    attribute.mq_maxmsg = MQ_MAX_MESSAGES;

    //set the maximum size of the message queue to the size of the request message
    attribute.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
     
    
    //open the 3 request queues with their name and the read-write property
    mq_fd_request = mq_open(Req_queue_33,O_RDWR | O_CREAT | O_EXCL |  O_NONBLOCK,0600 ,&attribute);
      
    mq_s1_request = mq_open(S1_queue_33,O_RDWR | O_CREAT | O_EXCL |  O_NONBLOCK, 0600, &attribute);
   
    mq_s2_request = mq_open(S2_queue_33,O_RDWR | O_CREAT | O_EXCL |  O_NONBLOCK, 0600, &attribute);
   

    //set the maximum number of messages that can be stored in the queue to MQ_MAX_MESSAGES
    attribute.mq_maxmsg = MQ_MAX_MESSAGES;
    
     //set the maximum size of the message queue to the size of the response message
    attribute.mq_msgsize = sizeof (MQ_RESPONSE_MESSAGE);
    

    //open the response message queue that is read only
    mq_fd_response = mq_open(Rsp_queue_33,O_RDONLY | O_CREAT | O_EXCL| O_NONBLOCK,0600,&attribute);

    //TODO
    //  * create the child processes (see process_test() and
    //    message_queue_test())


    // create the client process
    c = fork();
    Children_Id[0] = c;
    if (c < 0){
        perror("fork() client failed");
        exit (1);
    } else if (c == 0) {
        perror("starting client process");
        execlp ("./client", Req_queue_33, (char *)NULL); // Exectutes the client process
        perror("execlp() client failed");
    }
    
    // create the first worker process
    for (int i=0; i < N_SERV1; i++) {
        if (c > 0) {
            pid_t c = fork();
            Children_Id[i+1] = c;
            if (c < 0){
                perror("fork() w1 failed");
                exit(0);
            }
            if(c == 0) {
                execlp("./worker_s1", S1_queue_33, Rsp_queue_33, NULL); // Exectutes the worker_s1 process
                perror("execlp() worker 1 failed");
                exit(0);
            }
        }
    }
    // create the second worker process
    for (int i=0; i < N_SERV2; i++) {
        if (c > 0) {
            pid_t c = fork();
            Children_Id[i+1+N_SERV1] = c;
            if (c < 0){
                perror("fork() w2 failed");
                exit(0);
            }
            if (c==0) {
                execlp("./worker_s2", S2_queue_33, Rsp_queue_33, NULL); // Exectutes the worker_s2 process
                perror("execlp() worker 2 failed");
                exit(0);
            }
        }
    }
    perror("finished creating children");

    // loop until the client send all the requests
    while (true) {
        //check if client is done
        if(waiting == 0){
            if(waitpid(Children_Id[0],NULL, WNOHANG) == Children_Id[0]){
                waiting = 1;
            }
        }
        perror("waiting for worker");

        //get current attributes of client and request queue
        struct mq_attr mq_client_attr;
        mq_getattr(mq_fd_request, &mq_client_attr);
        struct mq_attr mq_s1_attr;
        mq_getattr(mq_s1_request, &mq_s1_attr);
        struct mq_attr mq_s2_attr;
        mq_getattr(mq_s2_request, &mq_s2_attr);

        //check if there are messages in the client request queue, and check if a new message will not exceed the maximum
        if (
            mq_client_attr.mq_curmsgs > 0
            && mq_s1_attr.mq_curmsgs < mq_s1_attr.mq_maxmsg
            && mq_s2_attr.mq_curmsgs < mq_s2_attr.mq_maxmsg
        ) {
            //pull a message and check if it works
            error_check = mq_receive (mq_fd_request,(char *)&request, sizeof(request), NULL);
            if(error_check >= 0) {
                messages_received++;
            }
            // Send to worker 1
            if (request.ServiceID == 1) { 
                mq_send(mq_s1_request, (char *)&request, sizeof(request), NULL);
                perror("sending to worker 1 queue");
                // Send to worker 2
            } else if (request.ServiceID == 2) { 
                mq_send(mq_s2_request, (char *)&request, sizeof(request), NULL);
                perror("sending to worker 2 queue");
            }
        }
        //check if there are any messages in the response queue.
        struct mq_attr mq_response_attr;
        mq_getattr(mq_fd_response, &mq_response_attr);
        if(mq_response_attr.mq_curmsgs > 0){
            //pulls a message and check if it works
            error_check = mq_receive(mq_fd_response, (char *)&response, sizeof(response), NULL);
            if(error_check >= 0){
                responses_received++;
                perror("print response");
            }
            printf("%u -> %u\n",response.RequestID, response.result);
        }
        //check if there is nothing left in both queues, if the client is terminated and 
        //if the amount of messages received is the same as the amount of messages printed on stdout
        if (
            mq_client_attr.mq_curmsgs == 0
            && mq_response_attr.mq_curmsgs == 0
            && waiting == 1
            && messages_received == responses_received
        ) {
            //if so, then break out of the main while loop
            break;
        }
    
    //send the shutdown messages to all workers
    request.data = -1;
    request.ServiceID = -1;
    request.RequestID = -1;
    for (int i = 0; i<N_SERV1; i++) {
        mq_send(mq_s1_request, (char *)&request, sizeof(request), NULL);
        perror("killed a child");
    }
    for (int i = 0; i<N_SERV2; i++) {
        mq_send(mq_s2_request, (char *)&request, sizeof(request), NULL);
    }
            
    //close and unlink all message queue's
    mq_close(mq_fd_request);
    mq_close(mq_s1_request);
    mq_close(mq_s2_request);
    mq_close(mq_fd_response);

    mq_unlink(Req_queue_33);
    mq_unlink(S1_queue_33);
    mq_unlink(S2_queue_33);
    mq_unlink(Rsp_queue_33);
 
    return (0);
}