/*
 */

#define	SOCKDBG(s)

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "gtypes.h"
#include "gc.h"
#define	NOUNIXPROTOTYPES
#include <jsyscall.h>

static struct {
	int				inuse;
	union {
		HANDLE		hand;
		SOCKET		sock;
	};
	jlong			timeout;	/* threads timeout on read */
} fhand[FD_SETSIZE];

static
int
allocHandle(void)
{
	int i;

	/* Skip stdin, stdout, stderr */
	for (i = 0; i < FD_SETSIZE; i++) {
		if (fhand[i].inuse == 0) {
			fhand[i].inuse = 1;
			return (i);
		}
	}

	return (-1);
}

static
void
freeHandle(int fd)
{
	fhand[fd].inuse = 0;
}

static
void
Finit(void)
{
	WORD ver;
	WSADATA data;

	fhand[0].inuse = 1;
	fhand[0].hand = GetStdHandle(STD_INPUT_HANDLE);
	fhand[1].inuse = 1;
	fhand[1].hand = GetStdHandle(STD_OUTPUT_HANDLE);
	fhand[2].inuse = 1;
	fhand[2].hand = GetStdHandle(STD_ERROR_HANDLE);

	/* This should be in Ninit but we don't have one */
	ver = MAKEWORD(2,0);
	WSAStartup(ver, &data);
}

static
int
Fmkdir(const char* path)
{
	if (CreateDirectory(path, NULL)) {
		return (0);
	}
	else {
		return (-1);
	}
}

static
int
Frmdir(const char* path)
{
	if (RemoveDirectory(path)) {
		return (0);
	}
	else {
		return (-1);
	}
}

static
int
FfileInfo(const char* path, int type, void* extra)
{
	BY_HANDLE_FILE_INFORMATION buf;
	HANDLE h;
	LARGE_INTEGER li;
	DWORD attr;

	attr = GetFileAttributes(path);
	if (attr == 0xFFFFFFFF) {
		return (0);
	}
	
	switch (type) {
	case JINFO_EXISTS:
		return (1);

	case JINFO_ISFILE:
		if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
			return (1);
		}
		return (0);

	case JINFO_ISDIR:
		if (attr & FILE_ATTRIBUTE_DIRECTORY) {
			return (1);
		}
		return (0);

	case JINFO_CANREAD:
		return (1);

	case JINFO_CANWRITE:
		if (!(attr & FILE_ATTRIBUTE_READONLY)) { 
			return (1);
		}
		return (0);
	}

	h = CreateFile(path, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (h == INVALID_HANDLE_VALUE) {
		printf("Invalid %d\n", GetLastError());
		return (0);
	}
	GetFileInformationByHandle(h, &buf);
	CloseHandle(h);

	switch (type) {
	case JINFO_LENGTH:
		*((jlong*)extra) = buf.nFileSizeLow;
		return (1);

	case JINFO_LASTMODIFIED:
		li.LowPart = buf.ftLastWriteTime.dwLowDateTime;
		li.HighPart = buf.ftLastWriteTime.dwHighDateTime;
		*((jlong*)extra) = (jlong)li.QuadPart / (jlong)10;
		return (1);
	}

	return (0);
}

static
int
FfileInfoFD(int fd, int type, void* extra)
{
	DWORD r;

	switch (type) {
	case JINFO_LENGTH:
		r = GetFileSize(fhand[fd].hand, NULL);
		if (r != 0xFFFFFFFF) {
			*((jlong*)extra) = (jlong)r;
			return (1);
		}
		break;

	case JINFO_AVAILABLE:
		return (GetFileSize(fhand[fd].hand, NULL) - SetFilePointer(fhand[fd].hand, (LONG)0, NULL, FILE_CURRENT));

	default:
		break;
	}

	return (0);
}

static
int
Freaddir(const char* p, jdirEntry** list)
{
	WIN32_FIND_DATA data;
	char path[256];
	jdirEntry* dirlist;
	jdirEntry* mentry;
	int count;
	HANDLE handle;

	strcpy(path, p);
	strcat(path, "\\*.*");

	handle = FindFirstFile(path, &data);
	if (handle == INVALID_HANDLE_VALUE) {
		*list = 0;
		return (-1);
	}

	count = 0;
	dirlist = 0;
	for (;;) {
		/* We skip '.' and '..' */
		if (strcmp(".", data.cFileName) != 0 && strcmp("..", data.cFileName) != 0) {
			mentry = gc_malloc_fixed(sizeof(jdirEntry) + strlen(data.cFileName) + 1);
			assert(mentry != 0);
			strcpy(mentry->name, data.cFileName);
			mentry->next = dirlist;
			dirlist = mentry;
			count++;
		}
		if (!FindNextFile(handle, &data)) {
			FindClose(handle);
			break;
		}
	}
	*list = dirlist;
	return (count);
}

/*
 * Threaded file open.
 */
static
int
Fopen(const char* path, int mode)
{
	int fd;

	fd = allocHandle();
	if (fd == -1) {
		return (-1);
	}
	switch (mode) {
	case JOPEN_READ:
		fhand[fd].hand = CreateFile(path, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		break;
	case JOPEN_WRITE:
		fhand[fd].hand = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		break;
	case JOPEN_APPEND:
		fhand[fd].hand = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		break;
	case JOPEN_READWRITE:
		fhand[fd].hand = CreateFile(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		break;
	default:
		freeHandle(fd);
		return (-1);
	}

	// printf("Opening %s on %d %x\n", path, fd, fhand[fd].hand);

	if (fhand[fd].hand == INVALID_HANDLE_VALUE) {
		freeHandle(fd);
		return (-1);
	}
	else {
		return (fd);
	}
}

/*
 * Threaded read
 */
static
ssize_t
Fread(int fd, void* buf, size_t len)
{
	ssize_t r;
	BOOL res;

	/* It would appear that we should block on a read of length
	 * zero if there's still data to be read but none happens
	 * to be available (to JDK 1.1.6 spec)
	 */
#if 0
	if (len == 0) {
		blockOnFile(fd, TH_READ, fhand[fd].timeout);
		return (0);
	}
#endif

	res = ReadFile(fhand[fd].hand, (LPVOID)buf, (DWORD)len, (LPDWORD)&r, NULL);
	if (res == 0) {
		r = -1;
	}

	return (r);
}

/*
 * Threaded write
 */
static
ssize_t
Fwrite(int fd, const void* buf, size_t len)
{
	ssize_t r;
	BOOL res;

	res = WriteFile(fhand[fd].hand, (LPVOID)buf, (DWORD)len, (LPDWORD)&r, NULL);
	if (res == 0) {
		r = -1;
	}

	return (r);
}

static
int
Flseek(int fd, off_t pos, int whence)
{
	DWORD ret;

	switch (whence) {
	case JSEEK_SET:
		ret = SetFilePointer(fhand[fd].hand, (LONG)pos, NULL, FILE_BEGIN);
		break;
	case JSEEK_CUR:
		ret = SetFilePointer(fhand[fd].hand, (LONG)pos, NULL, FILE_CURRENT);
		break;
	case JSEEK_END:
		ret = SetFilePointer(fhand[fd].hand, (LONG)pos, NULL, FILE_END);
		break;
	}

	// printf("seek %d %d %d -> %d\n", fd, (int)pos, whence, ret);

	return ((int)ret);
}

static
int
Fclose(int fd)
{
	CloseHandle(fhand[fd].hand);
	freeHandle(fd);
	return (0);
}

/* --------------------------------------------------------------------- */
/* Networking */

/*
 * Threaded socket create.
 */
static
int
Nsocket(int stream)
{
	int fd;

	fd = allocHandle();
	if (fd == -1) {
		return (-1);
	}
	if (stream == 0) {
		fhand[fd].sock = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, 0);
	}
	else {
		fhand[fd].sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	}
	if (fhand[fd].sock == INVALID_SOCKET) {
		freeHandle(fd);
		return (-1);
	}
	else {
		return (fd);
	}
}

/*
 * Threaded socket connect.
 */
static
int
Nconnect(int fd, struct sockaddr* addr, size_t len)
{
	int r;

	r = WSAConnect(fhand[fd].sock, (const struct sockaddr*)addr, len, NULL, NULL, NULL, NULL);

	if (r == SOCKET_ERROR) {
		return (-1);
	}
	else {
		return (0);
	}
}

/*
 * Threaded socket accept.
 */
static
int
Naccept(int fd, struct sockaddr* addr, size_t* len)
{
	int nfd;

	nfd = allocHandle();
	if (nfd == -1) {
		return (-1);
	}

	fhand[nfd].sock = WSAAccept(fhand[fd].sock, addr, len, NULL, 0);

	if (fhand[nfd].sock == INVALID_SOCKET) {
		freeHandle(nfd);
		return (-1);
	}
	return (nfd);
}

static
int
Nlisten(int fd, int len)
{
	if (listen(fhand[fd].sock, len) == SOCKET_ERROR) {
		return (-1);
	}
	else {
		return (0);
	}
}

static
int
Nbind(int fd, struct sockaddr* addr, int len)
{
	if (bind(fhand[fd].sock, addr, len) == SOCKET_ERROR) {
		return (-1);
	}
	else {
		return (0);
	}
}

static
int
Nclose(int fd)
{
	closesocket(fhand[fd].sock);
	freeHandle(fd);
	return (0);
}


/*
 * Threaded recvfrom 
 */
static
ssize_t 
Nrecvfrom(int fd, void* buf, size_t len, int flags, 
	struct sockaddr* from, int* fromlen)
{
	ssize_t r;
	int ret;
 	WSABUF wbuf;

	wbuf.len = (u_long)len;
	wbuf.buf = (char FAR*)buf;

	ret = WSARecvFrom(fhand[fd].sock, &wbuf, 1, &r, &flags,
		from, fromlen, NULL, NULL);

	if (ret == SOCKET_ERROR) {
		return (-1);
	}
	return (r);
}

/*
 * Threaded recvfrom 
 */
static
ssize_t 
Nread(int fd, void* buf, size_t len)
{
 	WSABUF wbuf;
	int r;
	int flags;
	int ret;

	wbuf.len = (u_long)len;
	wbuf.buf = (char FAR*)buf;
	flags = 0;

	ret = WSARecv(fhand[fd].sock, &wbuf, 1, &r, &flags, NULL, NULL);

	if (ret == SOCKET_ERROR) {
		return (-1);
	}
	return (r);
}

static
ssize_t
Nsendto(int fd, const void* buf, size_t len, int flags, const struct sockaddr* addr, int alen)
{
	ssize_t r;
	int res;
	WSABUF wbuf;

	wbuf.len = (u_long)len;
	wbuf.buf = (char FAR*)buf;

	if (addr != NULL)
		res = WSASendTo(fhand[fd].sock, &wbuf, 1, &r, flags, 
			addr, alen, NULL, NULL);
	else
		res = WSASend(fhand[fd].sock, &wbuf, 1, &r, flags, NULL, NULL);

	if (res == SOCKET_ERROR) {
		int err = WSAGetLastError();
		return (-1);
	}
	else {
		return (r);
	}
}

static
ssize_t
Nwrite(int fd, const void* buf, size_t len)
{
	ssize_t r;
	int res;
	WSABUF wbuf;

	wbuf.len = (u_long)len;
	wbuf.buf = (char FAR*)buf;
	res = WSASend(fhand[fd].sock, &wbuf, 1, &r, 0, NULL, NULL);

	if (res == SOCKET_ERROR) {
		return (-1);
	}
	else {
		return (r);
	}
}

/*
 * Set the socket timeout.
 */
static
int
Nsetreadtimeout(int fd, int time)
{
	fhand[fd].timeout = (jlong)time;
	return (1);
}

/*
 * Get the socket timeout.
 */
static
int
Ngetreadtimeout(int fd)
{
	return ((int)fhand[fd].timeout);
}

static
char*
Ngethostname(void)
{
	char* buf;

	buf = gc_malloc_fixed(64);
	if (gethostname(buf, 64) == 0) {
		return (buf);
	}
	else {
		return ("localhost");
	}
}

static
int*
Ngethostbyname(char* name)
{
	struct hostent* ent;
	int* addrs;

	ent = gethostbyname(name);
	if (ent == 0) {
		return (0);
	}
	addrs = (int*)gc_malloc_fixed(sizeof(int) * 2);
	addrs[0] = *(int*)ent->h_addr;
	addrs[1] = 0;
	return (addrs);
}

static
char*
Ngethostbyaddr(int addr)
{
	struct hostent* ent;

	ent = gethostbyaddr((char*)&addr, 4, AF_INET);
	if (ent == 0) {
		return (0);
	}
	return (ent->h_name);
}

static
int
Ngetsockname(int fd, struct sockaddr* addr, int* alen)
{
	if (getsockname(fhand[fd].sock, addr, alen) == SOCKET_ERROR) {
		return (-1);
	}
	else {
		return (0);
	}
}

static
int
Ngetpeername(int fd, struct sockaddr* addr, int* alen)
{	
	if (getpeername(fhand[fd].sock, addr, alen) == SOCKET_ERROR) {
		return (-1);
	}
	else {
		return (0);
	}
}

static
int
Nsetsockopt(int fd, int opt, int x1, const void* x2, int x3)
{
	return (-1);
}

static
int
Ngetsockopt(int fd, int opt, int x1, void* x2, int* x3)
{
	return (-1);
}

static
int
Nioctl(int fd, int opt, int* val)
{
	return (-1);
}

FileSystemCallInterface Kaffe_FileSystemCallInterface = {

	Finit,

	Fopen,
	Fread,	
	Fwrite, 
	Flseek,
	Fclose,
	FfileInfo,
	FfileInfoFD,

	Freaddir,

	Fmkdir,
	Frmdir,
	rename,
	remove,

};

NetworkCallInterface Kaffe_NetworkCallInterface = {

	Nsocket,
	Nbind,
	Nlisten,
	Nconnect,
	Naccept,
	Nread,	
	Nrecvfrom,

	Nwrite,
	Nsendto,
	Nclose,
	Nsetsockopt,   
	Ngetsockopt,
	Ngetsockname,
	Ngetpeername,

	Ngethostname,
	Ngethostbyname,
	Ngethostbyaddr,
	Nioctl,

	Nsetreadtimeout,
	Ngetreadtimeout,

};
