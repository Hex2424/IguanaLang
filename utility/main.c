#include "separator/separator.h"


int main()
{
    const char word[] = "int:32 main() {}";
    Separator_getSeparatedWords(word, sizeof(word));




    return 0;
}