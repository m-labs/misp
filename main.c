#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <irq.h>
#include <uart.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static void test_lua(void)
{
	lua_State *L;
	
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dostring(L, "for i=10,1,-1 do print(i) end\n");
	lua_close(L);
}

extern void *_heapstart;

int main()
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();
	mm_initialize(&_heapstart, 64*1024*1024);
	
	printf("Hello World\n");
	test_lua();
	
	while(1);
	
	return 0;
}
