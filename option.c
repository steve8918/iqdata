/*
 * Routines to analyze command-line option < option.c >
 *
 * コマンドラインオプション解析のためのルーチン
 *
 * Copyright (C) 1996,1997,1999 Taiji Yamada
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef NO_VMT
#include "vmtio.h"
#include "vmtlib.h"
#endif

int scan_option(int argc, char *argv[], char *key, ...)
{
  int i, n;
  char opt[1024], fmt[1024], *ptr;
  va_list argptr;
#ifndef ENABLE_VSSCANF
#define VLIST1(V) V[0]
#define VLIST2(V) V[0],V[1]
#define VLIST3(V) V[0],V[1],V[2]
#define VLIST4(V) V[0],V[1],V[2],V[3]
#define VLIST5(V) V[0],V[1],V[2],V[3],V[4]
#define VLIST6(V) V[0],V[1],V[2],V[3],V[4],V[5]
#define VLIST7(V) V[0],V[1],V[2],V[3],V[4],V[5],V[6]
#define VLIST8(V) V[0],V[1],V[2],V[3],V[4],V[5],V[6],V[7]
#define VLIST9(V) V[0],V[1],V[2],V[3],V[4],V[5],V[6],V[7],V[8]
#define VLIST10(V) V[0],V[1],V[2],V[3],V[4],V[5],V[6],V[7],V[8],V[9]
  int c;
  void **var;
#endif
  
  strcpy(opt, key);
  /*
   * scanf style option and its variations
   */
  if ((ptr=strchr(opt,'%')) != NULL) {
    int idx = 0;
    char *p, buf[1024];

#ifndef NO_VMT
    if ((p=strstr(opt,",..")) != NULL) {
      char *q, *r, *s = ptr;

      /*
       * matrix option such as "-m (%lf,..),..",
       * use like "-m (1,2,3),(4,5,6),(7,8,9)".
       */
      if (((q=strstr(opt,"("))!=NULL)&&((r=strstr(p+1,",.."))!=NULL)) {
        long *nrl, *nrh, *ncl, *nch;
	void ***m = NULL;
	fcctype_t type;
	size_t size;

	/*while (*(--q) == ',') (*nrl)++;
	while (*(--s) == ',') (*ncl)++;*/ /* rejected idea */
        strcpy(fmt,ptr);
        fmt[p-ptr] = '\0';
        while (*(--q) == ' '); *(++q) = '\0';
        for (i=1; i<argc-1; i++)
          if (strcmp(argv[i], opt)==0) {
	    /*if (m && *m) {
	      (matrix_free)(size, *m, *nrl, *ncl);
	      m = NULL;
	    }*/ /* critical idea */
            idx = i;
            strcpy(buf, argv[idx+1]);
	    type = fccode_type(fmt);
	    size = fccode_size(fmt);
            va_start(argptr, key);
	    (matrix_sscanf)(size, buf, fmt, (m=va_arg(argptr,void ***)), *(nrl=va_arg(argptr,long *)), (nrh=va_arg(argptr,long *)), *(ncl=va_arg(argptr,long *)), (nch=va_arg(argptr,long *)));
            va_end(argptr);
          }
	return idx;
      }
      /*
       * vector option such as "-v %lf,..", use like "-v 1,2,3".
       */
      else {
        long *nl, *nh;
        void **v = NULL;
	fcctype_t type;
	size_t size;

	/*while (*(--s) == ',') (*ncl)++;*/ /* rejected idea */
        strcpy(fmt,ptr);
        fmt[p-ptr] = '\0';
        while (*(--s) == ' '); *(++s) = '\0';
        for (i=1; i<argc-1; i++)
          if (strcmp(argv[i], opt)==0) {
	    /*if (v && *v) {
	      (vector_free)(size, *v, *nl);
	      v = NULL;
	    }*/ /* critical idea */
            idx = i;
            strcpy(buf, argv[idx+1]);
	    type = fccode_type(fmt);
	    size = fccode_size(fmt);
	    va_start(argptr, key);
	    (vector_sscanf)(size, buf, fmt, (v=va_arg(argptr,void **)), *(nl=va_arg(argptr,long *)), (nh=va_arg(argptr,long *)));
            va_end(argptr);
          }
	return idx;
      }
    }
#endif
    if ((p=strstr(opt,"\"%s\"")) != NULL) ptr = p;
    strcpy(fmt, ptr);
    *ptr = '\0';
    n = 0; while (*--ptr==' ') { *ptr = '\0'; n++; }
#ifndef ENABLE_VSSCANF
    c = 0;
    ptr = strchr(fmt, '%');
    while (ptr != NULL) {
      c++; if (c >= 10) break;
      ptr = strchr(ptr+1, '%');
    }
    var = (void **)malloc((c+1)*sizeof(void *));
    va_start(argptr, key);
    for (i=0; i<c; i++) var[i] = va_arg(argptr, void *);
    var[i] = NULL;
    va_end(argptr);
#endif
    for (i=1; i<argc; i++)
      if (strcmp(argv[i], opt)==0) {
        idx = i;
        /*
         * double quoted string option such as '-interp \"%s\"',
         * use like '-interp \"ghostview -\"'.
         */
        if ((ptr=strstr(key,"\"%s\"")) != NULL
            && strlen(opt)+n == (int)(ptr-key)) {
          if (idx+1 < argc && argv[idx+1][0] == '\"') {
            strcpy(buf, "");
            strcat(buf, argv[++i]+1);
            strcat(buf, " ");
            while (++i < argc) {
              strcat(buf, argv[i]);
              if ((ptr=strchr(buf,'\"')) != NULL) {
                *ptr = '\0';
                break;
              }
              if (i != argc-1) strcat(buf, " ");
            }
            va_start(argptr, key);
            strcpy(va_arg(argptr,void *), buf);
            va_end(argptr);
          } else {
            va_start(argptr, key);
            strcpy(va_arg(argptr,void *), argv[idx+1]);
            va_end(argptr);
          }
        }
        /*
         * option including some spaces such as "-title %s",
         * use like '-title "Title of this program"'.
         */
        else if (*((ptr=strchr(key,'%'))+1) == 's' && *(ptr+2) == '\0'
                 && strlen(opt)+n == (int)(ptr-key)) {
          va_start(argptr, key);
          strcpy(va_arg(argptr,void *), argv[idx+1]);
          va_end(argptr);
        }
        /*
         * normal scanf style options such "-geometry %dx%d",
         * use like "-geometry 640x480".
         */
        else {
          strcpy(buf, "");
          while (++i < argc) {
            strcat(buf, argv[i]);
            strcat(buf, " ");
          }
#ifdef ENABLE_VSSCANF
          va_start(argptr, key);
          vsscanf(buf, fmt, argptr);
          va_end(argptr);
#else
          switch (c) {
          case 1: sscanf(buf, fmt, VLIST1(var)); break;
          case 2: sscanf(buf, fmt, VLIST2(var)); break;
          case 3: sscanf(buf, fmt, VLIST3(var)); break;
          case 4: sscanf(buf, fmt, VLIST4(var)); break;
          case 5: sscanf(buf, fmt, VLIST5(var)); break;
          case 6: sscanf(buf, fmt, VLIST6(var)); break;
          case 7: sscanf(buf, fmt, VLIST7(var)); break;
          case 8: sscanf(buf, fmt, VLIST8(var)); break;
          case 9: sscanf(buf, fmt, VLIST9(var)); break;
          case 10: sscanf(buf, fmt, VLIST10(var)); break;
          }
#endif
        }
        break;
      }
#ifndef ENABLE_VSSCANF
    free(var);
#undef VLIST1
#undef VLIST2
#undef VLIST3
#undef VLIST4
#undef VLIST5
#undef VLIST6
#undef VLIST7
#undef VLIST8
#undef VLIST9
#undef VLIST10
#endif
    return idx;
  }
  /*
   * flag type option such as "[-+]on" or "[+-]on", use like "-on" or "+on".
   * "[-+]" style gives "-on": !0 and "+on": 0, and
   * "[+-]" style gives "+on": !0 and "-on": 0.
   */
  else if ((ptr=strstr(opt,"[-+]")) != NULL||
           (ptr=strstr(opt,"[+-]")) != NULL) {
    int *p;
    char buf[1024];
    
    va_start(argptr, key);
    p = (int *)va_arg(argptr, int *);
    va_end(argptr);
    strcpy(buf, (*(ptr+1)=='-')?"-":"+");
    strcat(buf, opt+strlen("[-+]"));
    for (i=1; i<argc; i++) {
      if (strcmp(argv[i], buf)==0) {
        *p = !0;
        return i; /* ! */
      }
    }
    strcpy(buf, (*(ptr+1)=='-')?"+":"-");
    strcat(buf, opt+strlen("[-+]"));
    for (i=1; i<argc; i++) {
      if (strcmp(argv[i], buf)==0) {
        *p = 0;
        return i; /* ! */
      }
    }
    return 0;
  }
  /*
   * flag type option such as "-on[-]", use like "-on": !0 or "-on-": 0.
   */
  else if ((ptr=strstr(opt,"[-]")) != NULL) {
    int *p;
    char buf[1024];

    *ptr = '\0';
    va_start(argptr, key);
    p = (int *)va_arg(argptr, int *);
    va_end(argptr);
    strcpy(buf, opt);
    for (i=1; i<argc; i++) {
      if (strcmp(argv[i], buf)==0) {
        *p = !0;
        return i; /* ! */
      }
    }
    strcat(buf, "-");
    for (i=1; i<argc; i++) {
      if (strcmp(argv[i], buf)==0) {
        *p = 0;
        return i; /* ! */
      }
    }
    return 0;
  }
  /*
   * bit-mask type option such as "-plot NO|LINE|DOT|POINT",
   * use like "-plot LINE" or "-plot DOT" or "-plot 'LINE|POINT'".
   */
  else if ((ptr=strstr(opt," ")) != NULL && strstr(ptr,"|") != NULL) {
    unsigned long b, *p;

    va_start(argptr, key);
    p = (unsigned long *)va_arg(argptr, unsigned long *);
    va_end(argptr);
    ptr = strtok(opt, " |");
    for (i=1; i<argc; i++)
      if (strcmp(argv[i], opt)==0) {
	int flag = 0;

	b = 0L;
        ptr = strtok(NULL, " |");
        while (ptr && i+1<argc) {
	  char *s = argv[i+1];

	  do {
	    if (strncmp(s, ptr, strlen(ptr))==0) {
	      *p = flag?(*p|b):b;
	      flag = !0;
	    }
	    s = strstr(s, "|");
	    s = s?(s+1):NULL;
	  } while (s != NULL);
          b = (b == 0L)?1L:(b<<1);
          ptr = strtok(NULL, " |");
        }
	if (flag)
	  return i; /* ! */
      }
  }
  /*
   * enumerate type option such as "-color RED GREEN BLUE",
   * use like "-color RED" or "-color GREEN" or "-color BLUE".
   */
  else if ((ptr=strstr(opt," ")) != NULL) {
    int c, *p;

    va_start(argptr, key);
    p = (int *)va_arg(argptr, int *);
    va_end(argptr);
    c = 0;
    ptr = strtok(opt, " ");
    for (i=1; i<argc; i++)
      if (strcmp(argv[i], opt)==0) {
        ptr = strtok(NULL, " ");
        while (ptr && i+1<argc) {
          if (strcmp(argv[i+1], ptr)==0) {
            *p = c;
            return i; /* ! */
          }
          c++;
          ptr = strtok(NULL, " ");
        }
      }
  }
  /*
   * another style options such as "-help",
   * use like "-help" and 'if (scan_option(argc,argv,"-help")){...}' in C.
   */
  else {
    for (i=1; i<argc; i++)
      if (strcmp(argv[i], key)==0)
        return i;
    return 0;
  }
  return 0;
}
