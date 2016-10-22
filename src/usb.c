//
// Created by greg on 12/10/16.
//

#include <usb.h>
#include <string.h>
#include <stdio.h>

void    pam_usb_init()
{
    usb_init();
    usb_find_busses();
    usb_find_devices();
    static struct usb_bus *bus;
    static struct usb_device *dev;
}

uint16_t find_sn_from_filename(const char *filename)
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for (bus = usb_busses; bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (strcmp(filename, dev->filename) == 0) {
                return dev->descriptor.iSerialNumber;
            }
        }
    }
    return 0;
}

void    show_all_usb()
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for (bus = usb_busses; bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            printf("%04x:%04x\n", dev->descriptor.idVendor, dev->descriptor.idProduct);
        }
    }
}

