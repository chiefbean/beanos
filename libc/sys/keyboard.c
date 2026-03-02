#include <sys/keyboard.h>
#include <stdio.h>
#include <sys/isr.h>

uint8_t shift = 0;

static void keyboard_callback(registers_t regs)
{
    uint8_t scancode = inb(0x60);
    if (scancode == 0x2A || scancode == 0x36 || scancode == 0xAA || scancode == 0xB6 || scancode == 0x3A) {
        shift ^= 1;
        return;
    }

    if (scancode == 0x0E) {
        keybuffer.tail--;
        keybuffer.head--;
    }

    if (scancode < 0x59) {
        char key = scancode_table[scancode];
        if (shift) key = shift_scancode_table[scancode];
        else scancode_table[scancode];
        keybuffer.buf[keybuffer.head++] = key;
    }
}

char keyboard_getchar() {
    if (keybuffer.head == keybuffer.tail) return '\0';
    return keybuffer.buf[keybuffer.tail++];
}

void init_keyboard()
{
    register_interrupt_handler(33, &keyboard_callback);

    outb(0x60, 0xF4);
    uint8_t scancode = inb(0x60);
    int count = 0;
    while(scancode != 0xfa || count >= 3) {
        outb(0x60, 0xF4);
        scancode = inb(0x60);
        count++;
    }

    if (count >= 3) {
        printf("Failed to initialize keyboard\n");
        __asm__ __volatile__("int $16");
    }
    init_scancodes();

    keybuffer.tail = 0;
    keybuffer.head = 0;

    printf("Keyboard initialized\n");
} 

void init_scancodes() {
    //scancode_table[0x01] = ''; //escape
    scancode_table[0x02] = '1';
    scancode_table[0x03] = '2';
    scancode_table[0x04] = '3';
    scancode_table[0x05] = '4';
    scancode_table[0x06] = '5';
    scancode_table[0x07] = '6';
    scancode_table[0x08] = '7';
    scancode_table[0x09] = '8';
    scancode_table[0x0A] = '9';
    scancode_table[0x0B] = '0';
    scancode_table[0x0C] = '-';
    scancode_table[0x0D] = '=';
    scancode_table[0x0E] = '\b'; //backspace
    scancode_table[0x0F] = '\t'; //tab
    scancode_table[0x10] = 'q';
    scancode_table[0x11] = 'w';
    scancode_table[0x12] = 'e';
    scancode_table[0x13] = 'r';
    scancode_table[0x14] = 't';
    scancode_table[0x15] = 'y';
    scancode_table[0x16] = 'u';
    scancode_table[0x17] = 'i';
    scancode_table[0x18] = 'o';
    scancode_table[0x19] = 'p';
    scancode_table[0x1A] = '[';
    scancode_table[0x1B] = ']';
    scancode_table[0x1C] = '\n';
    // scancode_table[0x1D] = ''; //left control
    scancode_table[0x1E] = 'a';
    scancode_table[0x1F] = 's';
    scancode_table[0x20] = 'd';
    scancode_table[0x21] = 'f';
    scancode_table[0x22] = 'g';
    scancode_table[0x23] = 'h';
    scancode_table[0x24] = 'j';
    scancode_table[0x25] = 'k';
    scancode_table[0x26] = 'l';
    scancode_table[0x27] = ';';
    scancode_table[0x28] = '\'';
    scancode_table[0x29] = '`';
    // scancode_table[0x2A] = ''; //left shift
    scancode_table[0x2B] = '\\';
    scancode_table[0x2C] = 'z';
    scancode_table[0x2D] = 'x';
    scancode_table[0x2E] = 'c';
    scancode_table[0x2F] = 'v';
    scancode_table[0x30] = 'b';
    scancode_table[0x31] = 'n';
    scancode_table[0x32] = 'm';
    scancode_table[0x33] = ',';
    scancode_table[0x34] = '.';
    scancode_table[0x35] = '/';
    // scancode_table[0x36] = ''; //right shift
    scancode_table[0x37] = '*';
    // scancode_table[0x38] = ''; //left alt
    scancode_table[0x39] = ' ';
    // scancode_table[0x3A] = ''; //caps lock
    // scancode_table[0x3B] = ''; //F1
    // scancode_table[0x3C] = ''; //F2
    // scancode_table[0x3D] = ''; //F3
    // scancode_table[0x3E] = ''; //F4
    // scancode_table[0x3F] = ''; //F5
    // scancode_table[0x40] = ''; //F6
    // scancode_table[0x41] = ''; //F7
    // scancode_table[0x42] = ''; //F8
    // scancode_table[0x43] = ''; //F9
    // scancode_table[0x44] = ''; //F10
    // scancode_table[0x45] = ''; //num lock
    // scancode_table[0x46] = ''; //scroll lock
    scancode_table[0x47] = '7';
    scancode_table[0x48] = '8';
    scancode_table[0x49] = '9';
    scancode_table[0x4A] = '-';
    scancode_table[0x4B] = '4';
    scancode_table[0x4C] = '5';
    scancode_table[0x4D] = '6';
    scancode_table[0x4E] = '+';
    scancode_table[0x4F] = '1';
    scancode_table[0x50] = '2';
    scancode_table[0x51] = '3';
    scancode_table[0x52] = '0';
    scancode_table[0x53] = '.';



    // scancode_table[0x57] = ''; //F11
    // scancode_table[0x58] = ''; //F12

    // init shifted table
    //shift_scancode_table[0x01] = ''; //escape
    shift_scancode_table[0x02] = '!';
    shift_scancode_table[0x03] = '@';
    shift_scancode_table[0x04] = '#';
    shift_scancode_table[0x05] = '$';
    shift_scancode_table[0x06] = '%';
    shift_scancode_table[0x07] = '^';
    shift_scancode_table[0x08] = '&';
    shift_scancode_table[0x09] = '*';
    shift_scancode_table[0x0A] = '(';
    shift_scancode_table[0x0B] = ')';
    shift_scancode_table[0x0C] = '_';
    shift_scancode_table[0x0D] = '+';
    shift_scancode_table[0x0E] = '\b'; //backspace
    shift_scancode_table[0x0F] = '\t'; //tab
    shift_scancode_table[0x10] = 'Q';
    shift_scancode_table[0x11] = 'W';
    shift_scancode_table[0x12] = 'E';
    shift_scancode_table[0x13] = 'R';
    shift_scancode_table[0x14] = 'T';
    shift_scancode_table[0x15] = 'Y';
    shift_scancode_table[0x16] = 'U';
    shift_scancode_table[0x17] = 'I';
    shift_scancode_table[0x18] = 'O';
    shift_scancode_table[0x19] = 'P';
    shift_scancode_table[0x1A] = '{';
    shift_scancode_table[0x1B] = '}';
    shift_scancode_table[0x1C] = '\n';
    // shift_scancode_table[0x1D] = ''; //left control
    shift_scancode_table[0x1E] = 'A';
    shift_scancode_table[0x1F] = 'S';
    shift_scancode_table[0x20] = 'D';
    shift_scancode_table[0x21] = 'F';
    shift_scancode_table[0x22] = 'G';
    shift_scancode_table[0x23] = 'H';
    shift_scancode_table[0x24] = 'J';
    shift_scancode_table[0x25] = 'K';
    shift_scancode_table[0x26] = 'L';
    shift_scancode_table[0x27] = ':';
    shift_scancode_table[0x28] = '"';
    shift_scancode_table[0x29] = '~';
    // shift_scancode_table[0x2A] = ''; //left shift
    shift_scancode_table[0x2B] = '|';
    shift_scancode_table[0x2C] = 'Z';
    shift_scancode_table[0x2D] = 'X';
    shift_scancode_table[0x2E] = 'C';
    shift_scancode_table[0x2F] = 'V';
    shift_scancode_table[0x30] = 'B';
    shift_scancode_table[0x31] = 'N';
    shift_scancode_table[0x32] = 'M';
    shift_scancode_table[0x33] = '<';
    shift_scancode_table[0x34] = '>';
    shift_scancode_table[0x35] = '?';
    // shift_scancode_table[0x36] = ''; //right shift
    shift_scancode_table[0x37] = '*';
    // shift_scancode_table[0x38] = ''; //left alt
    shift_scancode_table[0x39] = ' ';
    // shift_scancode_table[0x3A] = ''; //caps lock
    // shift_scancode_table[0x3B] = ''; //F1
    // shift_scancode_table[0x3C] = ''; //F2
    // shift_scancode_table[0x3D] = ''; //F3
    // shift_scancode_table[0x3E] = ''; //F4
    // shift_scancode_table[0x3F] = ''; //F5
    // shift_scancode_table[0x40] = ''; //F6
    // shift_scancode_table[0x41] = ''; //F7
    // shift_scancode_table[0x42] = ''; //F8
    // shift_scancode_table[0x43] = ''; //F9
    // shift_scancode_table[0x44] = ''; //F10
    // shift_scancode_table[0x45] = ''; //num lock
    // shift_scancode_table[0x46] = ''; //scroll lock
    shift_scancode_table[0x47] = '7';
    shift_scancode_table[0x48] = '8';
    shift_scancode_table[0x49] = '9';
    shift_scancode_table[0x4A] = '-';
    shift_scancode_table[0x4B] = '4';
    shift_scancode_table[0x4C] = '5';
    shift_scancode_table[0x4D] = '6';
    shift_scancode_table[0x4E] = '+';
    shift_scancode_table[0x4F] = '1';
    shift_scancode_table[0x50] = '2';
    shift_scancode_table[0x51] = '3';
    shift_scancode_table[0x52] = '0';
    shift_scancode_table[0x53] = '.';



    // shift_scancode_table[0x57] = ''; //F11
    // shift_scancode_table[0x58] = ''; //F12
}