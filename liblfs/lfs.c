/*
** LuaFileSystem
** Copyright Kepler Project 2003 (http://www.keplerproject.org/luafilesystem)
** Adapted for MISP - Sebastien Bourdeauducq, 2012
**
** File system manipulation library.
** This library offers these functions:
**   lfs.attributes (filepath [, attributename])
**   lfs.dir (path)
**   lfs.mkdir (path)
**   lfs.rmdir (path)
**   lfs.symlinkattributes (filepath [, attributename]) -- thanks to Sam Roberts
**
** $Id: lfs.c,v 1.61 2009/07/04 02:10:16 mascarenhas Exp $
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <yaffsfs.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <lfs.h>

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

/*
 * ** compatibility with Lua 5.2
 * */
#if (LUA_VERSION_NUM == 502)
#undef luaL_register
#define luaL_register(L,n,f) \
	        { if ((n) == NULL) luaL_setfuncs(L,f,0); else luaL_newlib(L,f); }

#endif

#define DIR_METATABLE "directory metatable"
typedef struct dir_data {
	int  closed;
	yaffs_DIR *dir;
} dir_data;

/*
** Utility functions
*/
static int pusherror(lua_State *L, const char *info)
{
	lua_pushnil(L);
	if (info==NULL)
		lua_pushstring(L, strerror(errno));
	else
		lua_pushfstring(L, "%s: %s", info, strerror(errno));
	lua_pushinteger(L, errno);
	return 3;
}

static int pushresult(lua_State *L, int i, const char *info)
{
	if (i==-1)
		return pusherror(L, info);
	lua_pushinteger(L, i);
	return 1;
}

/*
** Creates a link.
** @param #1 Object to link to.
** @param #2 Name of link.
** @param #3 True if link is symbolic (optional).
*/
static int make_link(lua_State *L)
{
	const char *oldpath = luaL_checkstring(L, 1);
	const char *newpath = luaL_checkstring(L, 2);
	return pushresult(L,
		(lua_toboolean(L,3) ? yaffs_symlink : yaffs_link)(oldpath, newpath), NULL);
}


/*
** Creates a directory.
** @param #1 Directory path.
*/
static int make_dir (lua_State *L) {
	const char *path = luaL_checkstring (L, 1);
	int fail;
	fail =  yaffs_mkdir (path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
	                     S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH );
	if (fail) {
		lua_pushnil (L);
        lua_pushfstring (L, "%s", strerror(errno));
		return 2;
	}
	lua_pushboolean (L, 1);
	return 1;
}

/*
** Removes a directory.
** @param #1 Directory path.
*/
static int remove_dir (lua_State *L) {
	const char *path = luaL_checkstring (L, 1);
	int fail;

	fail = yaffs_rmdir (path);

	if (fail) {
		lua_pushnil (L);
		lua_pushfstring (L, "%s", strerror(errno));
		return 2;
	}
	lua_pushboolean (L, 1);
	return 1;
}

/*
** Directory iterator
*/
static int dir_iter (lua_State *L) {
	struct yaffs_dirent *entry;
	
	dir_data *d = (dir_data *)luaL_checkudata (L, 1, DIR_METATABLE);
	luaL_argcheck (L, d->closed == 0, 1, "closed directory");

	if ((entry = yaffs_readdir (d->dir)) != NULL) {
		lua_pushstring (L, entry->d_name);
		return 1;
	} else {
		/* no more entries => close directory */
		yaffs_closedir (d->dir);
		d->closed = 1;
		return 0;
	}
}


/*
** Closes directory iterators
*/
static int dir_close (lua_State *L) {
	dir_data *d = (dir_data *)lua_touserdata (L, 1);
	
	if (!d->closed && d->dir) {
		yaffs_closedir (d->dir);
	}

	d->closed = 1;
	return 0;
}


/*
** Factory of directory iterators
*/
static int dir_iter_factory (lua_State *L) {
	const char *path = luaL_checkstring (L, 1);
	dir_data *d;
	lua_pushcfunction (L, dir_iter);
	d = (dir_data *) lua_newuserdata (L, sizeof(dir_data));
	luaL_getmetatable (L, DIR_METATABLE);
	lua_setmetatable (L, -2);
	d->closed = 0;
	d->dir = yaffs_opendir (path);
	if (d->dir == NULL)
          luaL_error (L, "cannot open %s: %s", path, strerror (errno));
	return 2;
}


/*
** Creates directory metatable.
*/
static int dir_create_meta (lua_State *L) {
	luaL_newmetatable (L, DIR_METATABLE);

        /* Method table */
	lua_newtable(L);
	lua_pushcfunction (L, dir_iter);
	lua_setfield(L, -2, "next");
	lua_pushcfunction (L, dir_close);
	lua_setfield(L, -2, "close");

        /* Metamethods */
	lua_setfield(L, -2, "__index");
	lua_pushcfunction (L, dir_close);
	lua_setfield (L, -2, "__gc");
	return 1;
}


/*
** Convert the inode protection mode to a string.
*/
static const char *mode2string (mode_t mode) {
  if ( S_ISREG(mode) )
    return "file";
  else if ( S_ISDIR(mode) )
    return "directory";
  else if ( S_ISLNK(mode) )
	return "link";
  else if ( S_ISSOCK(mode) )
    return "socket";
  else if ( S_ISFIFO(mode) )
	return "named pipe";
  else if ( S_ISCHR(mode) )
	return "char device";
  else if ( S_ISBLK(mode) )
	return "block device";
  else
	return "other";
}


/* inode protection mode */
static void push_st_mode (lua_State *L, struct yaffs_stat *info) {
	lua_pushstring (L, mode2string (info->st_mode));
}
/* device inode resides on */
static void push_st_dev (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, (lua_Number)info->st_dev);
}
/* inode's number */
static void push_st_ino (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, (lua_Number)info->st_ino);
}
/* number of hard links to the file */
static void push_st_nlink (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, (lua_Number)info->st_nlink);
}
/* user-id of owner */
static void push_st_uid (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, (lua_Number)info->st_uid);
}
/* group-id of owner */
static void push_st_gid (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, (lua_Number)info->st_gid);
}
/* device type, for special file inode */
static void push_st_rdev (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, (lua_Number)info->st_rdev);
}
/* time of last access */
static void push_st_atime (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, info->yst_atime);
}
/* time of last data modification */
static void push_st_mtime (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, info->yst_mtime);
}
/* time of last file status change */
static void push_st_ctime (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, info->yst_ctime);
}
/* file size, in bytes */
static void push_st_size (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, (lua_Number)info->st_size);
}

/* blocks allocated for file */
static void push_st_blocks (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, (lua_Number)info->st_blocks);
}
/* optimal file system I/O blocksize */
static void push_st_blksize (lua_State *L, struct yaffs_stat *info) {
	lua_pushnumber (L, (lua_Number)info->st_blksize);
}
static void push_invalid (lua_State *L, struct yaffs_stat *info) {
  luaL_error(L, "invalid attribute name");
  info->st_blksize = 0; /* never reached */
}

 /*
** Convert the inode protection mode to a permission list.
*/
static const char *perm2string (mode_t mode) {
  static char perms[10] = "---------\0";
  int i;
  for (i=0;i<9;i++) perms[i]='-';
  if (mode & S_IRUSR) perms[0] = 'r';
  if (mode & S_IWUSR) perms[1] = 'w';
  if (mode & S_IXUSR) perms[2] = 'x';
  if (mode & S_IRGRP) perms[3] = 'r';
  if (mode & S_IWGRP) perms[4] = 'w';
  if (mode & S_IXGRP) perms[5] = 'x';
  if (mode & S_IROTH) perms[6] = 'r';
  if (mode & S_IWOTH) perms[7] = 'w';
  if (mode & S_IXOTH) perms[8] = 'x';
  return perms;
}

/* permssions string */
static void push_st_perm (lua_State *L, struct yaffs_stat *info) {
    lua_pushstring (L, perm2string (info->st_mode));
}

typedef void (*_push_function) (lua_State *L, struct yaffs_stat *info);

struct _stat_members {
	const char *name;
	_push_function push;
};

struct _stat_members members[] = {
	{ "mode",         push_st_mode },
	{ "dev",          push_st_dev },
	{ "ino",          push_st_ino },
	{ "nlink",        push_st_nlink },
	{ "uid",          push_st_uid },
	{ "gid",          push_st_gid },
	{ "rdev",         push_st_rdev },
	{ "access",       push_st_atime },
	{ "modification", push_st_mtime },
	{ "change",       push_st_ctime },
	{ "size",         push_st_size },
 	{ "permissions",  push_st_perm },
	{ "blocks",       push_st_blocks },
	{ "blksize",      push_st_blksize },
	{ NULL, push_invalid }
};

/*
** Get file or symbolic link information
*/
static int _file_info_ (lua_State *L, int (*st)(const char*, struct yaffs_stat*)) {
	int i;
	struct yaffs_stat info;
	const char *file = luaL_checkstring (L, 1);

	if (st(file, &info)) {
		lua_pushnil (L);
		lua_pushfstring (L, "cannot obtain information from file `%s'", file);
		return 2;
	}
	if (lua_isstring (L, 2)) {
		int v;
		const char *member = lua_tostring (L, 2);
		if (strcmp (member, "mode") == 0) v = 0;
		else if (strcmp (member, "blocks")  == 0) v = 11;
		else if (strcmp (member, "blksize") == 0) v = 12;
		else /* look for member */
			for (v = 1; members[v].name; v++)
				if (*members[v].name == *member)
					break;
		/* push member value and return */
		members[v].push (L, &info);
		return 1;
	} else if (!lua_istable (L, 2))
		/* creates a table if none is given */
		lua_newtable (L);
	/* stores all members in table on top of the stack */
	for (i = 0; members[i].name; i++) {
		lua_pushstring (L, members[i].name);
		members[i].push (L, &info);
		lua_rawset (L, -3);
	}
	return 1;
}


/*
** Get file information using stat.
*/
static int file_info (lua_State *L) {
	return _file_info_ (L, yaffs_stat);
}


/*
** Get symbolic link information using lstat.
*/
static int link_info (lua_State *L) {
	return _file_info_ (L, yaffs_lstat);
}


/*
** Assumes the table is on top of the stack.
*/
static void set_info (lua_State *L) {
	lua_pushliteral (L, "_COPYRIGHT");
	lua_pushliteral (L, "Copyright (C) 2003-2009 Kepler Project");
	lua_settable (L, -3);
	lua_pushliteral (L, "_DESCRIPTION");
	lua_pushliteral (L, "LuaFileSystem is a Lua library developed to complement the set of functions related to file systems offered by the standard Lua distribution");
	lua_settable (L, -3);
	lua_pushliteral (L, "_VERSION");
	lua_pushliteral (L, "LuaFileSystem 1.5.0-misp");
	lua_settable (L, -3);
}


static const struct luaL_Reg fslib[] = {
	{"attributes", file_info},
	{"dir", dir_iter_factory},
        {"link", make_link},
	{"mkdir", make_dir},
	{"rmdir", remove_dir},
	{"symlinkattributes", link_info},
	{NULL, NULL},
};

int luaopen_lfs (lua_State *L) {
	dir_create_meta (L);
	luaL_register (L, "lfs", fslib);
	set_info (L);
	return 1;
}
