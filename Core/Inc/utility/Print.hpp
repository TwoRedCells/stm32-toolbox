/**
 * \file       utility/print.hpp
 * \class      Print
 * \brief      Base class that provides a minimalist printf.
 */

#ifndef INC_PRINT_HPP_
#define INC_PRINT_HPP_


#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include "hal/hal.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2


static const unsigned long dv[] = {
//  4294967296      // 32 bit unsigned max
   1000000000,     // +0
    100000000,     // +1
     10000000,     // +2
      1000000,     // +3
       100000,     // +4
//       65535      // 16 bit unsigned max
        10000,     // +5
         1000,     // +6
          100,     // +7
           10,     // +8
            1,     // +9
};

class Print
{
public:
    void xtoa(unsigned long x, const unsigned long *dp)
    {
       char c;
       unsigned long d;
       if(x)
       {
           while(x < *dp) ++dp;
           do
           {
               d = *dp++;
               c = '0';
               while(x >= d) ++c, x -= d;
               write(c);
           } while(!(d & 1));
       } else
           write('0');
    }

    void puth(unsigned n)
    {
       static const char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
       write(hex[n & 15]);
    }

    void printf(const char *format, ...)
    {
       va_list a;
       va_start(a, format);

       int i, dec; long n; double f;

       while(char c = *format++)
       {
           if(c == '%')
           {
               switch(c = *format++)
               {
                   case 's':                       // String
                       write(va_arg(a, char*));
                       break;
                   case 'c':                       // Char
                       write(va_arg(a, char));
                       break;
                   case 'i':                       // 16 bit Integer
                   case 'u':                       // 16 bit Unsigned
                       i = va_arg(a, int);
                       if(c == 'i' && i < 0) i = -i, write('-');
                       xtoa((unsigned)i, dv + 5);
                       break;
                   case 'l':                       // 32 bit Long
                   case 'n':                       // 32 bit uNsigned loNg
                       n = va_arg(a, long);
                       if(c == 'l' && n < 0) n = -n, write('-');
                       xtoa((unsigned long)n, dv);
                       break;
                   case 'x':                       // 16 bit heXadecimal
                       i = va_arg(a, int);
                       puth(i >> 12);
                       puth(i >> 8);
                       puth(i >> 4);
                       puth(i);
                       break;
                   case '.':						// float
                	   dec = *format++ - 0x30;
                	   f = va_arg(a, double);
                       if(f < 0) f = -f, write('-');
                	   print((int)f, DEC);
                	   write('.');
                	   print((f - (int)f) * pow(10, dec), DEC);
                	   format++;
                	   break;
                   case 0: return;
                   default: goto bad_fmt;
               }
           } else
    bad_fmt:    write(c);
       }
       va_end(a);
    }

    void print(const char* s)
    {
        write((char*)s);
    }

    void print(char* s)
    {
        write(s);
    }

    void print(unsigned int c, uint8_t base=HEX)
    {
        const char* format = base == HEX ? "%x" : "%u";
        printf((char*)format, c);
    }

    virtual void write(uint8_t b) = 0;

    void write(const uint8_t *buffer, size_t size)
    {
        for (int i=0; i<size; i++)
            write(buffer[i]);
    }

    void write(const char *s)
    {
        const char *p = s;
        while(*p)
        {
            write((uint8_t)(*p));
            p++;
        }
    }

};




#endif /* INC_PRINT_HPP_ */
