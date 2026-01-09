
#include "utils/timing_utils.h"

uint64_t	parse_timing(char *arg)
{
	uint64_t	time = 0;
	bool		inter = false;
	int		last_check = 0;
	int		current = 0;
	int		number;
	int		second;
	int		len = strlen(arg);

	//printf("arg : %s\n", arg);
	while (current <= len) {
		if (arg[current] == ',' || arg[current] == '\0') {
			arg[current] = '\0';
			if (!inter) {
				number = atoi(arg + last_check);
				//printf("%d last : %d arg : %s\n", number, last_check, arg + last_check);
				time = (time | (1 << number));
			} 
			else {
				second = atoi(arg + last_check);
				//printf("number : %d second : %d\n", number, second);
				inter = (number < second);
				while (((inter) ? second - number : number - second) >= 0) {
					time = (inter) ? (time | (1 << number++)) : (time | (1 << second++));
				}
				inter = false;
			}
			last_check = current + 1;
		}
		else if (arg[current] == '-') {
			inter = true;
			arg[current] = '\0';
			number = atoi(arg + last_check);
			last_check = current + 1;
		}
		current++;
	}
	//printf("time : %lX\n", time);
	return time;
}


minutes_t	parse_minutes(char *arg)
{
	return parse_timing(arg);
}

hours_t		parse_hours(char *arg)
{
	return parse_timing(arg);
}

dasyofweek_t	parse_days(char *arg)
{
	return parse_timing(arg);
}