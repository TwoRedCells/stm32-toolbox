#include <stdio.h>
#include <stdint.h>
#include <memory.h>

#define U8 (const uint8_t*)
//#define DEBUG_CHARACTERS


class Match
{
    friend class Regex;
    
public:
    size_t get_length(void)
    {
        return length;
    }
    
    void copy(uint8_t* destination)
    {
        memcpy(destination, match, length);
        destination[length] = 0;
    }
    
private:
    size_t length;
    const uint8_t* match;
};

class MatchCollection
{
public:
    size_t length;
    Match matches[10];
};

class CharacterClass
{
    friend class Regex;
 
 public:
    CharacterClass(const uint8_t* matches)
    {
        this->start = matches+1;
        this->end = last(matches, 0)-1;
//        printf("[%d] %s\r\n", end-start, this->start);
    }
    
    bool in(uint8_t test)
    {
//        printf("[%c in %s]\r\n", test, start);
        for (const uint8_t* p=start; p<end; p++)
            if (*p == test)
                return true;
        return false;
    }
    
private:
    CharacterClass(void)
    {
    }
    /**
     * Returns a pointer to the last character in a set.
     * @param   values The haystack to search.
     * @param   terminators Characters that define the end of the set.
     * @param   len The length of the haystack (safe) or -1 to use NUL termination (unsafe).
     * @returns Pointer to the terminating character in the set.
     */
    static const uint8_t* last(const uint8_t* values, uint8_t terminator, int len=-1)
    {
        for (const uint8_t* v=values; len==-1 || v-values <= len; v++)
            if (*v == terminator)
                return v;
        return nullptr;
    }
    
    const uint8_t* start;    
    const uint8_t* end;    
};

class Regex
{
public:

    Regex(const uint8_t* expression, bool case_insensitive=false)
    {
        this->expression = expression;
        this->case_insensitive = case_insensitive;
    }
    
    Regex(const char* expression, bool case_insensitive=false)
    {
        this->expression = (uint8_t*) expression;
        this->case_insensitive = case_insensitive;
    }
    
    
    /**
     * Evaluates the provided string against the regular expression.
     * @param   test The string to evaluate.
     * @length  length The length of the string (safe), or -1 (unsafe).
     * @returns A MatchCollection object containing the matches, if any.
     */
    MatchCollection match(const uint8_t test[], int length=-1)
    {
        const uint8_t* p_t = test;  // Pointer to the current character in the test string.
        const uint8_t* p_e = expression;  // Pointer to the current character in the regex.
        const uint8_t* p_ma = nullptr;  // Pointer to the first character in the current match.
        CharacterClass* classing = nullptr;  // Pointer to an active character class.
        bool strict_start = false;  // Must match from beginning.
        MatchCollection matches;
        
        while ((*p_t != 0 && length == -1) || (p_t-test < length))
        {
//            printf("%c", *p_e);
//            printf(".");
            // Escaped classes.  e.g. \d
            if (*p_e == '\\')
            {
                p_e++;
                if (*p_e == 'd')
                    classing = &class_numeric;
                else if (*p_e == 'w')
                    classing = &class_alpha_lower;
                else if (*p_e == 'W')
                    classing = &class_alpha_upper;
                else if (*p_e == 's')
                    classing = &class_whitespace;
            }
            
            // Embedded classes.  e.g. [abc]
            else if (*p_e == '[')
            {
                embedded.start = p_e + 1;
//                embedded.end = p_e + 2;
                embedded.end = p_e = CharacterClass::last(p_e, ']');
                classing = &embedded;
            }
            else if (*p_e == ']')
            {
                classing = nullptr;
            }
            
            // Strict start.
            else if (*p_e == '^')
            {
                strict_start = true;
                p_e++;
            }
            
#ifdef DEBUG_CHARACTERS
            printf("%c/%c ", *p_t, *p_e);
#endif

            // Now check for matches.
            // Class evaluation.
            if (classing && classing->in(*p_t)  // Character is in the current class.
                || !classing && *p_t == *p_e  // Exact character match.
                || !classing && (case_insensitive && ((*p_t < 'a' && *p_t+0x20 == *p_e) || (*p_t > 'Z' && *p_t-0x20 == *p_e)))  // Case insensitive match.
                || !classing && *p_e == '.'  // Wildcard match.
            )
            {
//               printf("=");
                p_e++;
                if (p_ma == nullptr)
                    p_ma = p_t;
                classing = nullptr;
            }

            // Enforce strict start.
            else if (strict_start)
            {
                break;
            }
            
            // No match.
            else
            {
                if (p_ma != nullptr)
                    p_t = p_ma;
                p_e = expression;
                p_ma = nullptr;
            }
            
            // Complete match.
            if (*p_e == 0)
            {
                matches.length++;
                matches.matches[matches.length].match = p_ma;
                matches.matches[matches.length].length = p_t - p_ma + 1;
                uint8_t buf[100];
                matches.matches[matches.length].copy(buf);
                printf("Match (%d, %d, [%s])\r\n", p_ma-test, p_t-test, buf);
 
            }
            
            p_t++;
        }
        
        return matches;
    }
    
    
    /**
     * Determines if a is in B.
     * @param   a The needle.
     * @param   B The haystack.
     * @param   l The length of the haystack (safe) or -1 to use NUL termination (unsafe).
     * @returns True if a is in B, otherwise false.
     */
    static bool in(uint8_t a, const uint8_t B[], int l=-1)
    {
        for (const uint8_t* p=B; l==-1 || p-B <= l; p++)
        {
            if (a == *p)  // Character match.
                return  true;
            if (*p == 0)  // NUL terminator found.
                break;
        }
        return false;  // No matches.
    }
    
    

private:
    static inline CharacterClass class_numeric = CharacterClass(U8"[0123456789]");  // \d
    static inline CharacterClass class_alpha_lower = CharacterClass(U8"[abcdefghijklmnopqrstuvwxyz]");  // \w
    static inline CharacterClass class_alpha_upper = CharacterClass(U8"[ABCDEFGHIJKLMNOPQRSTUVWXYZ]");  // \W
    static inline CharacterClass class_alpha = CharacterClass(U8"[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]"); // \A
    static inline CharacterClass class_whitespace = CharacterClass(U8"[\t\r\n ]");  // \s
    CharacterClass embedded;
    const uint8_t* set_literals = U8"()[].+\\?";
    const uint8_t* set_classes = U8"dwbs";

    const uint8_t* expression;
    bool case_insensitive = false;
};


int main()
{
    MatchCollection matches;
    printf("Starting.\r\n");
    uint8_t test[] = "Now is the time for all good (goons) to come to the aid of their good country. My Phone number is 519-760-2914.";
    
    printf("* in test pass : %d\r\n", Regex::in('d', U8"abcdefg"));
    printf("* in test fail : %d\r\n", Regex::in('x', U8"abcdefg"));

    
    printf("* strict start pass\r\n");
    Regex regex1("^Now");
    matches = regex1.match(test);
    
    printf("* strict start fail\r\n");
    Regex regex2("^is");
    matches = regex2.match(test);

    printf("* exact match pass\r\n");
    Regex regex3("good");
    matches = regex3.match(test);

    printf("* dot subtitution\r\n");
    Regex regex4("..oo");
    matches = regex4.match(test);

    printf("* escaped literal\r\n");
    Regex regex5("\\(go");
    matches = regex5.match(test);

    printf("* whitespace\r\n");
    Regex regex6("\\sa");
    matches = regex6.match(test);

    printf("* embedded class\r\n");
    Regex regex7("[())]");
    matches = regex7.match(test);

    printf("* phone\r\n");
    Regex regex8("\\d\\d\\d-\\d\\d\\d-\\d\\d\\d\\d");
    matches = regex8.match(test);

    return 0;
}
