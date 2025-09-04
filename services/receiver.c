#include "common.h"
#include "../helper.h"

void* run_receiver(void* arg) {
    printf("Receiver iniciado.\n");

    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    
    Package pkg;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
    while(1)
    {
        fflush(stdout);

        if ((recv_len = recvfrom(s, &pkg, sizeof(pkg), 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }

        printf("Pacote recebido de %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Conteúdo recebido: %s\n", pkg.buffer);
        
        //addToInboundQueue(*(Package*)buf, ntohs(si_other.sin_port), inet_ntoa(si_other.sin_addr));
    
    }
 
    close(s);
    return NULL;
}