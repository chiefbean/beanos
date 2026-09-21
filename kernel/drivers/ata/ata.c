#include <sys/io.h>
#include <kernel/ata.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uintptr_t ata_identify() {
    outb(0x1F6, 0xA0);
    outb(0x1F2, 0x00);
    outb(0x1F3, 0x00);
    outb(0x1F4, 0x00);
    outb(0x1F5, 0x00);
    outb(0x1F7, 0xEC);
    
    for(size_t i = 0; i < 4; i++) inb(0x3F6);
    uint8_t status = inb(0x1F7);

    if (status == 0) {
        printf("\nfailed to find drive");
        return 0;
    } else if (status == 0xFF) {
        printf("\nfloating bus identified, bailing");
        return 0;
    }

    while(status & 0x80) {
        status = inb(0x1F7);
    }

    uint8_t LBAmid = inb(0x1F4);
    uint8_t LBAhi = inb(0x1F5);

    if (LBAmid != 0 || LBAhi != 0) {
        printf("\ndrive is not ATA");
        return 0;
    }

    status = inb(0x1F7);
    while(!(status & (1 << 3)) && !(status & 1)) {
        status = inb(0x1F7);
    }

    if (status & 1) {
        printf("\nerror polling ATA");
        return 0;
    }

    uint16_t data[256];

    for(size_t i = 0; i < 256; i++) {
        uint16_t data_p = inw(0x1F0);
        data[i] = data_p;
    }

    uint32_t sectors = data[60] | ((uint32_t) data[61] << 16);
    char model[41];
    for (size_t i = 27; i < 47; i++) {
        model[2*(i-27)] = (char) (data[i] >> 8) & 0xFF;
        model[2*(i-27)+1] = (char) (data[i] & 0xFF);
    }
    model[40] = '\0';

    printf("\n%s: %x", model, sectors);
    ata_drive_t* drive = malloc(sizeof(ata_drive_t));
    drive->present = 1;
    drive->sectors = sectors;
    memcpy(drive->model, model, 41);

    return (uintptr_t) drive;
}

char* ata_read_sector(uint32_t lba, ata_drive_t* drive, char* buf) {
    if (lba >= drive->sectors) {
        printf("\nAttempted read of LBA larger than sectors");
        return NULL;
    }

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);

    outb(0x1F3, (unsigned char) lba);
    outb(0x1F4, (unsigned char) (lba >> 8));
    outb(0x1F5, (unsigned char) (lba >> 16));
    
    outb(0x1F7, 0x20);

    for(size_t i = 0; i < 4; i++) inb(0x3F6);
    uint8_t status = inb(0x1F7);

    if (status == 0) {
        printf("\nfailed to find drive");
        return 0;
    } else if (status == 0xFF) {
        printf("\nfloating bus identified, bailing");
        return 0;
    }

    while(status & 0x80) {
        status = inb(0x1F7);
    }

    status = inb(0x1F7);
    while(!(status & (1 << 3)) && !(status & 1)) {
        status = inb(0x1F7);
    }

    if (status & 1) {
        printf("\nerror polling ATA");
        return 0;
    }

    for(size_t i = 0; i < 256; i++) {
        uint16_t data = inw(0x1F0);
        buf[2*i] = (char) (data & 0xFF);
        buf[2*i+1] = (char) (data >> 8) & 0xFF;
    }
    
    return buf;
}

void* ata_write_sector(uint32_t lba, ata_drive_t* drive, char* buf) {
    if (lba >= drive->sectors) {
        printf("\nAttempted write of LBA larger than sectors");
        return NULL;
    }

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);

    outb(0x1F3, (unsigned char) lba);
    outb(0x1F4, (unsigned char) (lba >> 8));
    outb(0x1F5, (unsigned char) (lba >> 16));
    
    outb(0x1F7, 0x30);

    for(size_t i = 0; i < 4; i++) inb(0x3F6);
    uint8_t status = inb(0x1F7);

    if (status == 0) {
        printf("\nfailed to find drive");
        return (void*) 0;
    } else if (status == 0xFF) {
        printf("\nfloating bus identified, bailing");
        return (void*) 0;
    }

    while(status & 0x80) {
        status = inb(0x1F7);
    }

    status = inb(0x1F7);
    while(!(status & (1 << 3)) && !(status & 1)) {
        status = inb(0x1F7);
    }

    if (status & 1) {
        printf("\nerror polling ATA");
        return (void*) 0;
    }

    for(size_t i = 0; i < 256; i++) {
        uint16_t word = (uint8_t) buf[2*i] | ((uint8_t) buf[2*i+1] << 8);
        outw(0x1F0, word);
    }

    outb(0x1F7, 0xE7);
    status = inb(0x1F7);
    while(status & 0x80) status = inb(0x1F7);

    return (void*) 1;
}