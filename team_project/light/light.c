#include<linux/gpio.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/slab.h>

#define MY_SENSOR 100
#define MY_SENSOR_READ _IOR( MY_SENSOR, 0, int )

MODULE_LICENSE("GPL");

struct cdev my_cdev;
dev_t devno;
int major;
int pin = 13;

int light_dev_open(struct inode *pinode, struct file *pfile){
    printk("Open light drv\n");
    return 0;
}

int light_dev_close(struct inode *pinode, struct file *pfile){
    printk("Close light drv\n");
    return 0;
}

long light_dev_write(struct file *pfile, unsigned int command, unsigned long arg){
    if(command == MY_SENSOR_READ){
        char buf[50]; sprintf(buf, "%d", gpio_get_value(pin));
        printk("ret : %s\n", buf);
        copy_to_user((void __user *)arg, (const void *)buf, strlen(buf) + 1);
    }
    return 0;
}

struct file_operations fop = {
    .owner = THIS_MODULE,
    .open = light_dev_open,
    .unlocked_ioctl = light_dev_write,
    .release = light_dev_close,
};

int __init light_init(void){
    printk("INIT light moter\n");

    alloc_chrdev_region(&devno, 0, 2, "light_dev");
    major = MAJOR(devno);
    
    cdev_init(&my_cdev, &fop);
    my_cdev.owner = THIS_MODULE;

    printk("MAJOR: %d\n", MAJOR(devno));

    if(cdev_add(&my_cdev, devno, 1) < 0){
        printk("Device add err\n");
        return -1;
    }

    if(gpio_request(pin, "light_PIN") < 0){
        printk("gpio_request fail\n");
    }

    if(gpio_direction_input(pin) < 0){
        printk("gpio_direction_output fail\n");
    }
    return 0;
}

void __exit light_exit(void){
    printk("EXIT light module\n");
    gpio_free(pin);
    cdev_del(&my_cdev);
}

module_init(light_init);
module_exit(light_exit);
