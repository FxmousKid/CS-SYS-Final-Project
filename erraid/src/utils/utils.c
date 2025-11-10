#include "utils/utils.h"

/**
 * @brief Print usage string on stdout : -h option
 */
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
 -l, --little-endian\n\
\n\
Mandatory or optional arguments to long options are also mandatory or optional\
 for short options\n\
\n\
Made with Love by Iyan, Theo, and Florian.\n\
");
}

/**
 * @brief prints on stderr a custom error msg
 *
 * @param msg the custom error message
 *
 * @note this is internal function, and location is usually
 * a macro expanding to file and line number
 */
void	_write_perr(const char *func, const char *location)
{
	char	buf[256] = {0};
	strcpy(buf, "erraid: ");
	strcat(buf, func);
	strcat(buf, ": ");
	strcat(buf, strerror(errno));
	strcat(buf, location);

	write(STDERR_FILENO, buf, sizeof(buf));
}

/**
 * @brief prints on stderr a custom error msg
 *
 * @param msg the custom error message
 *
 * @note this is internal function, and location is usually
 * a macro expanding to file and line number
 */
void		_write_err(const char *msg, const char *location)
{
	char	buf[256] = {0};
	strcpy(buf, "erraid: ");
	strcat(buf, msg);
	strcat(buf, ": ");
	strcat(buf, location);

	write(STDERR_FILENO, buf, sizeof(buf));
}

/**
 * @brief prints the string representation of a cmd_type enum
 *
 * @param type the enum to print
 * @param newline whether to print a newline after the enum string
 */
void	print_cmd_enum(enum cmd_type type, bool newline)
{
	if (type == CMD_SI)
		printf("CMD_SI");
	else if (type == CMD_SQ)
		printf("CMD_SQ");
	else if (type == CMD_IF)
		printf("CMD_IF");
	else if (type == CMD_PL)
		printf("CMD_PL");
	else
		printf("UNKNOWN_CMD_TYPE");
	if (newline)
		printf("\n");
}

/**
 * @brief prints a NULL-terminated double array of strings
 *
 * @param tab the double array to print
 */
void	print_darr(const char *tab_name, char **tab)
{
	printf("%s = {", tab_name);
	while (*tab) {
		printf("\"%s\", ", *tab);
		tab++;
	}
	printf("\b\b}\n");
}

/**
 * @brief Frees a NULL-terminated double array
 *
 * @param darr the double array to free
 */
void	free_darr(char **darr)
{
	char	**tmp;

	tmp = darr;
	while (*tmp)
	{
		free(*tmp);
		tmp++;
	}
	free(darr);
}

/**
 * @brief Appends the string representation of an integer to a buffer
 *
 * @param buf the buffer to append to
 * @param n the integer to append
 */
void	append_int_to_buf(char *buf, int n)
{
	char	num_buf[12];
	int		i;
	int		j;

	buf += strlen(buf);
	if (n == 0)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return ;
	}
	i = 0;
	while (n > 0)
	{
		num_buf[i++] = (n % 10) + '0';
		n /= 10;
	}
	j = 0;
	while (i-- > 0)
		buf[j++] = num_buf[i];
	buf[j] = '\0';
}
