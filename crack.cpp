#include "cube.h"
#include <stdio.h>
#include <time.h>

extern void genprivkey(const char *seed, vector<char> &privstr, vector<char> &pubstr);

void genauthkey(const char *secret) {
    if(!secret[0]) { 
        printf("you must specify a secret password");
        return;
    }
    
    vector<char> privkey, pubkey;
    genprivkey(secret, privkey, pubkey);
    
    //printf("private key: %s\n", privkey.getbuf());
    //printf("public key: %s\n", pubkey.getbuf());
}

double measure(int times, const char* pass) {
    time_t t0, t1, diff;
    time(&t0);

    for (int i = 0; i < times; i++)
        genauthkey(pass); 

    time(&t1);
    return difftime(t1, t0);
}

int main(const int argc, const char** argv){ 
    const char* pass = argv[0];
    int  times = atoi(argv[1]);

    printf("Seconds needed to generate %i: %.2lf", times, measure(times, pass));
    return 0;
}
