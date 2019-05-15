#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
//#include <linux/fs.h>

MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir;
static struct task_struct *curr;
static struct debugfs_blob_wrapper blob;

void print_id(struct task_struct* task);

static ssize_t write_pid_to_input(struct file *fp, 
                                const char __user *user_buffer, 
                                size_t length, 
                                loff_t *position)
{
        pid_t input_pid;
	
	
//        printk("before sscanf");
	sscanf(user_buffer, "%u", &input_pid);
	//printk("%d\n", input_pid);
//	printk("input pid");
    
	// Find task_struct using input_pid. Hint: pid_task
	curr = pid_task(find_get_pid(input_pid), PIDTYPE_PID);
	//printk("found task_struct using input_pid\n");
        // Tracing process tree from input_pid to init(1) process
        // Make Output Format string: process_command (process_id)
    print_id(curr);
	
	//printk("made output format\n");

        return length;
}

void print_id(struct task_struct* task) {
    
    // Tracing process tree from input_pid to init(1) process
    if(task->pid != 1)
    print_id(task->real_parent);
    
    // Make Output Format string: process_command (process_id)
    blob.size += sprintf(blob.data + blob.size, "%s (%d)\n", task->comm, task->pid);
}

static const struct file_operations dbfs_fops = {
        .write = write_pid_to_input,
};

static int __init dbfs_module_init(void)
{
        // Implement init module code

	static char buffer[100000];
	blob.data = (void *) buffer;
	blob.size = 0;


        dir = debugfs_create_dir("ptree", NULL);
        
        /*if (!dir) {
                printk("Cannot create dir\n");
                return -1;
        }
	printk("dir created\n");*/

        inputdir = debugfs_create_file("input", 0444, dir, NULL, &dbfs_fops);
	/*if (!inputdir) {
                printk("Cannot create input dir\n");
                return -1;
        }
	printk("inputdir created\n");*/
        ptreedir = debugfs_create_blob("ptree", 0444, dir, &blob);
	/*if (!ptreedir) {
                printk("Cannot create ptree dir\n");
                return -1;
        }
	printk("ptreedir created\n"); */
	// Find suitable debugfs API

	
	printk("dbfs_ptree module initialize done\n");

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module code
	debugfs_remove_recursive(dir);
	
	printk("dbfs_ptree module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
