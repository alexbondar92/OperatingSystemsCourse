#include <errno.h>
#include <stdlib.h>

struct _cs_log{
	pid_t prev;
	pid_t next;
	int prev_priority;
	int next_priority;
	int prev_policy;
	int next_policy;
	long switch_time;
	int n_tickets;
};
typedef struct _cs_log cs_log;

int enable_logging (int size) {
	int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (243), "b" (size)
		: "memory"
		);
	
	if ((res) < 0) {
		errno = (-res);
		return -1;
	}
	return res;
}

int disable_logging () {
	int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (244)
		: "memory"
		);
	
	if ((res) < 0) {
		errno = (-res);
		return -1;
	}
	return res;
}

int get_logger_records (cs_log* user_mem) {
	int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (245), "b" (user_mem)
		: "memory"
		);
	
	if ((res) < 0) {
		errno = (-res);
		return -1;
	}
	return res;
}

int start_lottery_scheduler () {
	int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (246)
		: "memory"
		);
	
	if ((res) < 0) {
		errno = (-res);
		return -1;
	}
	return res;
}

int start_orig_scheduler () {
	int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (247)
		: "memory"
		);
	
	if ((res) < 0) {
		errno = (-res);
		return -1;
	}
	return res;
}

int set_max_tickets (int max_tickets) {
	int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (248), "b" (max_tickets)
		: "memory"
		);
	
	if ((res) < 0) {
		errno = (-res);
		return -1;
	}
	return res;
}
