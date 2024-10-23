#include <welcome.h>
#include <syscaller.h>
#include <font.h>

void welcome()
{
    fontSizeDownCaller(UNUSED);
    printRectangleCaller(UNUSED, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK); // set screen black
    uint16_t startX = 156;
    int startY = 160;
    uint32_t t = 0;

    for (uint32_t i = 0; i < PENGUIN_HEIGHT + 6; i++)
    {
        for (uint32_t j = 0; j < PENGUIN_WIDTH + 6; j++)
        {
            if (i < 3 || i >= PENGUIN_HEIGHT + 3 || j < 3 || j >= PENGUIN_WIDTH + 3)
            {
                printRectangleCaller(UNUSED, j * 5 + startX, i * 5 + startY, 5, 5, WHITE);
            }
            else
            {
                uint32_t color = penguin[t++];
                printRectangleCaller(UNUSED, j * 5 + startX, i * 5 + startY, 5, 5, color);
            }
        }
    }

    printRectangleCaller(UNUSED, 225, 60, 600, 105, WHITE);
}
