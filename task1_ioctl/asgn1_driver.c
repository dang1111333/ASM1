#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include "asgn1_ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Le Minh Dang - MSSV");
MODULE_DESCRIPTION("Assignment 1: Character Driver with IOCTL Interface");
MODULE_VERSION("1.0");

#define DEVICE_NAME "asgn1"
#define CLASS_NAME  "asgn1_class"
#define DRIVER_MAJOR 241
#define BUFFER_SIZE 1024

static int major_number = DRIVER_MAJOR;
static char device_buffer[BUFFER_SIZE];
static int buffer_len = 0;
static int open_count = 0;
static int write_count = 0;
static int read_count = 0;
static int last_write_size = 0;
static int current_mode = 1;

static struct class  *asgn1_class  = NULL;
static struct device *asgn1_device = NULL;
static DEFINE_MUTEX(asgn1_mutex);

static int asgn1_open(struct inode *inode, struct file *file) {
    mutex_lock(&asgn1_mutex);
    open_count++;
    mutex_unlock(&asgn1_mutex);
    pr_info("asgn1_driver: Device opened (open_count=%d)\n", open_count);
    return 0;
}

static int asgn1_release(struct inode *inode, struct file *file) {
    pr_info("asgn1_driver: Device closed\n");
    return 0;
}

static ssize_t asgn1_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    int bytes_to_read;
    mutex_lock(&asgn1_mutex);
    if (*offset >= buffer_len) {
        mutex_unlock(&asgn1_mutex);
        return 0;
    }
    bytes_to_read = buffer_len - *offset;
    if (len < bytes_to_read)
        bytes_to_read = len;

    if (copy_to_user(buf, device_buffer + *offset, bytes_to_read)) {
        mutex_unlock(&asgn1_mutex);
        return -EFAULT;
    }
    *offset += bytes_to_read;
    read_count++;
    mutex_unlock(&asgn1_mutex);
    pr_info("asgn1_driver: Read %d bytes (read_count=%d)\n", bytes_to_read, read_count);
    return bytes_to_read;
}

static ssize_t asgn1_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    char *temp_buf;
    size_t copy_len = len;

    if (copy_len >= BUFFER_SIZE - 10)
        copy_len = BUFFER_SIZE - 10;

    temp_buf = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!temp_buf)
        return -ENOMEM;

    if (copy_from_user(temp_buf, buf, copy_len)) {
        kfree(temp_buf);
        return -EFAULT;
    }
    temp_buf[copy_len] = '\0';

    mutex_lock(&asgn1_mutex);
    if (current_mode == 0) {
        buffer_len = snprintf(device_buffer, BUFFER_SIZE, "[KERNEL]%s", temp_buf);
    } else {
        buffer_len = snprintf(device_buffer, BUFFER_SIZE, "%s", temp_buf);
    }
    write_count++;
    last_write_size = buffer_len;
    mutex_unlock(&asgn1_mutex);

    kfree(temp_buf);
    pr_info("asgn1_driver: Written %d bytes, mode=%d\n", buffer_len, current_mode);
    return len;
}

static long asgn1_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int ret = 0;
    mutex_lock(&asgn1_mutex);

    switch (cmd) {
    case ASGN1_RESET_BUFFER:
        memset(device_buffer, 0, BUFFER_SIZE);
        buffer_len = 0;
        write_count = 0;
        read_count = 0;
        last_write_size = 0;
        pr_info("asgn1_driver: [IOCTL] Buffer and counters reset\n");
        break;

    case ASGN1_GET_STATS: {
        struct asgn1_stats stats;
        stats.open_count = open_count;
        stats.write_count = write_count;
        stats.read_count = read_count;
        stats.buffer_len = buffer_len;
        stats.last_write_size = last_write_size;
        if (copy_to_user((struct asgn1_stats __user *)arg, &stats, sizeof(stats))) {
            ret = -EFAULT;
        } else {
            pr_info("asgn1_driver: [IOCTL] GET_STATS dispatched\n");
        }
        break;
    }

    case ASGN1_SET_MODE: {
        int new_mode;
        if (copy_from_user(&new_mode, (int __user *)arg, sizeof(int))) {
            ret = -EFAULT;
        } else if (new_mode != 0 && new_mode != 1) {
            ret = -EINVAL;
        } else {
            current_mode = new_mode;
            pr_info("asgn1_driver: [IOCTL] SET_MODE to %d\n", current_mode);
        }
        break;
    }

    case ASGN1_GET_VERSION: {
        char ver[64] = "asgn1_driver v1.0 - Le Minh Dang";
        if (copy_to_user((char __user *)arg, ver, sizeof(ver))) {
            ret = -EFAULT;
        } else {
            pr_info("asgn1_driver: [IOCTL] GET_VERSION returned\n");
        }
        break;
    }

    default:
        pr_warn("asgn1_driver: [IOCTL] Invalid command 0x%x\n", cmd);
        ret = -ENOTTY;
        break;
    }

    mutex_unlock(&asgn1_mutex);
    return ret;
}

static struct file_operations asgn1_fops = {
    .owner          = THIS_MODULE,
    .open           = asgn1_open,
    .release        = asgn1_release,
    .read           = asgn1_read,
    .write          = asgn1_write,
    .unlocked_ioctl = asgn1_unlocked_ioctl,
};

static int __init asgn1_init(void) {
    int ret = register_chrdev(major_number, DEVICE_NAME, &asgn1_fops);
    if (ret < 0) return ret;

    asgn1_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(asgn1_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(asgn1_class);
    }

    asgn1_device = device_create(asgn1_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(asgn1_device)) {
        class_destroy(asgn1_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(asgn1_device);
    }
    pr_info("asgn1_driver: Module loaded with major=%d\n", major_number);
    return 0;
}

static void __exit asgn1_exit(void) {
    device_destroy(asgn1_class, MKDEV(major_number, 0));
    class_destroy(asgn1_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("asgn1_driver: Module unloaded\n");
}

module_init(asgn1_init);
module_exit(asgn1_exit);
