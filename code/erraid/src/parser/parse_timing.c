# include "parser/parse_timing.h"

bool	parse_timing(struct s_task *task, bool debug) 
{
	char		buf[PATH_MAX + 1] = {0};
	unsigned char	timing[TIMING_SIZE] = {0};
	int		fd;
	uint64_t	minutes_he; // Host endian
	uint32_t	hours_he; // Host endian

	if (debug)
		printf("path task : %s\n", task->path);

	if (!build_safe_path(buf, PATH_MAX + 1, task->path, TIMING_FILE)) {
		ERR_MSG("failed to build timing file");
		return false;
	}

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		ERR_SYS("open");
		return false;
	}

	if (read(fd, timing, TIMING_SIZE) < TIMING_SIZE) {
		ERR_SYS("fail read");
		close(fd);
		return false;
	}

	memcpy(&minutes_he, timing, 8);
	memcpy(&hours_he, timing + 8, 4);

	task->timing.minutes = htobe64(minutes_he);
	task->timing.hours = htobe32(hours_he);
	task->timing.days = (uint8_t)timing[12];
	
	if (debug) {
		printf("minutes: %" PRIx64 " hours: %x days: %x\n", task->timing.minutes, task->timing.hours, task->timing.days);
		print_timing(task->timing);
	}
	close(fd);
	return true;
}

void	test_timing_exemple()
{
	struct s_timing t;
	t.minutes = 0x00002000000007F0;
	t.hours = 0x00041100;
	t.days = 0x5C;

	printf("EXEMPLE TIMING :\n");
	print_timing(t);
}
