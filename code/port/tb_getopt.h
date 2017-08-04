
#if _HPUX_SOURCE || defined(sparc) && defined(__svr4__)
extern "C" {
  extern int getopt(int, char* const [], const char *);
  extern char* optarg; 
}
#endif // _HPUX_SOURCE

#if defined(__linux__) || defined(__APPLE_CC__)
  #include <unistd.h>
#endif

#ifdef _MSC_VER
  extern "C" {
    extern int getopt(int, char**, char*);
    extern char* optarg;
  }
#endif // _MSC_VER

extern "C" int optind; 

