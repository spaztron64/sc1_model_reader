#include <stdio.h>
//Soul Calibur 1 Arcade
//Data decompressor function, decompiled with IDA 7.5

#include <stdlib.h>
#include <string.h>
#include "defs.h"

// Function prototype
unsigned int __fastcall sub_800448C8(char *a1, int *a2);

unsigned int __fastcall sub_800448C8(char *a1, int *a2)
{
  int *v2; // $t1
  int v3; // $t0
  int v4; // $v0
  char v5; // $v0
  unsigned int v6; // $v1
  int v7; // $v0
  int v8; // $v1
  unsigned int v9; // $v0
  int v10; // $a2
  unsigned int v11; // $v1
  int *v12; // $a3
  int v13; // $v0
  int v14; // $v0

  v2 = a2;
LABEL_2:
  v3 = (unsigned __int8)*a1++;
  while ( v3 )
  {
    if ( v3 != 255 )
    {
      while ( 1 )
      {
        v4 = v3 & 1;
        if ( v3 == 1 )
          break;
        v3 >>= 1;
        if ( v4 )
        {
          v5 = *a1++;
          *(_BYTE *)a2 = v5;
          a2 = (int *)((char *)a2 + 1);
        }
        else
        {
          v7 = (unsigned __int8)*a1;
          v8 = (unsigned __int8)a1[1];
          a1 += 2;
          v9 = (v7 << 8) | v8;
          v10 = (((unsigned __int8)(v9 >> 11) - 1) & 0x1F) + 1;
          v11 = (((_WORD)v9 - 1) & 0x7FF) + 1;
          v12 = (int *)((char *)a2 - v11);
          if ( (((_WORD)v9 - 1) & 0x7FF) != 0 )
          {
            if ( v11 >= 4 )
              v11 = 4;
            do
            {
              v10 -= v11;
              *a2 = *v12;
              a2 = (int *)((char *)a2 + v11);
              v12 = (int *)((char *)v12 + v11);
            }
            while ( v10 > 0 );
            a2 = (int *)((char *)a2 + v10);
          }
          else
          {
            v13 = *(unsigned __int8 *)v12 | (*(unsigned __int8 *)v12 << 8);
            v14 = v13 | (v13 << 16);
            do
            {
              *a2 = v14;
              v10 -= 4;
              ++a2;
            }
            while ( v10 > 0 );
            a2 = (int *)((char *)a2 + v10);
          }
        }
      }
      goto LABEL_2;
    }
    v6 = *((_DWORD *)a1 + 1);
    *a2 = *(_DWORD *)a1;
    a2[1] = v6;
    v3 = HIBYTE(v6);
    a1 += 8;
    a2 = (int *)((char *)a2 + 7);
  }
  return ((char *)a2 - (char *)v2 + 3) & 0xFFFFFFFC;
}


int main(int argc, char *argv[])
{
// Check if a file name was provided
if (argc < 2) {
printf("Error: Please provide a file name.\n");
return 1;
}

// Open the file for reading
FILE *fp = fopen(argv[1], "rb");
if (!fp) {
printf("Error: Could not open file.\n");
return 1;
}

// Get the size of the file
fseek(fp, 0, SEEK_END);
size_t size = ftell(fp);
rewind(fp);

// Allocate a buffer to hold the file data
char *buffer = (char *)malloc(size);
if (!buffer) {
printf("Error: Could not allocate buffer.\n");
return 1;
}

// Read the file data into the buffer
if (fread(buffer, 1, size, fp) != size) {
printf("Error: Could not read file.\n");
return 1;
}

// Close the file
fclose(fp);

// Allocate a buffer to hold the decompressed data
int *decompressed = (int *)malloc(size * 2);
if (!decompressed) {
printf("Error: Could not allocate decompressed buffer.\n");
return 1;
}

// Decompress the data
size_t decompressed_size = sub_800448C8(buffer, decompressed);

// Write the decompressed data to a new file
fp = fopen("decompressed.bin", "wb");
if (!fp) {
printf("Error: Could not create output file.\n");
return 1;
}
if (fwrite(decompressed, 1, decompressed_size, fp) != decompressed_size) {
printf("Error: Could not write to output file.\n");
return 1;
}

// Close the file and free the buffers
fclose(fp);
free(buffer);
free(decompressed);

return 0;
}