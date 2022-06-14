#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "os_mem.h"
int main()
{
    memory_manager_t mm;
    setup_memory_manager(&mm);

    mm.create(10, 1) != 0;
    mm.create_object("o9");
    mm.create_object("o1");
    mm.create_object("o2");
    mm.create_object("o3");
    mm.create_object("o4");
    mm.create_object("o5");
    mm.create_object("o6");
    mm.create_object("o7");
    mm.create_object("o8");
    mm.create_object("o0");
    mm.link("o1", "1234567");
    mm.link("o1", "o3");
    mm.link("o4", "o8");
    mm.link("o4", "o1");
    mm.destroy_object("o1");
    mm.link("o1", "o4");
    mm.link("o2", "o4");
    mm.link("o4", "o2");
    mm.link("o4", "o6");
    mm.link("o5", "o6");
    mm.link("o7", "o1");
    mm.link("o4", "o8");
    mm.print_objects();
    mm.collect_dead_links();
    mm.destroy_object("o6");
    mm.collect_dead_links();
    mm.destroy_object("o2");
    mm.destroy_object("o5");
    mm.print_objects();
    mm.collect_dead_links();
    mm.destroy();

    return 0;
}
