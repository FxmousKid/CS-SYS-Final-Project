# include "parser/parse_timing.h"

#include <inttypes.h>

bool	parse_timing(struct s_task *task) 
{
	char		buf[PATH_MAX + 1] = {0};
	char		timing[13] = {0};
	int		fd;
	uint64_t	minutes_le;
	uint32_t	hours_le;

	task->timing.minutes = 0;
	task->timing.hours = 0;
	task->timing.days = 0;

	if (!build_safe_path(buf, PATH_MAX + 1, task->path, TIMING_FILE))
		ERR_MSG("failed to build timing file");
	
	printf("path task : %s\n", buf);

	fd = open(buf, O_RDONLY);
	if (fd <= 0) {
		ERR_MSG("fail open");
		return false;
	}

	if (read(fd, timing, 13) < 13) {
		ERR_MSG("fail read");
		return false;
	}

	memcpy(&minutes_le, timing, 8);
	memcpy(&hours_le, &timing[8], 4);

	task->timing.minutes = (minutes_le);
	task->timing.hours = (hours_le);
	task->timing.days = (uint8_t)timing[12];
	

	// printf("minutes: %lx hours: %x days: %x\n", task->timing.minutes, task->timing.hours, task->timing.days);
	printf("minutes: %" PRIu64 "hours: %x days: %x\n", task->timing.minutes, task->timing.hours, task->timing.days);
	close(fd);

	print_timing(task->timing);

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
