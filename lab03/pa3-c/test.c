union Byte {
	struct {
		unsigned int second: 4;
		unsigned int first: 4;
	} pair;
	unsigned int value: 8;
};

union Boolean {
    unsigned int value: 1;
};


union Byte findHuffman(union Byte r, int *len, char *outp, int *outb, int *rank, int *outbytes) {
    if (*outb < 0) return r;
    
    int l = *len-1, x = 0, y = 0;
    if (l < 0) return r;
    if (r.value & (1<<l)) {
        l--;
        if (l < 0) return r;
        if (r.value & (1<<l)) {
            l -= 3;
            if (l < 0) return r;
            x = 8;
            y = ((r.value & (7<<l))>>l);
            *len -= 5;   
        }
        else {
            l -= 2;
            if (l < 0) return r;
            x = 4;
            y = ((r.value & (3<<l))>>l);
            *len -= 4;
        }
    }
    else {
        l -= 2;
        if (l < 0) return r;
        x = 0;
        y = ((r.value & (3<<l))>>l);
        *len -= 3;
    }
    
    int ind = (*outb)>>1;
    if(ind+1 > *outbytes) {
        *outb = -1;
        return r;
    }
    if(((*outb)++) % 2)
        outp[ind] <<= 4;
    outp[ind] += rank[x+y];
    return findHuffman(r, len, outp, outb, rank, outbytes);
}

int min(int x, int y) {
    if (x>y) return y;
    return x;
}


int decode(const unsigned char *inp, int inbytes, char *outp, int outbytes) {
    if (inbytes == 0) return 0;

    int rank[16];
    union Boolean freq[16] = {0};
    int k = 0;
    for (; k < 8; k += 2) {
        union Byte b;
        b.value = inp[k >> 1];
        rank[k] = b.pair.first;
        rank[k+1] = b.pair.second;
        freq[b.pair.first].value = 1;
        freq[b.pair.second].value = 1;
    }
    for (int i = 0; i < 16; i++) {
        if (freq[i].value == 0) {
            rank[k++] = i;
        }
    }
    
    union Byte b;
    b.value = inp[4];
    unsigned int padding = b.pair.first;
    
    union Byte remainder;
    remainder.value = b.pair.second;
    int outb = 0, len = 4;
    remainder = findHuffman(remainder, &len, outp, &outb, rank, &outbytes);
    
    for (int i = 5; i < inbytes; i++) {
        union Byte b;
        b.value = inp[i];
        
        if (i+1 != inbytes || padding <= 4) {
            len += 4;
            remainder.value <<= 4;
            remainder.value += b.pair.first;
            remainder = findHuffman(remainder, &len, outp, &outb, rank, &outbytes);
        }
        else {
            len += (8-padding);
            remainder.value <<= (8-padding);
            remainder.value += (b.pair.first >> (padding-4));
            remainder = findHuffman(remainder, &len, outp, &outb, rank, &outbytes);
        }
        
        if (i+1 != inbytes) {
            len += 4;
            remainder.value <<= 4;
            remainder.value += b.pair.second;
            remainder = findHuffman(remainder, &len, outp, &outb, rank, &outbytes);
        }
        else if (padding < 4) {
            len += (4-padding);
            remainder.value <<= (4-padding);
            remainder.value += (b.pair.second >> padding);
            remainder = findHuffman(remainder, &len, outp, &outb, rank, &outbytes);
        }
    }
    return outb;
}
