#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#define PORT "9000"
#define BUFFER_SIZE 1024

static volatile sig_atomic_t caught_signal = 0;

static void signal_handler(int signum)
{
    int errno_saved = errno;
    if (signum == SIGINT || signum == SIGTERM)
    {
        caught_signal = 1;
    }
    errno = errno_saved;
}

static void client_handler(int cfd)
{
    char buf[BUFFER_SIZE];
    ssize_t nread;
    FILE *fp = fopen("/var/tmp/aesdsocketdata", "a+");
    if (fp == NULL)
    {
        syslog(LOG_ERR, "Could not open file");
        return;
    }

    while ((nread = recv(cfd, buf, BUFFER_SIZE, 0)) > 0)
    {
        fwrite(buf, sizeof(char), nread, fp);
        if (buf[nread - 1] == '\n')
            break;
    }

    // reset file pointer to beginning
    fseek(fp, 0, SEEK_SET);

    // send buffers by buffers
    while (fgets(buf, BUFFER_SIZE, fp) != NULL)
    {
        send(cfd, buf, strlen(buf), 0);
    }

    fclose(fp);
}

int main(int argc, char *argv[])
{
    struct sigaction new_action;
    memset(&new_action, 0, sizeof(new_action));
    new_action.sa_handler = signal_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    if (sigaction(SIGINT, &new_action, NULL) != 0)
    {
        fprintf(stderr, "Error %d (%s) registering for SIGTERM", errno, strerror(errno));
        exit(-1);
    }
    if (sigaction(SIGTERM, &new_action, NULL) != 0)
    {
        fprintf(stderr, "Error %d (%s) registering for SIGTERM", errno, strerror(errno));
        exit(-1);
    }

    openlog(NULL, 0, LOG_USER);

    bool daemonize = false;
    if (argc > 1 && strcmp(argv[1], "-d") == 0)
    {
        daemonize = true;
    }

    // mainly from man getaddrinfo example
    int sfd, s;
    socklen_t peer_addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct sockaddr_storage peer_addr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(NULL, PORT, &hints, &result);
    if (s != 0)
    {
        syslog(LOG_ERR, "getaddrinfo failed: %s", gai_strerror(s));
        exit(-1);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;
        int optval = 1;
        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(sfd);
    }

    freeaddrinfo(result);

    if (rp == NULL)
    {
        syslog(LOG_ERR, "Could not bind");
        exit(-1);
    }

    if(daemonize)
    {
        if (daemon(0, 0) == -1)
        {
            syslog(LOG_ERR, "Could not daemonize");
            exit(-1);
        }
    }

    if (listen(sfd, 5) == -1)
    {
        syslog(LOG_ERR, "Could not listen");
        exit(-1);
    }

    // inf loop for accepting and handling connections
    while (!caught_signal)
    {
        char host[NI_MAXHOST], service[NI_MAXSERV];
        peer_addrlen = sizeof(peer_addr);

        int cfd = accept(sfd, (struct sockaddr *)&peer_addr, &peer_addrlen);
        if (cfd == -1)
        {
            syslog(LOG_ERR, "accept failed: %s", strerror(errno));
            continue;
        }

        s = getnameinfo((struct sockaddr *)&peer_addr,
                        peer_addrlen, host, NI_MAXHOST,
                        service, NI_MAXSERV, NI_NUMERICSERV);

        if (s == 0)
        {
            syslog(LOG_DEBUG, "Accepted connection from %s:%s", host, service);
        }

        // handle client data here
        client_handler(cfd);

        if (s == 0)
        {
            syslog(LOG_DEBUG, "Closed connection from %s", host);
        }

        close(cfd);
    }

    if (caught_signal)
    {
        syslog(LOG_DEBUG, "Caught signal, exiting");
    }

    // cleanup
    close(sfd);
    unlink("/var/tmp/aesdsocketdata");
    closelog();

    return 0;
}