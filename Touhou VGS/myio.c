#include <stdio.h>

/*
 *----------------------------------------------------------------------------
 * fread
 *----------------------------------------------------------------------------
 */
size_t my_fread(void *buf, size_t size, size_t n, FILE *fp)
{
	size_t ret;
	size_t total=0;
	size_t req;
	req=n;
	req*=size;
	n=req;
	size=1;
	do {
		ret=fread(buf,size,n,fp);
		total+=ret;
		n-=ret;
		if(total==req) {
			return total;
		}
	} while(!feof(fp) && !ferror(fp));
	return total;
}

/*
 *----------------------------------------------------------------------------
 * fwrite
 *----------------------------------------------------------------------------
 */
size_t my_fwrite(void *buf, size_t size, size_t n, FILE *fp)
{
	size_t ret;
	size_t total=0;
	size_t req;
	req=n;
	req*=size;
	n=req;
	size=1;
	do {
		ret=fwrite(buf,size,n,fp);
		total+=ret;
		n-=ret;
		if(total==req) {
			return total;
		}
	} while(!ferror(fp));
	return total;
}
