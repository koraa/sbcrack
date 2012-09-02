#include "cube.h"
#include <stdio.h>

extern void genprivkey(const char *seed, vector<char> &privstr, vector<char> &pubstr);

void genauthkey(const char *secret) {
    if(!secret[0]) { 
        printf("you must specify a secret password");
        return;
    }
    
    vector<char> privkey, pubkey;
    genprivkey(secret, privkey, pubkey);
    
    printf("private key: %s\n", privkey.getbuf());
    printf("public key: %s\n", pubkey.getbuf());
}

int main(const int argc, const char** argv) {
    genauthkey(argv[0]); 
    return 0;
}
