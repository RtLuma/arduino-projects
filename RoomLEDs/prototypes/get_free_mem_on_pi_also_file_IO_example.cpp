#include <stdio.h>
#include <string.h>
// Return the amount of free memory in kbytes.
// Returns -1 if something went wrong.
int getfreememory()
{
  int returnValue;
  const int BUFFER_SIZE = 1000;
  char buffer[BUFFER_SIZE];
  FILE *fInput;
  int loop;
  int len;
  char ch;
  returnValue = -1;
  fInput = fopen("/proc/meminfo","r");
  if (fInput != NULL)
  {
    while (!feof(fInput))
    {
      fgets(buffer,BUFFER_SIZE-1,fInput);
      if (feof(fInput))
      {
        break;
      }
      buffer[BUFFER_SIZE-1] = 0;
      // Look for serial number
      if (strncmp(buffer,"MemFree:",8)==0)
      {
        // Extract mem free from the line.
        for(loop=0;loop<BUFFER_SIZE;loop++)
        {
          ch = buffer[loop];
          if (ch == ':')
          {
             returnValue = 0;
             continue;
          }
          if (ch == 0)
          {
              break;
          }
          if (returnValue >=0)
          {
             if (ch >='A')
             {
                break;
             }
             if ((ch >='0') && (ch <='9'))
             {
                returnValue = returnValue * 10 + (ch-'0');
             }
          }
        }
        break;
      }
    } 
    fclose(fInput);
  }
  return returnValue;
}

