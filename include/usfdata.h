#ifndef USFDATA_H
#define USFDATA_H

#include <stdint.h>
#define USFNULL ((usf_data) { .u = 0 })
#define USFTRUE ((usf_data) { .u = 1 })
#define USFDATAP(d) ((usf_data) { .p = d })
#define USFDATAU(d) ((usf_data) { .u = d })
#define USFDATAI(d) ((usf_data) { .i = d })

typedef union usf_data {
	void *p;
	uint64_t u;
	int64_t i;
} usf_data;

#endif
