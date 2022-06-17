#include <stdio.h>
#include "fixture.h"

int main()
{
    bool ok1 = is_insert_head_const();
    bool ok2 = is_insert_tail_const();
    bool ok3 = is_remove_head_const();
    bool ok4 = is_remove_tail_const();
    printf("%d %d %d %d", ok1, ok2, ok3, ok4);

    return 0;
}