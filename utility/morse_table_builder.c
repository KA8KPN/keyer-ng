#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * The purpose of this program is to generate the tables required to convert text into morse code and from morse code into text.
 * These tables are called "morse_table" and something not yet specified in the source.
 *
 * The morse table consists of a series of 16-bit numbers, with the top four bits used as a count or flag, and
 * the other bits used in various ways.  In the typical case, where the top four bits contains a value less than 13,
 * the top four bits contains a count of the number of sounds to be emitted, and the bottom 12 bits contains a 
 * bitmap where a 1 means a long sound, and a 0 is a short sound.  Each of those is separated by a short silence
 * and there is a long silence at the end of the character.
 *
 * At this time, the only other defined value for the top four bits is 13, which, with a bottom 12 bits of all zeroes, 
 * means a word space should go there.
 *
 * Each input value is stored in an input_t type value.  That contains the character (or other value, which I don't
 * yet use) the second one is a set of flags, and is currently only used to tell the system that the character is a
 * letter which also has a lowercase value.  The last field is a string.  Currently, only two types of strings are
 * defined "S" means insert a word space, and the letters "D" and "d" are used to encode sounds.  "D" for DAH and
 * d for dit.  The sounds are encoded in order from left to right.
 */

#define ALSO_LOWERCASE_FLAG 1

typedef struct {
    int c;
    unsigned long flags;
    char *target;
} input_t;

typedef struct {
    int c;
    int links[2];
} morse_decode_t;

typedef struct {
    int c;
    int macro;
} keyboard_macros_t;

input_t inputs[] = {
    {'A', ALSO_LOWERCASE_FLAG, "dD"},
    {'B', ALSO_LOWERCASE_FLAG, "Dddd"},
    {'C', ALSO_LOWERCASE_FLAG, "DdDd"},
    {'D', ALSO_LOWERCASE_FLAG, "Ddd"},
    {'E', ALSO_LOWERCASE_FLAG, "d"},
    {'F', ALSO_LOWERCASE_FLAG, "ddDd"},
    {'G', ALSO_LOWERCASE_FLAG, "DDd"},
    {'H', ALSO_LOWERCASE_FLAG, "dddd"},
    {'I', ALSO_LOWERCASE_FLAG, "dd"},
    {'J', ALSO_LOWERCASE_FLAG, "dDDD"},
    {'K', ALSO_LOWERCASE_FLAG, "DdD"},
    {'L', ALSO_LOWERCASE_FLAG, "dDdd"},
    {'M', ALSO_LOWERCASE_FLAG, "DD"},
    {'N', ALSO_LOWERCASE_FLAG, "Dd"},
    {'O', ALSO_LOWERCASE_FLAG, "DDD"},
    {'P', ALSO_LOWERCASE_FLAG, "dDDd"},
    {'Q', ALSO_LOWERCASE_FLAG, "DDdD"},
    {'R', ALSO_LOWERCASE_FLAG, "dDd"},
    {'S', ALSO_LOWERCASE_FLAG, "ddd"},
    {'T', ALSO_LOWERCASE_FLAG, "D"},
    {'U', ALSO_LOWERCASE_FLAG, "ddD"},
    {'V', ALSO_LOWERCASE_FLAG, "dddD"},
    {'W', ALSO_LOWERCASE_FLAG, "dDD"},
    {'X', ALSO_LOWERCASE_FLAG, "DddD"},
    {'Y', ALSO_LOWERCASE_FLAG, "DdDD"},
    {'Z', ALSO_LOWERCASE_FLAG, "DDdd"},
    {'0', 0, "DDDDD"},
    {'1', 0, "dDDDD"},
    {'2', 0, "ddDDD"},
    {'3', 0, "dddDD"},
    {'4', 0, "ddddD"},
    {'5', 0, "ddddd"},
    {'6', 0, "Ddddd"},
    {'7', 0, "DDddd"},
    {'8', 0, "DDDdd"},
    {'9', 0, "DDDDd"},
    {' ', 0, "S"},
    {'.', 0, "dDdDdD"},
    {',', 0, "DDddDD"},
    {'?', 0, "ddDDdd"},
    {'\'', 0, "dDDDDd"},
    {'!', 0, "DdDdDD"},
    {'/', 0, "DddDd"},
    {'(', 0, "DdDDd"}, // Prosign KN
    {')', 0, "DdDDdD"},
    {'&', 0, "dDddd"}, // Prosign: AS
    {':', 0, "DDDddd"},
    {';', 0, "DdDdDd"},
    {'=', 0, "DdddD"}, // Prosign BT
    {'+', 0, "dDdDd"}, // Prosign AR
    {'-', 0, "DddddD"},
    {'_', 0, "ddDDdD"},
    {'"', 0, "dDddDd"},
    {'$', 0, "dddDddD"},
    {'@', 0, "dDDdDd"},
    {'\\', 0, "DDDDDD"}  // Something that Anthony Good has defined as backslash, so I will use it, too.  The thing is, you have to map it to a custom character
                         // otherwise, it comes out as the Yen symbol.  I will map it to customer character 1
//  {512, 0, "dddDdD"}, // Prosign SK
//  {513, 0, "dddddddd"}, // Error
//  {514, 0, "DdDdD"}, // WTF?  "Starting Signal"  according to Wikipedia
//  {515, 0, "dddDd"}, // WTF?  "Understood" according to Wikipedia
};

keyboard_macros_t macros[] = {
    {0257, 0},
    {0260, 1}
};


void build_morse_decode_table() {
    morse_decode_t morse_decode_table[256];
    int last_element;
    int i;
    int current_element;
    char *s;

    last_element = 1;
    memset(morse_decode_table, 0, sizeof(morse_decode_table));
    for (i=0; i<((sizeof(inputs)/sizeof(input_t))); ++i) {
	current_element = 0;
	s = inputs[i].target;
	if ('S' != *s) {
	    int branch;
	    while (*s) {
		if ('D' == *s) {
		    branch = 1;
		}
		else {
		    branch = 0;
		}
		++s;
		if (0 == morse_decode_table[current_element].links[branch]) {
		    morse_decode_table[current_element].links[branch] = last_element;
		    last_element++;
		}
		current_element = morse_decode_table[current_element].links[branch];
	    }
	    morse_decode_table[current_element].c = ('\\' == inputs[i].c) ? '\x01' : inputs[i].c;
	}
    }
    puts("\n\n\n#ifdef FEATURE_MORSE_TO_TEXT");
    puts("// The morse_decode_table is an array of elements formed into a binary tree.  Each");
    puts("// of the elements consists of a printable character, which is what it prints if it is at");
    puts("// the end of a character with the pointer in that state, and an array of two elements ");
    puts("// if the next sound is a dit, it chooses the index held at offset zero in the two element");
    puts("// array to be the index of the next decode element, otherwise it chooses the index at");
    puts("// offset one.  It traverses the tree until the end of the character and then prints out the");
    puts("// character at the index of the current node.");
    puts("typedef struct {uint8_t c;uint8_t links[2];}morse_decode_t;");
    puts("#ifdef DEFINE_MORSE_TABLES");
    puts("morse_decode_t morse_decode_table[] = {");
    for (i=0; i<(last_element-1); ++i) {
	printf("{%d, {%d, %d}},\n", morse_decode_table[i].c, morse_decode_table[i].links[0], morse_decode_table[i].links[1]);
    }
    printf("{%d, {%d, %d}}\n", morse_decode_table[last_element-1].c, morse_decode_table[last_element-1].links[0], morse_decode_table[last_element-1].links[1]);
    puts("};");
    puts("#else //! DEFINE_MORSE_TABLES");
    puts("extern morse_decode_t morse_decode_table[];");
    puts("#endif //! DEFINE_MORSE_TABLES");
    puts("#endif // FEATURE_MORSE_TO_TEXT");
}


int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    puts("#ifndef MORSE_TABLE_H_INCLUDED");
    puts("#define MORSE_TABLE_H_INCLUDED\n");

    uint16_t outputs[256];
    int i;

    memset(outputs, 0, sizeof(outputs));
    for (i=0; i<((sizeof(inputs)/sizeof(input_t))); ++i) {
	int bitcount;
	uint16_t bitfield;
	char *s;

	bitcount = 0;
	bitfield = 0;
	s = inputs[i].target;

	if ('S' != *s) {
	    while (*s) {
		bitfield >>= 1;
		bitcount++;
		if('D' == *s) {
		    bitfield |= 0x800;
		}
		++s;
	    }
	    bitfield >>= (12 - bitcount);
	}
	else {
	    bitcount = 13;
	}
	bitfield |= (bitcount << 12);
	outputs[inputs[i].c] = bitfield;
	if (0 != (ALSO_LOWERCASE_FLAG & inputs[i].flags)) {
	    outputs[tolower(inputs[i].c)] = bitfield;
	}
    }

    for (i=0; i<((sizeof(macros)/sizeof(keyboard_macros_t))); ++i) {
	if (0 != outputs[macros[i].c]) {
	    fprintf(stderr, "Keyboard macro %d (keycode %d) is defined as both a regular character and a\nspecial character using the regular character\n", i, macros[i].c);
	}
	else {
	    outputs[macros[i].c] = 0xe000 | macros[i].macro;
	}
    }
    puts("#ifdef FEATURE_TEXT_TO_MORSE");
    puts("// The most significant four bits holds a count of the sounds made");
    puts("// The bottom 12 bits hold a bitmap of the length of the sounds, with the");
    puts("// least significant bit holding the first sound, the next least significant");
    puts("// bit holding the second sound, and so forth.  If the sound is long (a DAH)");
    puts("// then the corresponding bit is a one, otherwise the bit is a zero.");
    puts("// Eventually, the values, 0, 13, 14, and 15 for the most significant four");
    puts("// bits will mean different things.");
    puts("#ifdef DEFINE_MORSE_TABLES");
    puts("uint16_t morse_table[] = {");
    for (i=0; i<31; ++i) {
	printf(" // %03o '%c' %03o '%c' %03o '%c' %03o '%c' %03o '%c' %03o '%c' %03o '%c' %03o '%c'\n",
	       8*i, isprint(8*i) ? 8*i : '.',
	       8*i+1, isprint(8*i+1) ? 8*i+1 : '.',
	       8*i+2, isprint(8*i+2) ? 8*i+2 : '.',
	       8*i+3, isprint(8*i+3) ? 8*i+3 : '.',
	       8*i+4, isprint(8*i+4) ? 8*i+4 : '.',
	       8*i+5, isprint(8*i+5) ? 8*i+5 : '.',
	       8*i+6, isprint(8*i+6) ? 8*i+6 : '.',
	       8*i+7, isprint(8*i+7) ? 8*i+7 : '.');
	printf("    0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x,\n", outputs[8*i], outputs[8*i+1], outputs[8*i+2], outputs[8*i+3], outputs[8*i+4], outputs[8*i+5], outputs[8*i+6], outputs[8*i+7]);
    }
    printf(" // 370 '%c' 371 '%c' 372 '%c' 373 '%c' 374 '%c' 375 '%c' 376 '%c' 377 '%c'\n",
	   isprint(248) ? 248 : '.',
	   isprint(249) ? 249 : '.',
	   isprint(250) ? 250 : '.',
	   isprint(251) ? 251 : '.',
	   isprint(252) ? 252 : '.',
	   isprint(253) ? 253 : '.',
	   isprint(254) ? 254 : '.',
	   isprint(255) ? 255 : '.');
    printf("    0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x\n", outputs[248], outputs[249], outputs[250], outputs[251], outputs[252], outputs[253], outputs[254], outputs[255]);
    puts("};");
    puts("#else //! DEFINE_MORSE_TABLES");
    puts("extern uint16_t morse_table[];");
    puts("#endif //! DEFINE_MORSE_TABLES");
    puts("#endif // FEATURE_TEXT_TO_MORSE");

    build_morse_decode_table();

    puts("#endif // MORSE_TABLE_H_INCLUDED");
    return EXIT_SUCCESS;
}


