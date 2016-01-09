namespace I3SNHelpers
{
    
    void ByteSwap(unsigned char* b, int n);

    #define ByteSwap5(x) ByteSwap((unsigned char*) &x, sizeof(x))
    
    int64_t ByteSwapDOMClock(char dom_time[6]);

};