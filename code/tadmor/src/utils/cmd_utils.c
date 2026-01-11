#include "utils/cmd_utils.h"

static void free_argv(char **darr)
{
	char	**tmp;

	if (!darr)
		return;
	
	tmp = darr;
	while (*tmp)
	{
		free(*tmp);
		tmp++;
	}
	free(darr);
}

static void cleanup_partial_argv(char **argv, uint32_t count)
{
	for (uint32_t j = 0; j < count; j++) {
		if (argv[j])
		free(argv[j]);
	}
	if (argv)
		free(argv);
}


/**
 * @brief Reconstructs a single command line string from an array of arguments (argv).
 * For example, {"echo", "coucou", NULL} returns "echo coucou"
 *
 * @param argv  he NULL-terminated array of strings.
 * @return A dynamically allocated command string.
 */
static char *args_to_string(char **argv)
{
	char *str;
	size_t len = 0;
	size_t size = INITIAL_CMD_STR_SIZE;
	size_t arg_len;

	if (!argv || !*argv)
		return strdup("");

	if (!(str = calloc(size, sizeof(char)))) {
		ERR_SYS("calloc");
		return NULL;
	}

	for (int i = 0; argv[i] != NULL; ++i) {
		arg_len = strlen(argv[i]);
		
		// Need to be sure to have enough space for arg + separator + '\0'
		while (len + arg_len + 2 > size) {
		size *= 2;
		char *tmp = realloc(str, size);
		if (!tmp) {
			ERR_SYS("realloc");
			free(str);
			return NULL;
		}
		str = tmp;
		}

		// Add a space if it is not the first argument
		if (i > 0)
		str[len++] = ' ';
		
		memcpy(str + len, argv[i], arg_len);
		len += arg_len;
	}
	str[len] = '\0';
	return str;
}

static char **read_arguments(int fd)
{
	uint32_t argc;
	char **argv = NULL;

	// Read ARGC <uint32>
	if (!read_uint32(fd, &argc))
		return NULL;

	argv = calloc(argc + 1, sizeof(char*));
	if (!argv) {
		ERR_SYS("calloc");
		return NULL;
	}

	for (uint32_t i = 0; i < argc; i++) {
		uint32_t len;

		// Read LENGTH <uint32>
		if (!read_uint32(fd, &len)) {
			cleanup_partial_argv(argv, i);
			return NULL;
		}

		// Allocate LENGTH size + 1 for '\0'
		argv[i] = malloc(len + 1);
		if (!argv[i]) {
			ERR_SYS("malloc");
			cleanup_partial_argv(argv, i);
			return NULL;
		}

		// Read DATA <LENGTH bytes>
		if (!read_exact(fd, argv[i], len)) {
			cleanup_partial_argv(argv, i + 1);
			return NULL;
		}
		argv[i][len] = '\0';
	}
	argv[argc] = NULL; // Null-terminate the array
	return argv;
}

static bool	cmd_to_str_internal(int fd, char **cmd_str, bool parentheses)
{
	uint16_t 	type;
	uint32_t	nb_cmds;
	
	// Read TYPE <uint16>
	if (!read_uint16(fd, &type))
		return false;

	if (type == CMD_SI) {
		char **argv = read_arguments(fd);
		if (!argv)
			return false;
		
		*cmd_str = args_to_string(argv);
		free_argv(argv);
		return *cmd_str != NULL;
		
	}
	// TYPE = 'SQ' or 'PL' or 'IF' or 'ND' or 'OR'
	else {
		// Read NBCMDS <uint32>
		if (!read_uint32(fd, &nb_cmds))
			return false;
		return assemble_cmd_string(fd, type, nb_cmds, cmd_str, parentheses);
	}

	return false;
}

/**
 * @brief Recursively reads a CMD_SQ or CMD_PL from the FIFO and reconstructs the command string.
 * Format: ( [sub_cmd_1] ; [sub_cmd_2] ; ... ) or ( [sub_cmd_1] | [sub_cmd_2] | ... )
 *
 * @param fd        The file descriptor to read from.
 * @param type	    The command type.
 * @param nb_cmds   The number of sub-commands in the sequence.
 * @param cmd_str   Pointer to a char* where the resulting string will be stored.
 * @return true on success, false on read/allocation failure.
 */
bool assemble_cmd_string(int fd, uint16_t type, uint32_t nb_cmds, char **cmd_str, bool parentheses)
{
	char		*str;
	char		*tmp;
	size_t		len = 0;
	size_t		size = INITIAL_CMD_STR_SIZE;

	if (nb_cmds == 0) {
		*cmd_str = strdup("()"); // Or empty idk should not happen
		return *cmd_str != NULL;
	}

	if (!(str = calloc(size, sizeof(char)))) {
		ERR_SYS("calloc");
		return false;
	}

	if (parentheses) {
		// Start with '('
		str[len++] = '(';
		str[len++] = ' ';
	}

	// For every sub commands
	for (uint32_t i = 0; i < nb_cmds; i++) {
		char *sub_cmd_str = NULL;

		if (!cmd_to_str_internal(fd, &sub_cmd_str, true) || !sub_cmd_str) {
			free(str);
			if (sub_cmd_str) free(sub_cmd_str);
			return false;
		}

		size_t sub_len = strlen(sub_cmd_str);

		while (len + sub_len + 15 > size) {
			size *= 2;
			tmp = realloc(str, size);
			if (!tmp) {
				ERR_SYS("realloc");
				free(sub_cmd_str);
				free(str);
				return false;
			}
			str = tmp;
		}

		// Add ';' or '|' if it's not the first command
		if (i > 0) {
			switch(type){
			case CMD_SQ:
				len += sprintf(strl + len, " ; ");
				break;
			case CMD_PL:
				len += sprintf(strl + len, " | ");
				break;
			case CMD_ND:
				len += sprintf(strl + len, " && ");
				break;
			case CMD_OR:
				len += sprintf(strl + len, " || ");
				break;
			default:
				break;
			}
		}
		if (type == CMD_IF) {
			switch (i){
			case 0:
				len += sprintf(str + len, "if ");
				break;
			case 1:
				len  += sprintf(str + len, " ; then ");
				break;
			case 2:
				len += sprintf(str + len, " else ");
				break;
			default:
				break;
			}
		}


		memcpy(str + len, sub_cmd_str, sub_len);
		len += sub_len;
		free(sub_cmd_str);
	}

	if (type == CMD_IF)
		len += sprintf(str + len, " ; fi");

	if (parentheses) {
		// Add ')' at the end
		str[len++] = ' ';
		str[len++] = ')';
	}
	str[len] = '\0';
	*cmd_str = str;
	return true;
}

bool	cmd_to_str(int fd, char **cmd_str)
{
	return cmd_to_str_internal(fd, cmd_str, false);
}

bool	request_opt_tasks(char *fifo_request, uint16_t opcode, uint64_t opt)
{
	// Send OPCODE in BE
	char	buf[10] = {0};

	opcode = htobe16(opcode);
	opt = htobe64(opt);
	memcpy(buf, &opcode, 2);
	memcpy(buf + 2, &opt, 8);
	if (!writefifo(fifo_request, buf, 10))
		return false;
	return true;
}