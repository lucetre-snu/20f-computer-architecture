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

typedef struct {
	unsigned char code;
	int bits;
} table_entry;

// Huffman Coding
static table_entry table[] = {
	{0x00, 3}, {0x01, 3}, {0x02, 3}, {0x03, 3},
	{0x08, 4}, {0x09, 4}, {0x0a, 4}, {0x0b, 4},
	{0x18, 5}, {0x19, 5}, {0x1a, 5}, {0x1b, 5},
	{0x1c, 5}, {0x1d, 5}, {0x1e, 5}, {0x1f, 5}
};

static int concat(char *string, int *len_p, int *rest_bits_p, const table_entry *tentry, int buffer_len)
{
	/*
	 * Case 1: The previous result is byte-aligned
	 * Case 2: The last byte has enough space for newly appended bits
	 * Case 3: The last byte doesn't have enough space for new bits
	 */

	if (*rest_bits_p == 0) {			// Case 1
		if (*len_p >= buffer_len)
			return 1;
		string[(*len_p)++] = tentry->code << (8 - tentry->bits);
		*rest_bits_p = 8 - tentry->bits;
	} else if (*rest_bits_p >= tentry->bits) {	// Case 2
		string[*len_p-1] |= tentry->code << (*rest_bits_p - tentry->bits);
		*rest_bits_p -= tentry->bits;
	} else {					// Case 3
		if (*len_p >= buffer_len)
			return 1;
		string[*len_p-1] |= tentry->code >> (tentry->bits - *rest_bits_p);
		string[(*len_p)++] = tentry->code << (8 - tentry->bits + *rest_bits_p);
		*rest_bits_p += 8 - tentry->bits;
	}

	return 0;
}

int encode(const char *inp, int inbytes, char *outp, int outbytes)
{
	// Handle null string
	if (inbytes == 0)
		return 0;

	// Output buffer is too short
	if (outbytes <= 4)
		return -1;

	int count[16] = {0};
	int nibble_to_rank[16];

	// Measure frequency of each nibble
	for (int i = 0; i < inbytes; i++) {
		char c = inp[i];
		count[c & 0x0f]++;
		count[(c >> 4) & 0x0f]++;
	}

	// Rank 0 ~ 7, construct rank table
	unsigned char header_entry;
	for (int i = 0; i < 8; i++) {
		int max = 0;
		int freq_nibble = 16;
		for (int j = 15; j >= 0; j--)
			if (count[j] >= max) {
				max = count[j];
				freq_nibble = j;
			}
		nibble_to_rank[freq_nibble] = i;
		count[freq_nibble] = -1;

		if (i%2 == 0) {
			header_entry = freq_nibble << 4;
		} else {
			header_entry |= freq_nibble;
			outp[i/2] = header_entry;
		}
	}

	// Rank 8 ~ 15
	int rank = 8;
	for (int nibble = 0; nibble < 16; nibble++)
		if (count[nibble] >= 0)
			nibble_to_rank[nibble] = rank++;

	// Encode
	outp[4] = 0;
	int len = 1;
	int rest_bits = 4;
	for (int i = 0; i < inbytes; i++) {
		char c = inp[i];

		// Upper 4 bits
		if (concat(outp + 4, &len, &rest_bits,
					table + nibble_to_rank[(c >> 4) & 0x0f], outbytes - 4))
			return -1;

		// Lower 4 bits
		if (concat(outp + 4, &len, &rest_bits,
					table + nibble_to_rank[c & 0x0f], outbytes - 4))
			return -1;
	}

	outp[4] |= (rest_bits << 4);
	return len + 4;
}
