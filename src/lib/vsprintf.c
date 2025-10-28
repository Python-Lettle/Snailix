/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 */

#include <snailix/stdarg.h>
#include <snailix/string.h>
#include <snailix/assert.h>

#define ZEROPAD 0x01 // Pad with zeros
#define SIGN 0x02    // unsigned/signed long
#define PLUS 0x04    // Show plus
#define SPACE 0x08   // Space if plus
#define LEFT 0x10    // Left justified
#define SPECIAL 0x20 // 0x
#define SMALL 0x40   // Use lowercase letters
#define DOUBLE 0x80  // Floating point number

#define is_digit(c) ((c) >= '0' && (c) <= '9')

// Convert a string of digits to an integer and advance the pointer
static int skip_atoi(const char **s)
{
    int i = 0;
    while (is_digit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

// Convert integer to string in specified base
// str - output string pointer
// num - integer
// base - base of the number system
// size - string length
// precision - number length (precision)
// flags - options
static char *number(char *str, u32 *num, int base, int size, int precision, int flags)
{
    char pad, sign, tmp[36];
    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;
    int index;
    char *ptr = str;

    // If flags indicate lowercase letters, define lowercase letter set
    if (flags & SMALL)
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";

    // If flags indicate left alignment, clear zero padding flag
    if (flags & LEFT)
        flags &= ~ZEROPAD;

    // If base is less than 2 or greater than 36, exit
    // This program can only handle bases between 2-32
    if (base < 2 || base > 36)
        return 0;

    // If flags indicate zero padding, set pad to '0', otherwise to space
    pad = (flags & ZEROPAD) ? '0' : ' ';

    // If flags indicate signed number and num is negative, set sign to '-'
    if (flags & DOUBLE && (*(double *)(num)) < 0)
    {
        sign = '-';
        *(double *)(num) = -(*(double *)(num));
    }
    else if (flags & SIGN && !(flags & DOUBLE) && ((int)(*num)) < 0)
    {
        sign = '-';
        (*num) = -(int)(*num);
    }
    else
        // Otherwise, if flags indicate plus, set sign to '+', or space if SPACE flag, or 0
        sign = (flags & PLUS) ? '+' : ((flags & SPACE) ? ' ' : 0);

    // If signed, reduce width by 1
    if (sign)
        size--;

    // If flags indicate special conversion
    if (flags & SPECIAL)
    {
        // For hexadecimal, reduce width by 2 (for 0x)
        if (base == 16)
            size -= 2;
        // For octal, reduce width by 1 (for leading zero)
        else if (base == 8)
            size--;
    }

    i = 0;

    // If num is 0, set tmp to '0', otherwise convert num to characters based on base
    if (flags & DOUBLE)
    {
        u32 ival = (u32)(*(double *)num);
        u32 fval = (u32)(((*(double *)num) - ival) * 1000000);

        int mantissa = 6;
        while (mantissa --)
        {
            index = (fval) % base;
            (fval) /= base;
            tmp[i++] = digits[index];
        }

        tmp[i++] = '.';

        do
        {
            index = (ival) % base;
            (ival) /= base;
            tmp[i++] = digits[index];
        } while (ival);
    }
    else if ((*num) == 0)
    {
        tmp[i++] = '0';
    }
    else
    {
        while ((*num) != 0)
        {
            index = (*num) % base;
            (*num) /= base;
            tmp[i++] = digits[index];
        }
    }

    // If number of digits > precision, extend precision to number of digits
    if (i > precision)
        precision = i;

    // Subtract number of digits from width
    size -= precision;

    // Start forming the final result string

    // If flags don't have ZEROPAD and LEFT, fill remaining width with spaces
    if (!(flags & (ZEROPAD + LEFT)))
        while (size-- > 0)
            *str++ = ' ';

    // If signed, add sign
    if (sign)
        *str++ = sign;

    // If flags indicate special conversion
    if (flags & SPECIAL)
    {
        // For octal, add '0' at the beginning
        if (base == 8)
            *str++ = '0';
        // For hexadecimal, add '0x'
        else if (base == 16)
        {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    // If flags don't have LEFT, fill remaining width with pad character
    if (!(flags & LEFT))
        while (size-- > 0)
            *str++ = pad;

    // i now contains the number of digits

    // If number of digits < precision, fill with '0's
    while (i < precision--)
        *str++ = '0';

    // Add the converted digits
    while (i-- > 0)
        *str++ = tmp[i];

    // If width > 0, fill remaining width with spaces (for left alignment)
    while (size-- > 0)
        *str++ = ' ';
    return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    int len;
    int i;

    // String buffer for conversion
    char *str;
    char *s;
    int *ip;

    // Flags for number() function
    int flags;

    int field_width; // Output field width
    int precision;   // Minimum number of digits for integers; maximum number of characters for strings
    int qualifier;   // 'h', 'l' or 'L' for integer fields
    u32 num;
    u8 *ptr;

    // Point str to buf and scan format string
    for (str = buf; *fmt; ++fmt)
    {
        // Format conversion strings start with '%'
        // Scan fmt for '%' to find start of format conversion
        // Non-format characters are copied to str
        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }

        // Get flags from format string
        flags = 0;
    repeat:
        // Skip first '%'
        ++fmt;
        switch (*fmt)
        {
        // Left adjustment
        case '-':
            flags |= LEFT;
            goto repeat;
        // Show plus
        case '+':
            flags |= PLUS;
            goto repeat;
        // Show space
        case ' ':
            flags |= SPACE;
            goto repeat;
        // Special conversion
        case '#':
            flags |= SPECIAL;
            goto repeat;
        // Pad with zeros
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        // Get field width
        field_width = -1;

        // If width is a number, get it directly
        if (is_digit(*fmt))
            field_width = skip_atoi(&fmt);

        // If width is '*', get width from next argument
        else if (*fmt == '*')
        {
            ++fmt;
            field_width = va_arg(args, int);

            // If width is negative, it indicates left alignment
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        // Get precision
        precision = -1;

        // Precision starts with '.'
        if (*fmt == '.')
        {
            ++fmt;
            // If precision is a number, get it directly
            if (is_digit(*fmt))
                precision = skip_atoi(&fmt);

            // If precision is '*', get precision from next argument
            else if (*fmt == '*')
            {
                precision = va_arg(args, int);
            }
            // If precision is negative, set to 0
            if (precision < 0)
                precision = 0;
        }

        // Get length modifier
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
        {
            qualifier = *fmt;
            ++fmt;
        }

        // Handle conversion specifier
        switch (*fmt)
        {

        // Character
        case 'c':
            if (!(flags & LEFT))
                while (--field_width > 0)
                    *str++ = ' ';
            *str++ = (unsigned char)va_arg(args, int);
            while (--field_width > 0)
                *str++ = ' ';
            break;

        // String
        case 's':
            s = va_arg(args, char *);
            len = strlen(s);
            if (precision < 0)
                precision = len;
            else if (len > precision)
                len = precision;

            if (!(flags & LEFT))
                while (len < field_width--)
                    *str++ = ' ';
            for (i = 0; i < len; ++i)
                *str++ = *s++;
            while (len < field_width--)
                *str++ = ' ';
            break;

        // Octal
        case 'o':
            num = va_arg(args, unsigned long);
            str = number(str, &num, 8, field_width, precision, flags);
            break;

        // Pointer
        case 'p':
            if (field_width == -1)
            {
                field_width = 8;
                flags |= ZEROPAD;
            }
            num = va_arg(args, unsigned long);
            str = number(str, &num, 16, field_width, precision, flags);
            break;

        // Hexadecimal
        case 'x':
            flags |= SMALL;
        case 'X':
            num = va_arg(args, unsigned long);
            str = number(str, &num, 16, field_width, precision, flags);
            break;

        // Integer
        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            num = va_arg(args, unsigned long);
            str = number(str, &num, 10, field_width, precision, flags);
            break;

        // Number of characters written so far
        case 'n':
            ip = va_arg(args, int *);
            *ip = (str - buf);
            break;
        case 'f':
            flags |= SIGN;
            flags |= DOUBLE;
            double dnum = va_arg(args, double);
            str = number(str, (u32 *)&dnum, 10, field_width, precision, flags);
            break;
        case 'b': // binary
            num = va_arg(args, unsigned long);
            str = number(str, &num, 2, field_width, precision, flags);
            break;
        case 'm': // mac address
            flags |= SMALL | ZEROPAD;
            ptr = va_arg(args, char *);
            for (size_t t = 0; t < 6; t++, ptr++)
            {
                int num = *ptr;
                str = number(str, &num, 16, 2, precision, flags);
                *str = ':';
                str++;
            }
            str--;
            break;
        case 'r': // ip address
            flags |= SMALL;
            ptr = va_arg(args, u8 *);
            for (size_t t = 0; t < 4; t++, ptr++)
            {
                int num = *ptr;
                str = number(str, &num, 10, field_width, precision, flags);
                *str = '.';
                str++;
            }
            str--;
            break;
        default:
            if (*fmt != '%')
                *str++ = '%';
            if (*fmt)
                *str++ = *fmt;
            else
                --fmt;
            break;
        }
    }
    *str = '\0';

    i = str - buf;
    assert(i < 1024);
    return i;
}

// Format output string to buf
int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}
