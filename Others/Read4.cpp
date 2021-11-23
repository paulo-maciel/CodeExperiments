/**
 * The read4 API is defined in the parent class Reader4.
 *     int read4(char *buf4);
 */

class Solution
{
public:
  /**
     * @param buf Destination buffer
     * @param n   Number of characters to read
     * @return    The number of actual characters read
     */
  int read(char *buf, int n)
  {
    int nRead4 = n / 4;
    char *ptrBuf = buf;
    int leftToRead = n;

    // Read all chunks of 4.
    for (int i = 0; i < nRead4; ++i)
    {
      int r = read4(ptrBuf);
      ptrBuf += r;
      leftToRead -= r;
    }

    // asked to read more than there are available characters.
    if (leftToRead < 4)
    {
      int r = read4(ptrBuf);
      *(ptrBuf + min(leftToRead, r)) = '\0';
      leftToRead = 0;
    }

    return n - leftToRead;
  }
};