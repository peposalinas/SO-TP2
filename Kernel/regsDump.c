#include <regsDump.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <interrupts.h>

static char *registers[19] = {
    "RAX:",
    "RBX:",
    "RCX:",
    "RDX:",
    "RSI:",
    "RDI:",
    "RSP:",
    "RBP:",
    "R8 :",
    "R9 :",
    "R10:",
    "R11:",
    "R12:",
    "R13:",
    "R14:",
    "R15:",
    "RIP:",
    "CS :",
    "RFLAGS:",
};

// Prototipo de la función
void uint64ToHex(uint64_t num, char buf[17]);

void printRegisters(const char *msg, const uint64_t regs[19])
{
    savePopUpWindow();
    clearPopUp();
    printInPopUp((char *)msg, 0, 0); // Cast para evitar el warning de const

    char buf[19]; // Cambiar uint8_t por char para compatibilidad con printInPopUp
    buf[0] = '0';
    buf[1] = 'x';
    for (uint8_t i = 0; i < 18; i++)
    {
        printInPopUp(registers[i], (i % 2) * 24, i / 2 + 1);
        uint64ToHex(regs[i], buf + 2);
        printInPopUp(buf, (i % 2) * 24 + 5, i / 2 + 1);
    }
    printInPopUp(registers[18], 0, 10);
    uint64ToHex(regs[18], buf + 2);
    printInPopUp(buf, 8, 10);
    printInPopUp("Press ESC to continue", 0, 11);

    while (getKeyboardKey() != ESC_MAKE_CODE)
        ;

    resetPopUp();
}

void uint64ToHex(uint64_t num, char buf[17])
{
    int i = 15;
    while (i >= 0)
    {
        uint32_t digit = num % 16;
        buf[i] = (digit < 10 ? '0' : ('A' - 10)) + digit;
        num /= 16;
        i--;
    }
    buf[16] = '\0';
}