#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#define BUFFER_SIZE 100000

MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir;
static struct task_struct *curr;
static struct debugfs_blob_wrapper blob;

void print_pid_recursive(struct task_struct* task) {
    
    // Tracing process tree from input_pid to init(1) process
    if(task->pid != 1)
    print_pid_recursive(task->real_parent);
    
    // Make Output Format string: process_command (process_id)
    blob.size += snprintf(blob.data + blob.size,
                          BUFFER_SIZE - blob.size, "%s (%d)\n", task->comm, task->pid);
}

static ssize_t write_pid_to_input(struct file *fp,
                                  const char __user *user_buffer, size_t length, loff_t *position) {
    
    pid_t input_pid;
    
    sscanf(user_buffer, "%u", &input_pid);
    
    // Find task_struct using input_pid. Hint: pid_task
    curr = pid_task(find_get_pid(input_pid), PIDTYPE_PID);
    
    blob.size = 0;
    print_pid_recursive(curr);
    
    return length;
}

static const struct file_operations dbfs_fops = {
    .write = write_pid_to_input,
};

static int __init dbfs_module_init(void) {
    
    static char buffer[BUFFER_SIZE];
    blob.data = buffer;
    
    // Implement init module code
    dir = debugfs_create_dir("ptree", NULL);
    
    if (!dir) {
        printk("Cannot create ptree dir\n");
        return -1;
    }
    
    // Find suitable debugfs API
    inputdir = debugfs_create_file("input", S_IWUSR, dir, NULL, &dbfs_fops);
    ptreedir = debugfs_create_blob("ptree", S_IRUSR, dir, &blob);
    return 0;
}

static void __exit dbfs_module_exit(void) {
    
    // Implement exit module code
    debugfs_remove_recursive(dir);
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
