#include <errno.h>

int impress (int id) {
	int res;
	__asm__ (
		"int $0x80;"
		: "=a" (res)
		: "0" (243) ,"b" (id)
		: "memory"
	);
	if ((res) < 0) {
		errno = (-res);
		return -1;
	}
	return res;
}
