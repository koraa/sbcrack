#include "cube.h"
#include <stdio.h>
//#include <time.h>
#include <sys/time.h>

/////////////////////////////////////////
// EXTERN

extern void genprivkey(const char *seed, vector<char> &privstr, vector<char> &pubstr);

//////////////////////////////////////////
// UTIL

#define milli(x)      (x / 1000)
#define micro(x) milli(x / 1000)
#define nano(x)  micro(x / 1000)

typedef long double tfloat;
#define tfloat(x) ((tfloat)x)

tfloat nanotime() {
    timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return tfloat(t.tv_sec) + nano(tfloat(t.tv_nsec));
}

//////////////////////////////////////////
// KEYS

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

tfloat measure(int times, const char* pass) {
    tfloat t0, t1;

    t0 = nanotime();    
    for (int i = 0; i < times; i++)
        genauthkey(pass); 
    t1 = nanotime();

    return t1 - t0;
}

int main(const int argc, const char** argv){ 
    const char* pass = argv[0];
    int  times = atoi(argv[1]);

    printf("Seconds needed to generate %i: %Lf\n", times, measure(times, pass));
    return 0;
}
