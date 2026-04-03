#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

int main(int argc, char *argv[]){
    openlog(NULL, 0, LOG_USER);
    if(argc != 3){
        fprintf(stderr, "Wrong number of arguments");
        syslog(LOG_ERR, "Wrong number of arguments");
        return 1;
    }
    syslog(LOG_DEBUG, "Writing %s to file %s", argv[2], argv[1]);
    const char *writefile = argv[1];
    const char *writestr = argv[2];
    FILE *file = fopen(writefile, "w");
    if(file == NULL){
        perror("Perror returned");
        fprintf(stderr, "Error opening file %s\n", writefile);
        syslog(LOG_ERR, "Error opening file %s", writefile);
        return 1; 
    } else {
        fprintf(file, "%s", writestr);
        fclose(file);
    }
    return 0;
}