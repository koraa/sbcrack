#include "crypto.cpp"
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
// CRYPTO

void c_genprivkey(const char *seed, vector<char> &privstr, vector<char> &pubstr) {
    tiger::hashval hash;
    tiger::hash((const uchar *)seed, (int)strlen(seed), hash);
    bigint<8*sizeof(hash.bytes)/BI_DIGIT_BITS> privkey;
    memcpy(privkey.digits, hash.bytes, sizeof(privkey.digits));
    privkey.len = 8*sizeof(hash.bytes)/BI_DIGIT_BITS;
    privkey.shrink();
    privkey.printdigits(privstr);
    privstr.add('\0');

    ecjacobian c(ecjacobian::base);
    c.mul(privkey);
    c.normalize();
    c.print(pubstr);
    pubstr.add('\0');
}

//////////////////////////////////////////
// CRACK

void genauthkey(const char *secret) {
    if(!secret[0]) { 
        printf("you must specify a secret password");
        return;
    }
    
    vector<char> privkey, pubkey;
    c_genprivkey(secret, privkey, pubkey);
    
    printf("private key: %s\n", privkey.getbuf());
    printf("public key: %s\n", pubkey.getbuf());
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
    const char* pass = argv[1];
    int  times = 1;
    if (argc > 1)
      times = atoi(argv[2]);

    genauthkey(pass);
    //printf("Seconds needed to generate %i: %Lf\n", times, measure(times, pass));
    return 0;
}
