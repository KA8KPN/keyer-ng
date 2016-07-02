#ifndef MORSE_TABLE_H_INCLUDED
#define MORSE_TABLE_H_INCLUDED
// The most significant four bits holds a count of the sounds made
// The bottom 12 bits hold a bitmap of the length of the sounds, with the
// least significant bit holding the first sound, the next least significant
// bit holding the second sound, and so forth.  If the sound is long (a DAH)
// then the corresponding bit is a one, otherwise the bit is a zero.
// Eventually, the values, 0, 13, 14, and 15 for the most significant four
// bits will mean different things.
#ifdef DEFINE_MORSE_TABLES
uint16_t morse_table[] = {
 // 000 '.' 001 '.' 002 '.' 003 '.' 004 '.' 005 '.' 006 '.' 007 '.'
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 // 010 '.' 011 '.' 012 '.' 013 '.' 014 '.' 015 '.' 016 '.' 017 '.'
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xe04e, 0xe04f,
 // 020 '.' 021 '.' 022 '.' 023 '.' 024 '.' 025 '.' 026 '.' 027 '.'
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 // 030 '.' 031 '.' 032 '.' 033 '.' 034 '.' 035 '.' 036 '.' 037 '.'
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 // 040 ' ' 041 '!' 042 '"' 043 '#' 044 '$' 045 '%' 046 '&' 047 '''
    0xd000, 0x6035, 0x6012, 0x0000, 0x7048, 0x0000, 0x5002, 0x601e,
 // 050 '(' 051 ')' 052 '*' 053 '+' 054 ',' 055 '-' 056 '.' 057 '/'
    0x500d, 0x602d, 0x0000, 0x500a, 0x6033, 0x6021, 0x602a, 0x5009,
 // 060 '0' 061 '1' 062 '2' 063 '3' 064 '4' 065 '5' 066 '6' 067 '7'
    0x501f, 0x501e, 0x501c, 0x5018, 0x5010, 0x5000, 0x5001, 0x5003,
 // 070 '8' 071 '9' 072 ':' 073 ';' 074 '<' 075 '=' 076 '>' 077 '?'
    0x5007, 0x500f, 0x6007, 0x6015, 0x0000, 0x5011, 0x0000, 0x600c,
 // 100 '@' 101 'A' 102 'B' 103 'C' 104 'D' 105 'E' 106 'F' 107 'G'
    0x6016, 0x2002, 0x4001, 0x4005, 0x3001, 0x1000, 0x4004, 0x3003,
 // 110 'H' 111 'I' 112 'J' 113 'K' 114 'L' 115 'M' 116 'N' 117 'O'
    0x4000, 0x2000, 0x400e, 0x3005, 0x4002, 0x2003, 0x2001, 0x3007,
 // 120 'P' 121 'Q' 122 'R' 123 'S' 124 'T' 125 'U' 126 'V' 127 'W'
    0x4006, 0x400b, 0x3002, 0x3000, 0x1001, 0x3004, 0x4008, 0x3006,
 // 130 'X' 131 'Y' 132 'Z' 133 '[' 134 '\' 135 ']' 136 '^' 137 '_'
    0x4009, 0x400d, 0x4003, 0x0000, 0x0000, 0x0000, 0x0000, 0x602c,
 // 140 '`' 141 'a' 142 'b' 143 'c' 144 'd' 145 'e' 146 'f' 147 'g'
    0x0000, 0x2002, 0x4001, 0x4005, 0x3001, 0x1000, 0x4004, 0x3003,
 // 150 'h' 151 'i' 152 'j' 153 'k' 154 'l' 155 'm' 156 'n' 157 'o'
    0x4000, 0x2000, 0x400e, 0x3005, 0x4002, 0x2003, 0x2001, 0x3007,
 // 160 'p' 161 'q' 162 'r' 163 's' 164 't' 165 'u' 166 'v' 167 'w'
    0x4006, 0x400b, 0x3002, 0x3000, 0x1001, 0x3004, 0x4008, 0x3006,
 // 170 'x' 171 'y' 172 'z' 173 '{' 174 '|' 175 '}' 176 '~' 177 '.'
    0x4009, 0x400d, 0x4003, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};
#else //! DEFINE_MORSE_TABLES
extern uint16_t morse_table[];
#endif //! DEFINE_MORSE_TABLES



// The morse_decode_table is an array of elements formed into a binary tree.  Each
// of the elements consists of a printable character, which is what it prints if it is at
// the end of a character with the pointer in that state, and an array of two elements 
// if the next sound is a dit, it chooses the index held at offset zero in the two element
// array to be the index of the next decode element, otherwise it chooses the index at
// offset one.  It traverses the tree until the end of the character and then prints out the
// character at the index of the current node.
typedef struct {uint8_t c;uint8_t links[2];}morse_decode_t;
#ifdef DEFINE_MORSE_TABLES
morse_decode_t morse_decode_table[] = {
{0, {1, 3}},
{69, {9, 2}},
{65, {18, 16}},
{84, {4, 12}},
{78, {5, 7}},
{68, {6, 24}},
{66, {35, 56}},
{75, {8, 25}},
{67, {0, 48}},
{73, {14, 10}},
{85, {11, 30}},
{70, {0, 0}},
{77, {13, 20}},
{71, {26, 22}},
{83, {15, 23}},
{72, {34, 33}},
{87, {21, 17}},
{74, {0, 29}},
{82, {19, 40}},
{76, {53, 59}},
{79, {37, 27}},
{80, {0, 64}},
{81, {0, 0}},
{86, {61, 32}},
{88, {50, 0}},
{89, {51, 0}},
{90, {36, 43}},
{0, {39, 28}},
{48, {0, 0}},
{49, {47, 0}},
{0, {45, 31}},
{50, {0, 0}},
{51, {0, 0}},
{52, {0, 0}},
{53, {0, 0}},
{54, {0, 57}},
{55, {0, 0}},
{0, {38, 0}},
{56, {54, 0}},
{57, {0, 0}},
{0, {41, 0}},
{43, {0, 42}},
{46, {0, 0}},
{0, {0, 44}},
{44, {0, 0}},
{0, {46, 58}},
{63, {0, 0}},
{39, {0, 0}},
{0, {55, 49}},
{33, {0, 0}},
{47, {0, 0}},
{40, {0, 52}},
{41, {0, 0}},
{38, {0, 0}},
{58, {0, 0}},
{59, {0, 0}},
{61, {0, 0}},
{45, {0, 0}},
{95, {0, 0}},
{0, {60, 0}},
{34, {0, 0}},
{0, {62, 0}},
{0, {0, 63}},
{36, {0, 0}},
{0, {65, 0}},
{64, {0, 0}}
};
#else //! DEFINE_MORSE_TABLES
extern morse_decode_t morse_decode_table[];
#endif //! DEFINE_MORSE_TABLES
#endif // MORSE_TABLE_H_INCLUDED
