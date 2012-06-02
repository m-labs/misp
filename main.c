#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <irq.h>
#include <uart.h>

#include <glue.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static void test_lua(void)
{
	lua_State *L;
	
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dostring(L, "for i=10,1,-1 do print(i) end\n");
	luaL_dostring(L, "function attrdir (path)\n"
"    for file in lfs.dir(path) do\n"
"        if file ~= \".\" and file ~= \"..\" then\n"
"            local f = path..'/'..file\n"
"            print (\"\t \"..f)\n"
"            local attr = lfs.attributes (f)\n"
"            assert (type(attr) == \"table\")\n"
"            if attr.mode == \"directory\" then\n"
"                attrdir (f)\n"
"            else\n"
"                for name, value in pairs(attr) do\n"
"                    print (name, value)\n"
"                end\n"
"            end\n"
"        end\n"
"    end\n"
"end\n"
"\n"
"attrdir (\"/\")\n");
	lua_close(L);
}

extern void *_heapstart;

int main()
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();
	mm_initialize(&_heapstart, 64*1024*1024);
	fs_init();
	
	printf("Hello World\n");
	test_lua();
	
	while(1);
	
	return 0;
}
