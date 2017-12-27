#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "DCDriver.hpp"

void printUsage(const char* progName)
{
   printf("Usage: %s source [output]\n",progName);
}

int main(const int argc, const char **argv)
{
   if( argc == 3 )
   {
      DC::DCDriver driver;

      driver.parse( argv[1] );

      std::ofstream file;

      file.open(argv[2]);

      driver.print( file ) << "\n";
   }
   else if( argc == 2 )
   {
      DC::DCDriver driver;
      if( std::strncmp( argv[ 1 ], "-h", 2 ) == 0 )
      {
         printUsage(argv[0]);
         return( EXIT_SUCCESS );
      }
      else
      {
         driver.parse( argv[1] );
         driver.print( std::cout ) << "\n";
      }
   }
   else
   {
      printUsage(argv[0]);
      return ( EXIT_FAILURE );
   }
   return( EXIT_SUCCESS );
}
