#include <stdio.h>
#include "makeg.tab.h"

extern int cond_count, target_count, init_count, lines_count, comments_count;
int full_info = 0;
extern FILE *yyin;

int main(int argc, char *argv[])
{
    // printf("%s\n", argv[1]);
    if (argc == 2)
        yydebug = 0;
    else if (*argv[2] == '1')
        yydebug = 1;
    else if (*argv[2] == '2')
        full_info = 1;
    yyin = fopen(argv[1], "r");
    if (yyin == 0)
    {
        printf("Cannot open file\n");
        return 0;
    }
    yyparse();
    printf("Statistics:\n\ttargets: %i\n\tconditions: %i\n\tvariables: %i\n\tlines: %i\n\tcomments: %i\n",
           target_count,
           cond_count,
           init_count,
           lines_count,
           comments_count);

    fclose(yyin);
    return 0;
}
