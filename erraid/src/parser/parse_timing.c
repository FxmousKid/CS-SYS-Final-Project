# include "parser/parse_timing.h"

bool	parse_timing(struct s_task *task) 
{
	char		buf[PATH_MAX + 1] = {0};
	char		timing[13] = {0};
	uint64_t	minutes = 0;
	uint32_t	hours = 0;
	uint8_t		days = 0;
	int		fd;

	snprintf(buf, 7 + strlen(task->path), "%stiming", task->path);
	printf("path task : %s\n", buf);

	fd = open(buf, O_RDONLY);
	if (fd <= 0) {
		return false;
	}

	if (read(fd, timing, 13) < 13) {
		return false;
	}
	memcpy(&minutes, timing, 8);

	memcpy(&hours, &timing[8], 4);

	days = (uint8_t)timing[12];
	printf("minutes: %lx hours: %x days: %x\n", minutes, hours, days);
	close(fd);
	return true;
}