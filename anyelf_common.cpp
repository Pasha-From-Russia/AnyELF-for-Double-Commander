/*
Copyright (C) 2020-2022 by Pasha-From-Russia

Common functions for AnyElf-DoubleCommander plugin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "anyelf.h"
#include <fcntl.h>
#include <unistd.h>

//---------------------------------------------------------------------------
void DCPCALL ListGetDetectString(
        char *t_detectString,
        int   t_maxlen)
{
    if (!t_detectString) {
        return;
    }
    snprintf(t_detectString, t_maxlen, "%s", "EXT=\"*\"");
}

//---------------------------------------------------------------------------
bool isElfFile(const char *t_fileToLoad)
{
    int f = open(t_fileToLoad, O_RDONLY);
    if (f == -1) {
        return false;
    }
    uint32_t header = 0;
    int8_t size = sizeof(header);
    read(f, &header, size);
    close(f);
    //check if has 0x7F E L F signature
    return (header == 0x464C457F);
}
