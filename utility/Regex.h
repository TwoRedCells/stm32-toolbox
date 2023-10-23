#define DEBUG
/**
 * Regular expression library for microcontrollers.
 * (c) 2023 Yvan Rodrigues, all rights reserved.
 * 
 * This library uses regular expressions to evaluate whether a string matches a pattern.
 * While not a fully-functional POSIX-style regex handler, it is optimized to provided
 * the core functionality that is most often used in the logic of microcontrollers and
 * embedded systems, while occupying a minimal footprint of compiled code.
 * 
 * Features
 * --------
 * * Uses uint8_t* as the underlying string type.
 * * Can be compiled with `#define SAFE_BUFFERS` to use explicit string lengths instead of NUL termination.
 * * No dynamic memory allocation, and minimal stack usage. All comparisons are done using pointers to strings allocated by the programmer.
 * * Case-sensitive or case-insensitive matching.
 * * Exact character-to-character matches.
 * * Match any character (`.` operator).
 * * Ad-hoc character classes (`[]` operators), such as `[aeiou]`.
 * * Predefined character classes (`\w` => alpha, `\d` => numeric, `\s` => whitespace).
 * * Repetition operators (`?` => zero or one, `*` => zero or more, `+` => one or more).
 * * Start-of-string and end-of-string matching (`^$` operators).
 * * Boundary matching (`\b` operator).
 * * Escaping of reserved charcaters with `\\`.
 * 
 * Limitations
 * -----------
 * * No distinction is made between single-line and multi-line matching.
 * * All matches are non-greedy.
 * * Groups are not supported (yet).
 * 
 * How to Use
 * ----------
 * * Instantiate a `Regex` object by passing the constuctor a regular expression, and optionally whether case-sensitive matching should be used.
 * * Call the `match` method of the `Regex` object to start evaluation. A `MatchCollection` object will be returned with zero or more matches.
 * * Use the `copy` method of the `Match` object to copy a matching result to your buffer.
 * 
 * Example
 * -------
 * const uint8_t test[] = "My postal code is N0B 1E0. In Canada Santa Claus uses H0H 0H0.";
 * Regex postal(U8"\w\d\w \d\w\d");
 * MatchCollection matches = postal.match(test);
 * 
 * uint8_t buffer[100];
 * printf("There were %d" matches.\r\n", matches.get_count());
 * for (int i=0; i < matches.get_count(); i++)
 * {
 *     matches[i].copy(buffer);
 *     printf("[%d]: %s\r\n", i, buffer);
 * }
 */
 
#ifdef DEBUG
    #include <stdio.h>  // for printf
#endif
#include <stdint.h>
#include <memory.h>  // for memcpy

// This can be used before string literals e.g. U8"Hello, world!"
#define U8 (const uint8_t*)
//#define DEBUG_CHARACTERS

// If defined, NUL termination of the test string will not be relied on.
// The regular expression itself must be NUL terminated.
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
    bool includes(uint8_t test)
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


/**
 * Represents a repetition definition, enclosed in curly braces.
 */  
class Repetition
{
	friend class Regex;

private:
	size_t min;
	size_t max;
	
	void parse(const uint8_t* definition, size_t length)
	{
		
	}
	
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
        const uint8_t* p_m = p_t;  // Pointer to the first character in the current match.
        CharacterClass* classing = nullptr;  // Pointer to an active character class.
        bool strict_start = false;  // Must match from beginning of the test.
		bool strict_end = false;  // Must match to the end of the test.
        bool in_adhoc = false;  // True when evaluating an ad-hoc class.
        MatchCollection matches;  // The running list of matches.
        size_t lookahead = 0;
      
        while ((*p_t != 0 && !length) || (p_t-test < length))
        {
			bool boundary_test = false;  // True when looking for a word boundary.
            // STEP 1: SET THE CURRENT STATE BASED ON MODIFIERS.
#ifdef DEBUG_CHARACTERS
            printf("%s %c/%c", p_e == expression ? "\r\n" : "", *p_e, *p_t);
#endif
            
			// Check for boundary testing.
			if (*p_e == '\\' && *(p_e+1) == 'b')
			{
				boundary_test = true;
				p_e++;
			}
			
            // When in the 'classing' state, the value of the expression pointer represents a class of one or more characters,
            // so instead of checking for value equality we check if the set includes the test subject.
            if (!classing)
            {
				// Escaped classes.  e.g. \d
                if (*p_e == '\\')
                {
                    p_e++;
					switch (*p_e)
					{
                    case 'd':
                        classing = &class_numeric;
						break;
                    case 'D':
                        classing = &class_not_numeric;
						break;
                    case 'w':
                        classing = &class_alpha;
						break;
                    case 'W':
                        classing = &class_not_alpha;
						break;
 					case 's':
                        classing = &class_whitespace;
						break;
					case 'S':
                        classing = &class_not_whitespace;
						break;
					}
                }
                
                // Ad-hoc classes.  e.g. [abc]
                else if (*p_e == '[')
                {
                    adhoc.set(p_e);  // A single CharacterClass is reused for ad-hoc classes.
                    classing = &adhoc;  // We remain in classing mode until this is reset to nullptr.
                    while (*p_e != ']')  // Move the pattern pointer to the end of the adhoc class definition.
                        p_e++;
                 }
    
                // Strict start.  e.g. ^Test
                else if (*p_e == '^')
                {
                    strict_start = true;
                    p_e++;
                }
    
                // Strict end.  e.g. test$
                else if (*p_e == '$')
                {
                    strict_end = true;
                    p_e++;
                }
            }
			
         
            // Look ahead for operators that change the meaning of the current expression pointer.
            if (class_lookahead.includes(*(p_e+1)))
                lookahead = 1;
            bool const zero_or_one_match = *(p_e+1) == '?';  // True when subject expression character followed by ?.
            bool zero_or_more_match = *(p_e+1) == '*';  // True when subject expression character followed by *.
            bool one_or_more_match = *(p_e+1) == '+';  // True when subject expression character followed by +.

            // STEP 2: CHECK FOR ALL THE KINDS OF MATCHES.
            bool class_match = classing && classing->includes(*p_t);  // Character is in the current class.
            bool exact_match = !classing && *p_t == *p_e;  // Exact character match.
            bool caseless_match = !classing && (case_insensitive && ((*p_t < 'a' && *p_t+0x20 == *p_e) || (*p_t > 'Z' && *p_t-0x20 == *p_e)));  // Case insensitive match.
            bool wildcard_match = !classing && *p_e == '.';  // Wildcard match.
            bool unconditional_match = class_match || exact_match || caseless_match || wildcard_match;  // Any of the above.
            // The above are true matches, where the test pointer matches the exact meaning of the expression pointer.

            // Whereas the following are conditional on what follows.
 			bool boundary_match = boundary_test && ((p_t == test && class_alpha.includes(*p_t)) || (*p_t == 0 && class_alpha.includes(*(p_t-1))) || (p_t != test && class_alpha.includes(*(p_t+1)) && class_whitespace.includes(*p_t)) || (*p_t != 0 && class_alpha.includes(*(p_t-1)) && class_whitespace.includes(*p_t)));
            bool more_match = (zero_or_more_match || one_or_more_match) && unconditional_match;  // Matched because additional charcaters in a 'many' match.
            bool zero_match = (zero_or_one_match || zero_or_more_match || one_or_more_match) && !unconditional_match;  // Matched because the first character matches everything.
            bool match = unconditional_match || zero_match || boundary_match;  // Any match

            // STEP 3: TAKE ACTIONS BASED ON MATCHES OR LACK OF MATCHES. THIS MAY MEAN MOVING THE EXPRESSION POINTER OR THE TEST POINTER.
            // Enforce strict start.
            if (strict_start && !match && p_t == test)
                break;  // The first character didn't match, so we can give up.

            // Enforce strict end.
            if (strict_end && *p_e != 0)
                break;
                
            // When a character doesn't match, we abandon the current partial match and reset.
            if (!match)
            {
                p_e = expression;
                p_t = p_m ? p_m+1 : p_t+1;
                p_m = p_t;  // Pick up where we left off.
                classing = nullptr;
                continue;
            }			

            // A zero match doesn't capture any new characters, but it advances the expression pointer.
            if (zero_match)
            {
                p_e += (1 + lookahead);
                p_t += 0;
                lookahead = 0;
				classing = nullptr;
            }
            
            // In a more match we wait for multiples of the same character without advancing the expression pointer.
            else if (more_match)
            {
                p_t++;
                p_e += 0;
            }
            
			// Boundary match where the start of the boundary is the start of the test.
			else if (boundary_match)
			{
				p_t += 0;
				p_e++;
			}

            // All others.
            else
            {
                p_t++;
                p_e += (1 + lookahead);
                lookahead = 0;
                
                // Clear out of the current class.
                classing = nullptr;
            }

            // Complete match found; capture it.
            if (*p_e == 0)  // Pattern's trailing NUL encountered.
            {
                p_t--;
                printf("#");
                Match match;
                match.value = (uint8_t*) p_m;
                match.length = p_t - p_m + 1;
                matches.add(match);  // add() will copy the object.
                p_e = expression;
                p_t++;
                p_m = p_t;
            }
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
    static inline CharacterClass class_not_numeric = CharacterClass(U8"[^0123456789]");  // \d
    static inline CharacterClass class_alpha = CharacterClass(U8"[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]"); // \w
    static inline CharacterClass class_not_alpha = CharacterClass(U8"[^ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]"); // \w
    static inline CharacterClass class_whitespace = CharacterClass(U8"[\t\r\n ]");  // \s
    static inline CharacterClass class_not_whitespace = CharacterClass(U8"[^\t\r\n ]");  // \s
    static inline CharacterClass class_lookahead = CharacterClass(U8"[?*+]");  // \s
    CharacterClass adhoc;
    const uint8_t* set_literals = U8"()[].+\\?^$";
    const uint8_t* set_classes = U8"dwbs";
    const uint8_t* expression;
    bool case_insensitive = false;
	Repetition _repetition;
	Repetition* repetition = nullptr;

};


void test(const char* name, const char* pattern, const uint8_t* string, size_t expected=999, bool case_insensitive=false)
{
    printf("\r\n* %s : /%s/ ", name, pattern);
    Regex regex(pattern, case_insensitive);
    MatchCollection matches = regex.match(string);
    uint8_t buffer[100];
    printf("= %d matches (%d expected).\r\n", matches.get_count(), expected);
	if (matches.get_count() != expected)
	{
		for (size_t i=0; i < matches.get_count(); i++)
		{
			matches[i].copy(buffer, 100);
			printf("`%s`\r\n", buffer);
		}
	}
}


int main()
{
    MatchCollection matches;
    printf("Starting.\r\n");
    uint8_t sample1[] = "Now is the time for all good men (goons) to come to the aid of their good country. In God we trust. Glm glom gloom gloooomy! Toffee is goooey, gah!. My Phone number is 519-760-2914. CT scans are called cat scans. My postal code is N0B 1E0 but Santa's is H0H 0H0.";
    
    test("strict start pass", "^Now", sample1, 1);
    test("strict start fail", "^is", sample1, 0);
    test("exact match pass single", "h", sample1, 5);
    test("exact match pass", "good", sample1, 2);
    test("dot subtitution", "..oo", sample1, 6);
    test("escaped literal", "\\(go", sample1, 1);
    test("whitespace", "\\sa", sample1, 3);
    test("ad-hoc class", "[aeiou][aeiou]", sample1, 12);
    test("inverted ad-hoc class", "[^abcdefghijklmnopqrstuvwxyz]", sample1, 98);
    test("phone", "\\d\\d\\d-\\d\\d\\d-\\d\\d\\d\\d", sample1, 1);
    test("phone partial", "[0123456789]-[0123456789]", sample1, 2);
    test("zero_or_one1", "ca?t", sample1, 2, true);
    test("zero_or_one2", "\\d?\\d", sample1, 4, true);
    test("zero_or_more1", "glo*m", sample1, 4, true);
    test("one_or_more", "go+d", sample1, 3, true);
    test("phone", "\\d+-\\d+-\\d+", sample1, 1);
	test("postal", "\\w\\d\\w \\d\\w\\d", sample1, 2, true);
	test("boundary", "\\bt", sample1, 999, true);
    return 0;
}
