#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/atomic.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Le Minh Dang - MSSV");
MODULE_DESCRIPTION("Assignment 1: Misc Device Counter Driver");
MODULE_VERSION("1.0");

#define DEVICE_NAME "counter"

static atomic_t counter_val = ATOMIC_INIT(0);
static atomic_t read_operations = ATOMIC_INIT(0);
static atomic_t write_operations = ATOMIC_INIT(0);

static ssize_t counter_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    char out_str[32];
    int current_val, str_len;

    if (*offset > 0)
        return 0;

    current_val = atomic_read(&counter_val);
    atomic_inc(&counter_val);
    atomic_inc(&read_operations);

    str_len = snprintf(out_str, sizeof(out_str), "%d\n", current_val);
    if (copy_to_user(buf, out_str, str_len))
        return -EFAULT;

    *offset += str_len;
    pr_info("counter_driver: Read val=%d, incremented to %d\n", current_val, current_val + 1);
    return str_len;
}

static ssize_t counter_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    char kbuf[32];
    int new_val, ret;
    size_t copy_len = len < sizeof(kbuf) - 1 ? len : sizeof(kbuf) - 1;

    if (copy_from_user(kbuf, buf, copy_len))
        return -EFAULT;
    kbuf[copy_len] = '\0';

    ret = kstrtoint(strim(kbuf), 10, &new_val);
    if (ret != 0 || new_val < 0) {
        pr_warn("counter_driver: Invalid write value\n");
        return -EINVAL;
    }

    atomic_set(&counter_val, new_val);
    atomic_inc(&write_operations);
    pr_info("counter_driver: Counter set to %d\n", new_val);
    return len;
}

static struct file_operations counter_fops = {
    .owner   = THIS_MODULE,
    .read    = counter_read,
    .write   = counter_write,
};

static struct miscdevice counter_misc_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DEVICE_NAME,
    .fops  = &counter_fops,
};

static int counter_proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "=== Counter Device Statistics ===\n");
    seq_printf(m, "Current Counter Value : %d\n", atomic_read(&counter_val));
    seq_printf(m, "Total Read Operations : %d\n", atomic_read(&read_operations));
    seq_printf(m, "Total Write/Resets    : %d\n", atomic_read(&write_operations));
    return 0;
}

static int counter_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, counter_proc_show, NULL);
}

static const struct proc_ops counter_proc_ops = {
    .proc_open    = counter_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init counter_init(void) {
    int ret = misc_register(&counter_misc_dev);
    if (ret) {
        pr_err("counter_driver: Failed to register misc device\n");
        return ret;
    }
    if (!proc_create("counter_info", 0444, NULL, &counter_proc_ops)) {
        pr_warn("counter_driver: Failed to create /proc/counter_info\n");
    }
    pr_info("counter_driver: Registered successfully as /dev/counter (major=10, minor=%d)\n",
            counter_misc_dev.minor);
    return 0;
}

static void __exit counter_exit(void) {
    remove_proc_entry("counter_info", NULL);
    misc_deregister(&counter_misc_dev);
    pr_info("counter_driver: Unregistered successfully\n");
}

module_init(counter_init);
module_exit(counter_exit);
