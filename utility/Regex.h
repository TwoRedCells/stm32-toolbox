#include <stdio.h>
#include <stdint.h>
#include <memory.h>

#define U8 (const uint8_t*)
#define DEBUG_CHARACTERS
//#define SAFE_BUFFERS

/**
 * A regular expression resulting match.
 */
class Match
{
    friend class Regex;
    friend class MatchCollection;
    
public:
    /**
     * Gets the number of characters in the match.
     * @returns The match length.
     */
    size_t get_length(void)
    {
        return length;
    }
    
    /**
     * Copies the contents of the match to a provided buffer.
     * @param   destination The destination buffer.
     * @param   l The size of the destination buffer (empty/0 for skipping safe checks).
     */
#ifdef SAFE_BUFFERS
    void copy(uint8_t* destination, uint16_t l)
#else
    void copy(uint8_t* destination, uint16_t l=0)
#endif
    {
 //       memcpy(destination, value, !l ? length : length<l?length:l);
        memcpy(destination, value, length);
        destination[length] = 0;
    }
    
private:
    size_t length = 0;
    uint8_t* value;
};


/**
 * A collection of regular expression matches.
 */
class MatchCollection
{
    friend class Regex;
public:
    /**
     * Creates an empty collection instance.
     */
    MatchCollection(void)
    {
    }
    
    
    /**
     * Gets thhe number of matches in the collection.
     * @returns The size of the collection.
     */
    size_t get_count(void)
    {
        return count;
    }
    
    /**
     * Returns the match with the specified index.
     * @param index The index.
     * @returns A reference to the Match object.
     */
    Match& operator [] (size_t index)
    {
        return matches[index];
    }
    
private:
    /**
     * Adds the specified match to the collection.
     * @param   match The match to add.
     * @returns The new size of the collection.
     */
    size_t add(Match& match)
    {
        matches[count].length = match.length;
        matches[count].value = match.value;
        return ++count;
    }


    size_t count = 0;
    Match matches[250];
};


/**
 * A class of characters to include (or exclude) when evaluating a regular expression.
 * @note This class is used internally to Regex only.
 */
class CharacterClass
{
    friend class Regex;
 
private:
    /**
     * Constructs an empty instance.
     */
    CharacterClass(void)
    {
    }

    /**
     * Constructs a CharacterClass instance.
     * @param matches A list of characters surrounded by square brackets to include in the class. A caret (^) before the first character indicates that the meaning of the class should be negative (exclusive).
     */
    CharacterClass(const uint8_t* matches)
    {
        set(matches);
    }
    
    /**
     * Evaluates whether the test character is included (or excluded in the case of inverse operatiob) in the class.
     * @param   test The test character.
     * @Returns True on a match; otherwise false.
     */
    bool in(uint8_t test)
    {
        if (inverted)
        {
            for (const uint8_t* p=start; p<end; p++)
                if (*p == test)
                    return false;
            return true;
        }
        else
        {
            for (const uint8_t* p=start; p<end; p++)
                if (*p == test)
                    return true;
            return false;            
        }
    }
    
    /**
     * Sets the internal match pattern.
     * @param   matches The pattern.
     */
    void set(const uint8_t* matches)
    {
        start = matches + 1;
        end = last(matches, ']');
        if (*start == '^')
        {
            inverted = true;
            start++;
        }
    }

    /**
     * Returns a pointer to the last character in a set.
     * @param   values The haystack to search.
     * @param   terminators Characters that define the end of the set.
     * @param   len The length of the haystack (safe).
     * @returns Pointer to the terminating character in the set.
     */
#ifdef SAFE_BUFFERS
    static const uint8_t* last(const uint8_t* values, uint8_t terminator, uint16_t len)
#else
    static const uint8_t* last(const uint8_t* values, uint8_t terminator, uint16_t len=0)
#endif
    {
        for (const uint8_t* v=values; !len || v-values <= len; v++)
            if (*v == terminator)
                return v;
        return nullptr;
    }
    
    const uint8_t* start;    
    const uint8_t* end;  
    bool inverted = false;
};

class Regex
{
public:
    /**
     * Constructs a Regex object.
     * @param   expression The pattern to evaluate against.
     * @param   case_insensitive True if evaluation should be done ignoring case. Defaults to false.
     */
    Regex(const uint8_t* expression, bool case_insensitive=false)
    {
        this->expression = expression;
        this->case_insensitive = case_insensitive;
    }
    
    /**
     * Constructs a Regex object.
     * @param   expression The pattern to evaluate against.
     * @param   case_insensitive True if evaluation should be done ignoring case. Defaults to false.
     */
    Regex(const char* expression, bool case_insensitive=false)
    {
        this->expression = (uint8_t*) expression;
        this->case_insensitive = case_insensitive;
    }
    
    /**
     * Evaluates the provided string against the regular expression.
     * @param   test The string to evaluate.
     * @length  length The length of the string (safe).
     * @returns A MatchCollection object containing the matches, if any.
     */
#ifdef SAFE_BUFFERS
    MatchCollection match(const uint8_t test[], uint16_t length)
#else
     MatchCollection match(const uint8_t test[], uint16_t length=0)   
#endif
    {
        const uint8_t* p_t = test;  // Pointer to the current character in the test string.
        const uint8_t* p_e = expression;  // Pointer to the current character in the regex.
        const uint8_t* p_ma = nullptr;  // Pointer to the first character in the current match.
        CharacterClass* classing = nullptr;  // Pointer to an active character class.
        bool strict_start = false;  // Must match from beginning.
        bool in_adhoc = false;  // True when evaluating an ad-hoc class.
        bool zero_or_one_match = false;  // True when subject expression character followed by ?.
        bool zero_or_more_match = false;  // True when subject expression character followed by *.
        bool one_or_more_match = false;  // True when subject expression character followed by +.
        MatchCollection matches;  // The running list of matches.
        
        while ((*p_t != 0 && !length) || (p_t-test < length))
        {
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
            
            // Ad-hoc classes.  e.g. [abc]
            else if (*p_e == '[')
            {
                adhoc.set(p_e);
                classing = &adhoc;
                in_adhoc = true;
            }

            // Strict start.  e.g. ^Test
            else if (*p_e == '^')
            {
                strict_start = true;
                p_e++;
            }
            
            zero_or_one_match = *(p_e+1) == '?';
            zero_or_more_match = *(p_e+1) == '*';
            one_or_more_match = *(p_e+1) == '+';
            
#ifdef DEBUG_CHARACTERS
            printf(" %c%c", *p_t, *p_e);
#endif
            bool class_match = classing && classing->in(*p_t);  // Character is in the current class.
            bool exact_match = !classing && *p_t == *p_e;  // Exact character match.
            bool caseless_match = !classing && (case_insensitive && ((*p_t < 'a' && *p_t+0x20 == *p_e) || (*p_t > 'Z' && *p_t-0x20 == *p_e)));  // Case insensitive match.
            bool wildcard_match = !classing && *p_e == '.';  // Wildcard match.
            bool unconditional_match = class_match || exact_match || caseless_match || wildcard_match;  // Any of the above.
            bool conditional_match = zero_or_one_match || zero_or_more_match || one_or_more_match;
            bool match = unconditional_match || conditional_match;  // Any match
            
            // Now check for matches.
            if (match)
            {
#ifdef DEBUG_CHARACTERS
                 printf("=");
#endif
                 p_e++;  // Evaluate next character in the pattern.
                 if (p_ma == nullptr)
                    p_ma = p_t;  // Point to the start of the match.
            }
            
            // Sometimes a match.
            if (zero_or_one_match)
            {
                if (!unconditional_match)
                    p_t--;
                p_e++;  // Most past the modifier.
                zero_or_one_match = false;
            }
            
            // Sometimes a match.
            if (zero_or_more_match)
            {
                if (unconditional_match)
                {
                    p_e--;
                }
                else
                {
                    p_t--;
                    p_e += 2;  // Most past the modifier.
                    zero_or_more_match = false;                  
                }
            }
            
            // Sometimes a match.
            if (one_or_more_match)
            {
                if (unconditional_match)
                {
                    p_e--;
                }
                else
                {
                    p_e++;  // Most past the modifier.
                    p_t--;
                    one_or_more_match = false;                  
                }
            }

            if (class_match)
            {
                while (in_adhoc && *p_e != ']')  // Move the pattern pointer to the end of the adhoc class definition.
                    p_e++;
                classing = nullptr;
                if (in_adhoc)
                {
                    in_adhoc = false;
                    p_e++;
                }
            }
            

            // Enforce strict start.
            if (strict_start && p_t == test && !match)
                break;  // The first character didn't match, so we can give up.
            
            // No match, reset the pattern pointer to its start.
            if (!match)
            {
                p_e = expression;
                p_ma = nullptr;
            }
            
            // Complete match found.
            if (*p_e == 0)  // Pattern's trailing NUL encountered.
            {
                printf("#");
                Match match;
                match.value = (uint8_t*) p_ma;
                match.length = p_t - p_ma + 1;
                matches.add(match);  // add() will copy the object.
            }
     
            p_t++;  // Evaluate the next character in the test string.
       }
        
        return matches;  // All tests exhausted, return the results.
    }
    
    
    /**
     * Determines if a is in B.
     * @param   a The needle.
     * @param   B The haystack.
     * @param   l The length of the haystack (safe).
     * @returns True if a is in B, otherwise false.
     */
#ifdef SAFE_BUFFERS
    static bool in(uint8_t a, const uint8_t B[])
#else
    static bool in(uint8_t a, const uint8_t B[], uint16_t l=0)
#endif
    {
        for (const uint8_t* p=B; !l || p-B <= l; p++)
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
    CharacterClass adhoc;
    const uint8_t* set_literals = U8"()[].+\\?^$";
    const uint8_t* set_classes = U8"dwbs";

    const uint8_t* expression;
    bool case_insensitive = false;
};


void test(const char* name, const char* pattern, const uint8_t* string, bool case_insensitive=false)
{
    printf("\r\n* %s : /%s/ ", name, pattern);
    Regex regex(pattern, case_insensitive);
    MatchCollection matches = regex.match(string);
    uint8_t buffer[100];
    printf("= %d matches.\r\n", matches.get_count());
    for (size_t i=0; i < matches.get_count(); i++)
    {
        matches[i].copy(buffer, 100);
        printf("`%s`\r\n", buffer);
    }
}


int main()
{
    MatchCollection matches;
    printf("Starting.\r\n");
    uint8_t sample1[] = "Now is the time for all good men (goons) to come to the aid of their good country. In God we trust. Glm glom gloom! Toffee is goooey, gah!. My Phone number is 519-760-2914. CT scans are called cat scans.";
    
    test("strict start pass", "^Now", sample1);
    test("strict start fail", "^is", sample1);
    test("exact match pass", "good", sample1);
    test("dot subtitution", "..oo", sample1);
    test("escaped literal", "\\(go", sample1);
    test("whitespace", "\\sa", sample1);
    test("ad-hoc class", "[aeiou][aeiou]", sample1);
    test("inverted ad-hoc class", "[^abcdefghijklmnopqrstuvwxyz]", sample1);
    test("phone", "\\d\\d\\d-\\d\\d\\d-\\d\\d\\d\\d", sample1);
    test("phone partial", "[0123456789]-[0123456789]", sample1);
    test("zero_or_one", "ca?t", sample1, true);
    test("zero_or_more", "glo*m", sample1, true);
    test("one_or_more", "go+d", sample1, true);

    return 0;
}
