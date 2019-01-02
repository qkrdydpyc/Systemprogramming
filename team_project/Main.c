#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>

#define MY_LED_NUM 100
#define MY_LED_WRITE _IOW( MY_LED_NUM, 1, int )

#define MY_SERVO_NUMBER 100
#define MY_SERVO_WRITE _IOW ( MY_SERVO_NUMBER, 1, int )

#define MY_SENSOR 100
#define MY_SENSOR_READ _IOR( MY_SENSOR, 0, int )

int gas_val;

void servo_write(int fd, int value){
    char buf[3];
    sprintf(buf, "%d", value);
    ioctl(fd, MY_SERVO_WRITE, buf);
}

void led_write(int fd, char RGB, int value){
    char buf[3];
    buf[0] = RGB; buf[1] = value ? '1' : '0';
    ioctl(fd, MY_LED_WRITE, buf);
}

int sensor_read(int fd){
    char buf[3];
    ioctl(fd, MY_SENSOR_READ, buf);
    return buf[0] == '1';
}

int main(int argc, char **argv){
    int servo_fd = open("/dev/servo_dev", O_RDWR);
    int servo_fd2 = open("/dev/servo_dev2", O_RDWR);
    int led_fd = open("/dev/led_dev", O_RDWR);

    int touch_fd = open("/dev/tsensor_dev", O_RDWR);
    int rain_fd = open("/dev/rain_dev", O_RDWR);
    int light_fd = open("/dev/light_dev", O_RDWR);

    int window = 0;
    int settedwindow = 0;

    int curtain = 0;
    int settedcurtain = 0;
    
    int rs = 0;
    int gs = 0;
    int bs = 0;
    if(argc > 1)
        gas_val = argv[1][0] == '1';

    while(1){
        sleep(1);
        if(window != settedwindow){
            if(window){
                printf("asdf\n");
                servo_write(servo_fd, 1);
                sleep(1);
                servo_write(servo_fd, 0);
            }
            else{
                printf("asdf2\n");
                servo_write(servo_fd, 2);
                sleep(1);
                servo_write(servo_fd, 0);
            }
            settedwindow = window;
        }
        if(curtain != settedcurtain){
            if(curtain){
                printf("asdf3\n");
                servo_write(servo_fd2, 1);
                sleep(1);
                servo_write(servo_fd2, 0);
            }
            else{
                printf("asdf4\n");
                servo_write(servo_fd2, 2);
                sleep(1);
                servo_write(servo_fd2, 0);
            }
            settedcurtain = curtain;
        }
        int touch_val = sensor_read(touch_fd);
        int light_val = !sensor_read(light_fd);
        int rain_val = !sensor_read(rain_fd);

        if((!rain_val && !gas_val && !light_val) || (!touch_val && !rain_val && !gas_val && light_val)){
            window = 1;
        }
        else window = 0;

        if((touch_val && !light_val) || !touch_val){
            curtain = 1;
        }
        else curtain = 0;

        led_write(led_fd, 'B', rain_val);
        led_write(led_fd, 'G', gas_val);
        printf("%d %d\n", window, curtain);
        printf("%d %d %d %d\n", touch_val, rain_val, gas_val, light_val);
    }
    
    return 0;
}