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

    //set the maximum number of messages that can be stored in the queue to 10
    attribute.mq_maxmsg = 10;

    //set the maximum size of the message queue to the size of the request message
    attribute.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
     
    
    //open the 3 request queues with their name and the write only property
    mq_fd_request = mq_open(Req_queue_33,O_RDWR | O_CREAT | O_EXCL,0600 ,&attribute);
      
    mq_s1_request = mq_open(S1_queue_33,O_RDWR | O_CREAT | O_EXCL, 0600, &attribute);
   
    mq_s2_request = mq_open(S2_queue_33,O_RDWR | O_CREAT | O_EXCL, 0600, &attribute);
   

    //set the maximum number of messages that can be stored in the queue to 10
    attribute.mq_maxmsg = MQ_MAX_MESSAGES;
    
     //set the maximum size of the message queue to the size of the request message
    attribute.mq_msgsize = sizeof (MQ_RESPONSE_MESSAGE);
    

    //open the response message queue that is read only
    mq_fd_response = mq_open(Rsp_queue_33,O_RDONLY | O_CREAT | O_EXCL,0600,&attribute);

    //TODO
    //  * create the child processes (see process_test() and
    //    message_queue_test())



    c = fork();
    Children_Id[0] = c;

    if (c < 0){
        perror("fork() client failed");
        exit (1);
    }
    else if (c==0) {
        perror("starting client process");
        execlp ("./client",Req_queue_33, (char *)NULL);
        
        perror("execlp() client failed");
    }
    waitpid (c, NULL, 0);

    while (mq_receive (mq_fd_request,(char *)&request, sizeof(request),NULL)> -1) {
        if (request.ServiceID == 1) {
            mq_send(mq_s1_request, (char *)&response, sizeof(response), NULL);
            perror("sending to worker 1 queue");
        } else if (request.ServiceID == 2) {
            mq_send(mq_s2_request, (char *)&response, sizeof(response), NULL);
        }
    }
    for (int i=0; i<N_SERV1; i++) {
        if (c>0) {
            pid_t c = fork();
            Children_Id[i+1] = c;
            if (c < 0){
                perror("fork() w1 failed");
                exit (1);
            }
            if(c==0) {
                execlp("./worker_s1", S1_queue_33, Rsp_queue_33, NULL);
                perror("execlp() worker 1 failed");
                exit(0);
            }
        }
    }
    // for(int i=0; i<N_SERV2; i++) {
    //     if (c>0) {
    //         pid_t c = fork();
//             Children_Id[i+1+N_SERV1] = c;

    //         if (c < 0){
    //             perror("fork() w2 failed");
    //             exit (1);
    //         }
    //         if (c==0) {
    //             execlp("worker_s2", "worker_s2", &S2_queue_33, &Rsp_queue_33, NULL);
    //             perror("execlp() worker 2 failed");
    //         }
    //     }
    // }
    // sleep(3);
    


    for(int i=0; i<1+N_SERV1+N_SERV2; i++){
        printf("id from child %u is: %u\n", i,Children_Id[i]);
    }
    
    
    
    int i = 0;
    if (mq_receive(Rsp_queue_33, (char *)&response, sizeof(response), 0) > -1) {
        printf(i + "->" + response.result);
        i++;
    }
    //  * read requests from the Req queue and transfer them to services
    //  * read answers from services in the Rep queue and print them
    //  * wait until the clients have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues
    // contain your goup number (to ensure uniqueness during testing)
    
    return (0);
}
