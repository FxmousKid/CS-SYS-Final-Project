#include "utils/utils.h"

/**
 * @brief Print usage string on stdout : -h option
 */
void	print_help(void)
{
	printf("\
Usage: tadmor [OPTION...]\n\
A client that can interact the erraid daemon\n\
\n\
Options Valid for tadmor :\n\
\n\
 -l, --list-tasks\n\
 -r, --remove-task=TASKID\n\
 -x, --show-exit-code-history=TASKID\n\
 -o, --show-last-stdout=TASKID\n\
 -e, --show-last-stderr=TASKID\n\
\n\
Options to create tasks :\n\
 -c, --create-simple-command\n\
 -s, --create-sequence-command\n\
 -m, --minutes=MINUTES\n\
 -H, --hours=HOURS\n\
 -d, --days-of-week=DAYSOFWEEK\n\
 -n, --no-time\n\
\n\
Miscellenaeous options :\n\
 -q, --stop-daemon\n\
 -p, --pipes-directory=PATH\n\
 -b, --debug\n\
 -h, --help\n\
\n\
Mandatory or optional arguments to long options are also mandatory or optional\
 for short options\n\
\n\
Made with Love by Iyan, Theo, and Florian.\n\
");
}

int	get_logfd(void)
{
	static int fd = 0;

	if (fd == 0)
		fd = open(LOGFILE_PATH, O_RDWR | O_CREAT | O_TRUNC, 0644);

	if (fd < 0) {
		printf("Could not create logfile\n");
		exit(2);
	}

	return fd;
}

/**
 * @brief prints on stderr a custom error msg
 *
 * @param location the macro that wraps __FILE__ and __LINE__
 *
 * @note this is internal function, and location is usually
 * a macro expanding to file and line number
 */
void	_write_perr(const char *location)
{
	dprintf(get_logfd(), ": %s [%s]\n", strerror(errno), location);
}

/**
 * @brief prints on stderr a custom error msg
 *
 * @param msg the custom error message
 *
 * @note this is internal function, and location is usually
 * a macro expanding to file and line number
 */
void	_write_err(const char *location)
{

	dprintf(get_logfd(), "[%s]\n", location);
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

/**
 * @brief Convert a relative path to an absolute path
 *
 * @param relative_path	relative path to convert
 * @param absolute_path	buffer to write the absolute path
 * @return 		true on success, false otherwise
 */
bool	convert_to_absolute_path(const char *relative_path, char *absolute_path)
{
	char    *tmp;

	if (relative_path[0] == '/') {
		strlcpy(absolute_path, relative_path, PATH_MAX + 1);
		return true;
	}

	tmp = realpath(relative_path, NULL);
	if (!tmp) {
		ERR_SYS("realpath : %s", relative_path);
		return false;
	}

	strlcpy(absolute_path, tmp, PATH_MAX + 1);
	free(tmp);
	return true;
}

/**
 * @brief Converts a binary bitmask into a string format, or '*' or '-' for special cases.
 *
 * @param mask      The uint64_t, uint32_t, or uint8_t bitmask (MINUTES HOURS DAYSOFWEEK).
 * @param max_val   The maximum index to check (59 for MINUTES, 23 for HOURS, 6 for DAYSOFWEEK).
 * @return          A dynamically allocated string (for example "0,5,10", "*", or "-").
 */
static char	*mask_to_str(uint64_t mask, int max_val)
{
	char	*str;
	int	count = 0;
	size_t	len = 0;

	str = malloc(MAX_TIMING_LEN);
	if (!str) {
		ERR_SYS("malloc");
		return NULL;
	}
	str[0] = '\0';

	for (int i = 0; i <= max_val; i++) {
		if ((mask >> i) & 1) {
			count++;
			if (len > 0)
				len += snprintf(str + len, MAX_TIMING_LEN - len, ",%d", i);
			else
				len += snprintf(str + len, MAX_TIMING_LEN - len, "%d", i);
		}
	}
	// Special cases: No execution time ('-') or every time ('*')
	if (count == 0) {
		free(str);
		// strdup is used to respect the dynamic allocation
		return strdup("-");
	} else if (count == (max_val + 1)) {
		free(str);
		// strdup is used to respect the dynamic allocation
		return strdup("*");
	}

	return str;
}
/**
 * @brief Converts the s_timing structure (bitmasks) into a formatted string.
 *
 * @param timing    Pointer to the s_timing structure with minutes, hours, and days bitmasks.
 * @return          A dynamically allocated string representing the schedule.
 */
char	*timing_to_string(const struct s_timing *timing)
{
	size_t	total_len;
	char	*min_str = NULL;
	char	*hour_str = NULL;
	char	*day_str = NULL;
	char	*result = NULL;
	
	// Convert masks
	day_str = mask_to_str(timing->days, 6);
	hour_str = mask_to_str(timing->hours, 23);
	min_str = mask_to_str(timing->minutes, 59);

	if (!min_str || !hour_str || !day_str)
		goto error;

	// Allocate the result string (+3 is for the 2 spaces and '\0')
	total_len = strlen(min_str) + strlen(hour_str) + strlen(day_str) + 3;
	result = malloc(total_len);
	if (!result) {
		ERR_SYS("malloc");
		goto error;
	}

	// Format the string to "MINUTES HOURS DAYSOFWEEK"
	snprintf(result, total_len, "%s %s %s", min_str, hour_str, day_str);

error:
	if (min_str) free(min_str);
	if (hour_str) free(hour_str);
	if (day_str) free(day_str);

	return result;
}

/**
 * @brief Formats the task output (TASKID, timing, command) into a string and writes it to STDOUT.
 *
 * @param task_id       The taskid.
 * @param timing_str    The formatted timing string.
 * @param cmd_str       The command line string.
 * @return              true on successful write, false on write error or truncation.
 */
bool	write_task_output(uint64_t task_id, const char *timing_str, const char *cmd_str)
{
	char	buf[MAX_LINE_LEN];
	int	len;

	len = snprintf(buf, MAX_LINE_LEN, "%" PRIu64 ": %s %s\n", task_id, timing_str, cmd_str);

	if (write(STDOUT_FILENO, buf, len) != len) {
		ERR_SYS("write truncated %s", buf);
		return false;
	}
	return true;
}