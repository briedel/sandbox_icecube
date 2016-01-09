namespace I3SNHelpers
{

    void ByteSwap(unsigned char* b, int n)
    {
        register int i = 0;
        register int j = n-1;
        while (i<j)
        {
            std::swap(b[i], b[j]);
            i++, j--;
        }
    }
    
    
    int64_t ByteSwapDOMClock(char dom_time[6])
    {
        int64_t shifted_dom_time;
        shifted_dom_time = ((int64_t) dom_time[5] +
                          (((int64_t) dom_time[4])<<8) +
                          (((int64_t) dom_time[3])<<16) +
                          (((int64_t) dom_time[2])<<24) +
                          (((int64_t) dom_time[1])<<32) +
                          (((int64_t) dom_time[0])<<40));
        return shifted_dom_time;
    }
}