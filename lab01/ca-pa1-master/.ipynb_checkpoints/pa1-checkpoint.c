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

typedef struct Huffman {
	int code;
	int length;
} Huffman;

Huffman const H[16] = {
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

void body_encode(Huffman *curr, Huffman next, char *outp, int *outbytes) {
	int code = next.code;
	int length = next.length;

	curr->code = (curr->code << length) + code;
	curr->length += length;
	if (curr->length >= 8) {
        outp[(*outbytes)++] = (curr->code >> (curr->length % 8));
		curr->code &= (1 << (curr->length % 8)) - 1;
		curr->length %= 0x08;
	}
}

/* TODO: Implement this function */
int encode(const char *inp, int inbytes, char *outp, int outbytes)
{
    if (inbytes == 0)
        return 0;
    
	int freq[16] = {0};
	int rank[16];
	int n_rank = 0, max_outbytes = outbytes;

	for (int i = 0; i < inbytes; i++) {
		int nibble1 = (inp[i] & 0xF0) >> 4;
		int nibble2 = (inp[i] & 0x0F);
		freq[nibble1]++;
		freq[nibble2]++;
	}
	
	while (n_rank < 8) {
		int maxIdx = 0;
		for (int i = 0; i < 16; i++)
			if (freq[maxIdx] < freq[i])
				maxIdx = i;
        
		if (n_rank % 2 == 0)
            outp[n_rank/2] = maxIdx;
        else {
            outp[n_rank/2] <<= 4;
            outp[n_rank/2] += maxIdx;
        }
        
		freq[maxIdx] = -1;
		rank[maxIdx] = n_rank++;
    }
    outbytes = 4;
    if (outbytes >= max_outbytes) return -1;

	for (int i = 0; i < 16; i++)
		if (freq[i] != -1)
			rank[i] = n_rank++;
    
	Huffman curr = {0x00, 4};
	for (int i = 0; i < inbytes; i++) {
        if (outbytes >= max_outbytes) return -1;
		int nibble1 = (inp[i] & 0xF0) >> 4;
		body_encode(&curr, H[rank[nibble1]], outp, &outbytes);
        
		int nibble2 = (inp[i] & 0x0F);
        if (outbytes >= max_outbytes) return -1;
		body_encode(&curr, H[rank[nibble2]], outp, &outbytes);
	}
    
    int padding = (8-curr.length) % 8;
    if (padding) {
        if (outbytes >= max_outbytes) return -1;
        outp[4] += padding << 4;
        outp[outbytes++] = curr.code << padding;
    }

	return outbytes;
}
