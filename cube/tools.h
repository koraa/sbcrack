// generic useful stuff for any C++ program

#ifndef _TOOLS_H
#define _TOOLS_H

#ifdef NULL
#undef NULL
#endif
#define NULL 0

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef signed long long int llong;
typedef unsigned long long int ullong;

// moved to it's own .h file
#include "assert.h"

inline void *operator new(size_t size) 
{ 
    void *p = malloc(size);
    if(!p) abort();
    return p;
}
inline void *operator new[](size_t size) 
{
    void *p = malloc(size);
    if(!p) abort();
    return p;
}
inline void operator delete(void *p) { if(p) free(p); }
inline void operator delete[](void *p) { if(p) free(p); } 

inline void *operator new(size_t, void *p) { return p; }
inline void *operator new[](size_t, void *p) { return p; }
inline void operator delete(void *, void *) {}
inline void operator delete[](void *, void *) {}

// moved to it's own .h file
#include "tmath.h"

// moved to it's own .h file
#include "loops.h"

#define DELETEP(p) if(p) { delete   p; p = 0; }
#define DELETEA(p) if(p) { delete[] p; p = 0; }

#ifdef WIN32

#ifndef __GNUC__
#pragma warning (3: 4189)       // local variable is initialized but not referenced
#pragma warning (disable: 4244) // conversion from 'int' to 'float', possible loss of data
#pragma warning (disable: 4267) // conversion from 'size_t' to 'int', possible loss of data
#pragma warning (disable: 4355) // 'this' : used in base member initializer list
#pragma warning (disable: 4996) // 'strncpy' was declared deprecated
#endif

#define strcasecmp _stricmp
#define PATHDIV '\\'

#else
#define __cdecl
#define _vsnprintf vsnprintf
#define PATHDIV '/'
#endif

// moved to string.h
#include "tstring.h"

// also moved to loops.h

// databuf moved to it's own .h
#include "databuf.h"

template<class T>
static inline float heapscore(const T &n) { return n; }

// moved to it's own .h file
#include "sort.h"

// TODO: moved to own .h
#include "vector.h"

static inline uint hthash(const char *key)
{
    uint h = 5381;
    for(int i = 0, k; (k = key[i]); i++) h = ((h<<5)+h)^k;    // bernstein k=33 xor
    return h;
}

static inline bool htcmp(const char *x, const char *y)
{
    return !strcmp(x, y);
}

static inline uint hthash(int key)
{   
    return key;
}

static inline bool htcmp(int x, int y)
{
    return x==y;
}

template<class T> struct hashset
{
    typedef T elem;
    typedef const T const_elem;

    enum { CHUNKSIZE = 64 };

    struct chain { T elem; chain *next; };
    struct chainchunk { chain chains[CHUNKSIZE]; chainchunk *next; };

    int size;
    int numelems;
    chain **chains;

    chainchunk *chunks;
    chain *unused;

    hashset(int size = 1<<10)
      : size(size)
    {
        numelems = 0;
        chunks = NULL;
        unused = NULL;
        chains = new chain *[size];
        loopi(size) chains[i] = NULL;
    }

    ~hashset()
    {
        DELETEA(chains);
        deletechunks();
    }

    chain *insert(uint h)
    {
        if(!unused)
        {
            chainchunk *chunk = new chainchunk;
            chunk->next = chunks;
            chunks = chunk;
            loopi(CHUNKSIZE-1) chunk->chains[i].next = &chunk->chains[i+1];
            chunk->chains[CHUNKSIZE-1].next = unused;
            unused = chunk->chains;
        }
        chain *c = unused;
        unused = unused->next;
        c->next = chains[h];
        chains[h] = c;
        numelems++;
        return c;
    }
     
    #define HTFIND(key, success, fail) \
        uint h = hthash(key)&(this->size-1); \
        for(chain *c = this->chains[h]; c; c = c->next) \
        { \
            if(htcmp(key, c->elem)) return (success); \
        } \
        return (fail);

    template<class K>
    T *access(const K &key)
    {
        HTFIND(key, &c->elem, NULL);
    }

    template<class K, class E>
    T &access(const K &key, const E &elem)
    {
        HTFIND(key, c->elem, insert(h)->elem = elem);
    }

    template<class K>
    T &operator[](const K &key)
    {
        HTFIND(key, c->elem, insert(h)->elem);
    }

    template<class K>
    T &find(const K &key, T &notfound)
    {
        HTFIND(key, c->elem, notfound);
    }

    template<class K>
    const T &find(const K &key, const T &notfound)
    {
        HTFIND(key, c->elem, notfound);
    }

    template<class K>
    bool remove(const K &key)
    {
        uint h = hthash(key)&(size-1);
        for(chain **p = &chains[h], *c = chains[h]; c; p = &c->next, c = c->next)
        {
            if(htcmp(key, c->elem))
            {
                *p = c->next;
                c->elem.~T();
                new (&c->elem) T;
                c->next = unused;
                unused = c;
                numelems--;
                return true;
            }
        }
        return false;
    }

    void deletechunks()
    {
        for(chainchunk *nextchunk; chunks; chunks = nextchunk)
        {
            nextchunk = chunks->next;
            delete chunks;
        }
    }

    void clear()
    {
        if(!numelems) return;
        loopi(size) chains[i] = NULL;
        numelems = 0;
        unused = NULL;
        deletechunks();
    }

    static inline chain *getnext(void *i) { return ((chain *)i)->next; }
    static inline T &getdata(void *i) { return ((chain *)i)->elem; }
};

template<class K, class T> struct hashtableentry
{
    K key;
    T data;

    hashtableentry() {}
    hashtableentry(const K &key, const T &data) : key(key), data(data) {}
};

template<class U, class K, class T>
static inline bool htcmp(const U *x, const hashtableentry<K, T> &y)
{
    return htcmp(x, y.key);
}

template<class U, class K, class T>
static inline bool htcmp(const U &x, const hashtableentry<K, T> &y)
{
    return htcmp(x, y.key);
}

template<class K, class T> struct hashtable : hashset<hashtableentry<K, T> >
{
    typedef hashtableentry<K, T> entry;
    typedef struct hashset<entry>::chain chain;
    typedef K key;
    typedef T value;

    hashtable(int size = 1<<10) : hashset<entry>(size) {}

    entry &insert(const K &key, uint h)
    {
        chain *c = hashset<entry>::insert(h);
        c->elem.key = key;
        return c->elem;
    }
    
    T *access(const K &key)
    {
        HTFIND(key, &c->elem.data, NULL);
    }

    T &access(const K &key, const T &data)
    {
        HTFIND(key, c->elem.data, insert(key, h).data = data);
    }

    T &operator[](const K &key)
    {
        HTFIND(key, c->elem.data, insert(key, h).data);
    }

    T &find(const K &key, T &notfound)
    {
        HTFIND(key, c->elem.data, notfound);
    }

    const T &find(const K &key, const T &notfound)
    {
        HTFIND(key, c->elem.data, notfound);
    }   

    static inline chain *getnext(void *i) { return ((chain *)i)->next; }
    static inline K &getkey(void *i) { return ((chain *)i)->elem.key; }
    static inline T &getdata(void *i) { return ((chain *)i)->elem.data; }
};

#define enumerates(ht,t,e,b)      loopi((ht).size)  for(hashset<t>::chain *enumc = (ht).chains[i]; enumc;) { t &e = enumc->elem; enumc = enumc->next; b; }
#define enumeratekt(ht,k,e,t,f,b) loopi((ht).size)  for(hashtable<k,t>::chain *enumc = (ht).chains[i]; enumc;) { const hashtable<k,t>::key &e = enumc->elem.key; t &f = enumc->elem.data; enumc = enumc->next; b; }
#define enumerate(ht,t,e,b)       loopi((ht).size) for(void *enumc = (ht).chains[i]; enumc;) { t &e = (ht).getdata(enumc); enumc = (ht).getnext(enumc); b; }

struct unionfind
{
    struct ufval
    {
        int rank, next;

        ufval() : rank(0), next(-1) {}
    };
    
    vector<ufval> ufvals;

    int find(int k)
    {
        if(k>=ufvals.length()) return k;
        while(ufvals[k].next>=0) k = ufvals[k].next;
        return k;
    }
    
    int compressfind(int k)
    {
        if(ufvals[k].next<0) return k;
        return ufvals[k].next = compressfind(ufvals[k].next);
    }
    
    void unite (int x, int y)
    {
        while(ufvals.length() <= max(x, y)) ufvals.add();
        x = compressfind(x);
        y = compressfind(y);
        if(x==y) return;
        ufval &xval = ufvals[x], &yval = ufvals[y];
        if(xval.rank < yval.rank) xval.next = y;
        else
        {
            yval.next = x;
            if(xval.rank==yval.rank) yval.rank++;
        }
    }
};

template <class T, int SIZE> struct ringbuf
{
    int index, len;
    T data[SIZE];

    ringbuf() { clear(); }

    void clear()
    {
        index = len = 0;
    }

    bool empty() const { return !len; }

    const int length() const { return len; }

    T &add()
    {
        T &t = data[index];
        index++;
        if(index >= SIZE) index -= SIZE;
        if(len < SIZE) len++;
        return t;
    }

    T &add(const T &e) { return add() = e; }

    T &operator[](int i)
    {
        i += index - len;
        return data[i < 0 ? i + SIZE : i%SIZE];
    }

    const T &operator[](int i) const
    {
        i += index - len;
        return data[i < 0 ? i + SIZE : i%SIZE];
    }
};

template <class T, int SIZE> struct queue
{
    int head, tail, len;
    T data[SIZE];
    
    queue() { clear(); }
    
    void clear() { head = tail = len = 0; }

    int length() const { return len; }
    bool empty() const { return !len; }
    bool full() const { return len == SIZE; }

    T &added() { return data[tail > 0 ? tail-1 : SIZE-1]; }
    T &added(int offset) { return data[tail-offset > 0 ? tail-offset-1 : tail-offset-1 + SIZE]; }
    T &adding() { return data[tail]; }
    T &adding(int offset) { return data[tail+offset >= SIZE ? tail+offset - SIZE : tail+offset]; }
    T &add()
    {
        ASSERT(len < SIZE);    
        T &t = data[tail];
        tail = (tail + 1)%SIZE;
        len++;
        return t;
    }

    T &removing() { return data[head]; }
    T &removing(int offset) { return data[head+offset >= SIZE ? head+offset - SIZE : head+offset]; }
    T &remove()
    {
        ASSERT(len > 0);
        T &t = data[head];
        head = (head + 1)%SIZE;
        len--;
        return t;
    }
};

inline char *newstring(size_t l)                { return new char[l+1]; }
inline char *newstring(const char *s, size_t l) { return copystring(newstring(l), s, l+1); }
inline char *newstring(const char *s)           { return newstring(s, strlen(s));          }
inline char *newstringbuf(const char *s)        { return newstring(s, MAXSTRLEN-1);       }

const int islittleendian = 1;
#ifdef SDL_BYTEORDER
#define endianswap16 SDL_Swap16
#define endianswap32 SDL_Swap32
#define endianswap64 SDL_Swap64
#else
inline ushort endianswap16(ushort n) { return (n<<8) | (n>>8); }
inline uint endianswap32(uint n) { return (n<<24) | (n>>24) | ((n>>8)&0xFF00) | ((n<<8)&0xFF0000); }
inline ullong endianswap64(ullong n) { return endianswap32(uint(n >> 32)) | ((ullong)endianswap32(uint(n)) << 32); }
#endif
template<class T> inline T endianswap(T n) { union { T t; uint i; } conv; conv.t = n; conv.i = endianswap32(conv.i); return conv.t; }
template<> inline ushort endianswap<ushort>(ushort n) { return endianswap16(n); }
template<> inline short endianswap<short>(short n) { return endianswap16(n); }
template<> inline uint endianswap<uint>(uint n) { return endianswap32(n); }
template<> inline int endianswap<int>(int n) { return endianswap32(n); }
template<> inline ullong endianswap<ullong>(ullong n) { return endianswap64(n); }
template<> inline llong endianswap<llong>(llong n) { return endianswap64(n); }
template<> inline double endianswap<double>(double n) { union { double t; uint i; } conv; conv.t = n; conv.i = endianswap64(conv.i); return conv.t; }
template<class T> inline void endianswap(T *buf, int len) { for(T *end = &buf[len]; buf < end; buf++) *buf = endianswap(*buf); }
template<class T> inline T endiansame(T n) { return n; }
template<class T> inline void endiansame(T *buf, int len) {}
#ifdef SDL_BYTEORDER
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define lilswap endiansame
#define bigswap endianswap
#else
#define lilswap endianswap
#define bigswap endiansame
#endif
#else
template<class T> inline T lilswap(T n) { return *(const uchar *)&islittleendian ? n : endianswap(n); }
template<class T> inline void lilswap(T *buf, int len) { if(!*(const uchar *)&islittleendian) endianswap(buf, len); }
template<class T> inline T bigswap(T n) { return *(const uchar *)&islittleendian ? endianswap(n) : n; }
template<class T> inline void bigswap(T *buf, int len) { if(*(const uchar *)&islittleendian) endianswap(buf, len); }
#endif

/* workaround for some C platforms that have these two functions as macros - not used anywhere */
#ifdef getchar
#undef getchar
#endif
#ifdef putchar
#undef putchar
#endif

struct stream
{
#ifdef WIN32
#ifdef __GNUC__
    typedef off64_t offset;
#else
    typedef __int64 offset;
#endif
#else
    typedef off_t offset;
#endif
    
    virtual ~stream() {}
    virtual void close() = 0;
    virtual bool end() = 0;
    virtual offset tell() { return -1; }
    virtual offset rawtell() { return tell(); }
    virtual bool seek(offset pos, int whence = SEEK_SET) { return false; }
    virtual offset size();
    virtual int read(void *buf, int len) { return 0; }
    virtual int write(const void *buf, int len) { return 0; }
    virtual int getchar() { uchar c; return read(&c, 1) == 1 ? c : -1; }
    virtual bool putchar(int n) { uchar c = n; return write(&c, 1) == 1; }
    virtual bool getline(char *str, int len);
    virtual bool putstring(const char *str) { int len = (int)strlen(str); return write(str, len) == len; }
    virtual bool putline(const char *str) { return putstring(str) && putchar('\n'); }
    virtual int printf(const char *fmt, ...);
    virtual uint getcrc() { return 0; }

    template<class T> bool put(T n) { return write(&n, sizeof(n)) == sizeof(n); }
    template<class T> bool putlil(T n) { return put<T>(lilswap(n)); }
    template<class T> bool putbig(T n) { return put<T>(bigswap(n)); }

    template<class T> T get() { T n; return read(&n, sizeof(n)) == sizeof(n) ? n : 0; }
    template<class T> T getlil() { return lilswap(get<T>()); }
    template<class T> T getbig() { return bigswap(get<T>()); }

};

enum
{
    CT_PRINT   = 1<<0,
    CT_SPACE   = 1<<1,
    CT_DIGIT   = 1<<2,
    CT_ALPHA   = 1<<3,
    CT_LOWER   = 1<<4,
    CT_UPPER   = 1<<5,
    CT_UNICODE = 1<<6
};
extern const uchar cubectype[256];
static inline int iscubeprint(uchar c) { return cubectype[c]&CT_PRINT; }
static inline int iscubespace(uchar c) { return cubectype[c]&CT_SPACE; }
static inline int iscubealpha(uchar c) { return cubectype[c]&CT_ALPHA; }
static inline int iscubealnum(uchar c) { return cubectype[c]&(CT_ALPHA|CT_DIGIT); }
static inline int iscubelower(uchar c) { return cubectype[c]&CT_LOWER; }
static inline int iscubeupper(uchar c) { return cubectype[c]&CT_UPPER; }
static inline int cube2uni(uchar c)
{ 
    extern const int cube2unichars[256]; 
    return cube2unichars[c]; 
}
static inline uchar uni2cube(int c)
{
    extern const int uni2cubeoffsets[8];
    extern const uchar uni2cubechars[];
    return uint(c) <= 0x7FF ? uni2cubechars[uni2cubeoffsets[c>>8] + (c&0xFF)] : 0;
}
extern int decodeutf8(uchar *dst, int dstlen, uchar *src, int srclen, int *carry = NULL);
extern int encodeutf8(uchar *dstbuf, int dstlen, uchar *srcbuf, int srclen, int *carry = NULL);

extern char *makerelpath(const char *dir, const char *file, const char *prefix = NULL, const char *cmd = NULL);
extern char *path(char *s);
extern char *path(const char *s, bool copy);
extern const char *parentdir(const char *directory);
extern bool fileexists(const char *path, const char *mode);
extern bool createdir(const char *path);
extern size_t fixpackagedir(char *dir);
extern const char *sethomedir(const char *dir);
extern void addrepositories();
extern const char *addpackagedir(const char *dir);
extern const char *findfile(const char *filename, const char *mode);
extern stream *openrawfile(const char *filename, const char *mode);
extern stream *openzipfile(const char *filename, const char *mode);
extern stream *openfile(const char *filename, const char *mode);
extern stream *opentempfile(const char *filename, const char *mode);
extern stream *opengzfile(const char *filename, const char *mode, stream *file = NULL, int level = Z_BEST_COMPRESSION);
extern stream *openutf8file(const char *filename, const char *mode, stream *file = NULL);
extern char *loadfile(const char *fn, int *size, bool utf8 = true);
extern void sortfiles(vector<char *> &files);
extern bool listdir(const char *dir, bool rel, const char *ext, vector<char *> &files);
extern int listfiles(const char *dir, const char *ext, vector<char *> &files);
extern int listzipfiles(const char *dir, const char *ext, vector<char *> &files);
extern void seedMT(uint seed);
extern uint randomMT(void);

#endif

