#include "Save.h"

#include <raylib.h>

namespace
{
    const char* SaveFile = "save.txt";
}
int Save::LoadBestZel()
{
    if (!FileExists(SaveFile)) return 0;

    char* text = LoadFileText(SaveFile);
    if (text == nullptr) return 0;

    const int best = TextToInteger(text);
    UnloadFileText(text);
    return best;
}

void Save::SaveBestZel(int zel)
{
    SaveFileText(SaveFile, TextFormat("%i", zel));
}
