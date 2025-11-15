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

/**
 * @brief Removes trailing slash from a path if present
 * 
 * @param path The path to clean
 */
void	remove_trailing_slash(char *path)
{
	size_t len = strlen(path);

	if (len > 0 && path[len - 1] == '/')
		path[len - 1] = '\0';
}


/**
 * @brief Builds a path by concatenating two parts safely
 * 
 * @param dest 		Destination buffer
 * @param dest_size 	Size of destination buffer
 * @param part1 	First part of the path
 * @param part2 	Second part of the path
 * @return 		true on success, false if truncation occurred
 */
bool	build_safe_path(char *dest, size_t dest_size, const char *part1, const char *part2)
{
	if (strlcpy(dest, part1, dest_size) >= dest_size)
		return false;
	
	size_t current_len = strlen(dest);
	
	// Add a slash if necessary
	if (current_len > 0 && current_len < dest_size - 1 && dest[current_len - 1] != '/') {
		dest[current_len] = '/';
		dest[current_len + 1] = '\0';
		current_len++;
	}
	
	if (strlcat(dest, part2, dest_size) >= dest_size)
		return false;
		
	return true;
}