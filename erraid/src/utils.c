#include "utils.h"

void	print_help(void)
{
	printf("\
Usage: erraid [OPTION...]\n\
Spawns a deamon that receives tasks to perform at specific times.\n\
\n\
Options Valid for the deamon :\n\
\n\
 -r, --run-directory=PATH\n\
 -h, --help\n\
\n\
Mandatory or optional arguments to long options are also mandatory or optional\
 for short options\n\
\n\
Made with Love by Iyan, Theo, and Florian.\n\
");
}
