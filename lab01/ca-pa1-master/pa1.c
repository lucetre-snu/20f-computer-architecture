//---------------------------------------------------------------
//
//  4190.308 Computer Architecture (Fall 2020)
//
//  Project #1: Compressing Data with Huffman Coding
//
//  September 9, 2020
//
//  Injae Kang (abcinje@snu.ac.kr)
//  Sunmin Jeong (sunnyday0208@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//---------------------------------------------------------------
#include <stdio.h>

typedef struct Huffman {
	int code;
	int length;
} Huffman;

Huffman const H[0x10] = {
	{0b000, 3},
	{0b001, 3},
	{0b010, 3},
	{0b011, 3},
	{0b1000, 4},
	{0b1001, 4},
	{0b1010, 4},
	{0b1011, 4},
	{0b11000, 5},
	{0b11001, 5},
	{0b11010, 5},
	{0b11011, 5},
	{0b11100, 5},
	{0b11101, 5},
	{0b11110, 5},
	{0b11111, 5}
};

Huffman body_encode(Huffman curr, Huffman next) {
	char code = next.code;
	int length = next.length;

	curr.code = (curr.code << length) + code;
	curr.length += length;
	if (curr.length >= 0x08) {
		printf("%02x ", curr.code >> (curr.length % 0x08));
		curr.code &= (1 << (curr.length % 0x08)) - 1;
		curr.length %= 0x08;
	}
	return curr;
}

/* TODO: Implement this function */
int encode(const unsigned char *inp, int inbytes, unsigned char *outp, int outbytes)
{
	int freq[0x10] = {0};
	int rank[0x10];
	int n_rank = 0;

	for (int i = 0; i < inbytes; i++) {
		int nibble1 = (inp[i] & 0xF0) >> 4;
		int nibble2 = (inp[i] & 0x0F);
		freq[nibble1]++;
		freq[nibble2]++;
	}
	
	while (n_rank < 0x08) {
		int maxIdx = 0;
		for (int i = 0; i < 0x10; i++)
			if (freq[maxIdx] < freq[i])
				maxIdx = i;
		freq[maxIdx] = -1;
		rank[maxIdx] = n_rank++;
		
		printf("%01x", maxIdx);
	}

	for (int i = 0; i < 0x10; i++)
		if (freq[i] != -1)
			rank[i] = n_rank++;

	printf("\nHufffdmanEncod121ing\n");

	Huffman curr = {0x00, 4};
	for (int i = 0; i < inbytes; i++) {
		int nibble1 = (inp[i] & 0xF0) >> 4;
		int nibble2 = (inp[i] & 0x0F);

		curr = body_encode(curr, H[rank[nibble1]]);
		curr = body_encode(curr, H[rank[nibble2]]);
	}
	printf("\n\n%d %02x", curr.length, curr.code);

	// for (int i = 0; i < 0x10; i++) {
	// 	printf("%01b ", H[i].code);
	// }
	

	return 0;
}
