#include <stdio.h>
#include <string.h>

#include <irq.h>
#include <uart.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

void free(void *ptr)
{
	printf("don't call me!\n");
	while(1);
}

void *realloc(void *ptr, size_t size)
{
	printf("don't call me!\n");
	while(1);
}

static void *stupid_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	static char buffer[1024*1024];
	static int index;
	void *r;
	
	if((nsize % 4) != 0)
		nsize += 4 - (nsize % 4);
	
	if(nsize > 0) {
		if(ptr == NULL) {
			r = &buffer[index];
			index += nsize;
			if(index > sizeof(buffer)) {
				printf("alloc failed\n");
				return NULL;
			}
			return r;
		} else {
			r = stupid_alloc(NULL, NULL, 0, nsize);
			if(r == NULL) return NULL;
			memcpy(r, ptr, osize);
			return r;
		}
	} else
		return NULL;
}

static void test_lua(void)
{
	lua_State *L;
	
	L = lua_newstate(stupid_alloc, NULL); // TODO: use luaL_newstate from lauxlib
	luaopen_base(L); // TODO: consider luaL_openlibs from linit
	luaopen_coroutine(L);
	luaopen_table(L);
	luaL_dostring(L, "for i=10,1,-1 do print(i) end\n");
	
	lua_close(L);
}

int main()
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();
	
	printf("Hello World\n");
	test_lua();
	
	while(1);
	
	return 0;
}
