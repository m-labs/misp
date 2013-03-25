#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <irq.h>
#include <uart.h>

#include <glue.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

//#define TEST_BASIC
//#define TEST_DIR
#define TEST_FILE

static void test_lua(void)
{
	lua_State *L;
	
	L = luaL_newstate();
	luaL_openlibs(L);
#ifdef TEST_BASIC
	luaL_dostring(L, "for i=10,1,-1 do print(i) end\n");
#endif
#ifdef TEST_DIR
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
#endif
#ifdef TEST_FILE
	luaL_dostring(L, "f = assert(io.open(\"/patchpool/nil - Cid and Lucy.fnp\"))\n"
		"t = f:read(\"*all\")\n"
		"print(t)\n"
		"f:close()\n");
#endif
	lua_close(L);
}

extern void *_heapstart;

extern void agg_test(void);

int main()
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();
	mm_initialize(&_heapstart, 64*1024*1024);
	fs_init();
	
	printf("Hello World\n");
	test_lua();
	agg_test();
	
	while(1);
	
	return 0;
}
