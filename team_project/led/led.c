#include<linux/gpio.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/slab.h>

#define MY_LED_NUM 100
#define MY_LED_WRITE _IOW( MY_LED_NUM, 1, int )

MODULE_LICENSE("GPL");

struct cdev my_cdev;
dev_t devno;
int major;
char *msg = NULL;

int rpin = 16;
int gpin = 20;
int bpin = 21;

int led_dev_open(struct inode *pinode, struct file *pfile){
    printk("open led drv\n");
    return 0;
}

int led_dev_close(struct inode *pinode, struct file *pfile){
    printk("close led drv\n");
    return 0;
}

long led_dev_write(struct file *pfile, unsigned int command, unsigned long arg){
    if(command == MY_LED_WRITE){
        memset(msg, 0, 32);
        if(copy_from_user((void *)msg, (const void *)arg, (ssize_t)strlen((char *)arg))){
            printk("Write err\n");
            return -1;
        }
        printk("copy from user: %s\n", msg);
        int setPin = -1;
        if(msg[0] == 'R')setPin = rpin;
        else if(msg[0] == 'G')setPin = gpin;
        else if(msg[0] == 'B')setPin = bpin;
        else{
            printk("input str err\n");
            return -1;
        }
        gpio_set_value(setPin, msg[1] == '0');
    }
    else{
        printk("read err\n");
    }
    return 0;
}

struct file_operations fop = {
    .owner = THIS_MODULE,
    .open = led_dev_open,
    .unlocked_ioctl = led_dev_write,
    .release = led_dev_close,
};

int __init led_init(void){
    printk("init led drv\n");
    
    alloc_chrdev_region(&devno, 0, 2, "led_dev");
    major = MAJOR(devno);

    cdev_init(&my_cdev, &fop);
    my_cdev.owner = THIS_MODULE;

    msg = (char *)kmalloc(32, GFP_KERNEL);
    printk("MAJOR: %d\n", MAJOR(devno));
    
    if(cdev_add(&my_cdev, devno, 1) < 0){
        printk("Device add err\n");
        return -1;
    }

    if(gpio_request(rpin, "rpin") < 0) return !printk("gpio request fail (r)\n");
    if(gpio_request(gpin, "gpin") < 0) return !printk("gpio request fail (g)\n");
    if(gpio_request(bpin, "bpin") < 0) return !printk("gpio request fail (b)\n");

    gpio_direction_output(rpin, 1);
    gpio_direction_output(gpin, 1);
    gpio_direction_output(bpin, 1);

    return 0;
}

void __exit led_exit(void){
    printk("EXIT led module\n");
    if(msg) kfree(msg);
    gpio_free(rpin);
    gpio_free(gpin);
    gpio_free(bpin);
    cdev_del(&my_cdev);
}

module_init(led_init);
module_exit(led_exit);
