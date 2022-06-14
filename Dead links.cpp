#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "os_mem.h"

struct object
{                  //основной объект с ктр имеем дело
    char name[32]; //длина имени максимум 32 символа
    int type;      //  1 - живой объект, 0 - мертвый, 2 - рут
    int links[16]; //количество ссылок на другие объекты макс 16
                   //ссылками будут являться индексы в массиве (нужно быть внимательным при перестановках)
};

int counter = -1; // счетчик количества объектвов (-1 = памяти нет)
int max_size;
object *array;

int create(int size, int num_pages)
{ //создаем массив где храним наши объекты
    if (counter == -1)
    {
        max_size = size;
        array = (object *)malloc(++counter * sizeof(object));
        return 1;
    }
    return 0;
}

int destroy()
{ //уничтожаеи нашу память
    if (counter != -1)
    {
        counter = -1;
        free(array);
        return 1;
    }
    return 0;
}

int create_object(const char *name)
{ //создание объекта
    if (counter >= 0 && counter < max_size)
    {
        //если память сущ и не достигнут предел
        for (int i = 0; i < counter; ++i) //проверка на наличие имени
        {
            if (!strcmp(array[i].name, name)) //если уже сущ с таким именем
            {
                return 0;
            }
        }

        array = (object *)realloc(array, ++counter * sizeof(object)); //выделяем память под новый объект
        strcpy(array[counter - 1].name, name);                        //записываем в конец
        array[counter - 1].type = 0;                                  //мертворождение
        for (int &link : array[counter - 1].links)                    // Range-based for loop // ОДИЛ ака
        {
            link = -1;
        } //без ссылок
        return 1;
    }
    return 0;
}
int destroy_object(const char *name)
{
    for (int i = 0; i < counter; ++i)
    {
        if (!strcmp(array[i].name, name))
        { // нашли нужное имя
            for (int k = i + 1; k < counter; ++k)
                array[k - 1] = array[k]; //сдвигаем на место удаленного

            for (int j = 0; j < counter; ++j)    //удаляем ссылку на удаленный файл
                for (int &link : array[j].links) //и увеличиваем значение ссылок на объекты после удаленного
                {
                    if (link == i)
                        link = -2;
                    else if (link > i)
                        link--;
                }
            array = (object *)realloc(array, --counter * sizeof(object)); //уменьшаем массив

            return 1;
        }
    }
    return 0;
}

int set_root(const char *name)
{ //сделать объект корневым
    if (counter >= 0)
        for (int i = 0; i < counter; ++i)
            if (!strcmp(array[i].name, name) && array[i].type != 2)
            { //найдя искомое имя, делаем его корневым
                array[i].type = 2;
                return 1;
            }
    return 0;
}

int link(const char *object1_name, const char *object2_name)
{
    for (int i = 0; i < counter; ++i)
        if (!strcmp(array[i].name, object1_name)) //находим первый объект
            for (int k = 0; k < counter; ++k)
                if (!strcmp(array[k].name, object2_name)) //находим второй объект
                    for (int &link : array[i].links)      //создаем ссылку // Odil aka
                    {
                        if (link == -1)
                        {
                            link = k;
                            return 1;
                        }
                    }
    return 0;
}

void sort()
{
    object temp{}; //временный объект
    for (int i = 0; i < counter - 1; i++)
    { //сортировка пузырьком
        for (int k = i; k < counter; k++)
        {
            if (strcmp(array[i].name, array[k].name) > 0)
            { //если первый больше второго
                temp = array[i];
                array[i] = array[k];
                array[k] = temp;

                for (int p = 0; p < counter; ++p)
                    for (int &link : array[p].links)
                    { //тажке меняем ссылки. из-за сортировки, расположение объектов изменилось
                        if (link == i)
                            link = k;
                        else if (link == k)
                            link = i;
                    }
            }
        }
    }
}

void print_objects()
{ //сортируем и печатаем
    if (counter >= 0)
    {
        sort();
        for (int i = 0; i < counter; ++i)
            printf("%s \n", array[i].name);
    }
    printf("\n");
}

void collect_dead_links()
{ //сортируем, отмечаем живые и печатаем только живые
    if (counter >= 0)
    {
        for (int i = 0; i < counter; ++i)
        {
            int j = 0;
            for (int &link : array[i].links)
            {
                if (link == -2)
                {
                    printf("%s, %d\n", array[i].name, j);
                }
                j++;
            }
        }
    }
    printf("\n");
}
void setup_memory_manager(memory_manager_t *mm)
{
    mm->create = create;
    mm->destroy = destroy;
    mm->create_object = create_object;
    mm->destroy_object = destroy_object;
    mm->print_objects = print_objects;
    mm->set_root = set_root;
    mm->link = link;
    mm->collect_dead_links = collect_dead_links;
}

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
