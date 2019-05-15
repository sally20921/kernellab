#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir;
static struct task_struct *curr;
static struct debugfs_blob_wrapper *myblob;
unsigned char *str;
unsigned char *temp;
static ssize_t write_pid_to_input(struct file *fp, 
                                const char __user *user_buffer, 
                                size_t length, 
                                loff_t *position)
{
        pid_t input_pid;

        sscanf(user_buffer, "%u", &input_pid);
	str = kmalloc(1024,GFP_KERNEL);
	temp = kmalloc(1024, GFP_KERNEL);
	curr = pid_task(find_vpid(input_pid), PIDTYPE_PID);
	sprintf(temp, "%s (%u)\n", curr->comm, curr->pid);
	strcat(temp, str);
	strcpy(str, temp);
	while(curr-> pid !=1 ){
		curr = curr->real_parent;
		sprintf(temp, "%s (%u) \n", curr->comm, curr->pid);
		strcat(temp, str);
		strcpy(str, temp);
	}
	myblob->data = (void*) str;
	myblob->size = strlen(str)+1;
	
       	// Find task_struct using input_pid. Hint: pid_task

        // Tracing process tree from input_pid to init(1) process

        // Make Output Format string: process_command (process_id)

        return length;
}

static const struct file_operations dbfs_fops = {
        .write = write_pid_to_input,
};

static int __init dbfs_module_init(void)
{
        // Implement init module code

        dir = debugfs_create_dir("ptree", NULL);
        
        if (!dir) {
                printk("Cannot create ptree dir\n");
                return -1;
        }
	myblob = kmalloc(sizeof (struct debugfs_blob_wrapper), GFP_KERNEL);
        inputdir = debugfs_create_file("input",0666, dir, NULL, &dbfs_fops);
        ptreedir = debugfs_create_blob("ptree", 0666, dir, myblob); // Find suitable debugfs API

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        debugfs_remove_recursive( dir );// Implement exit module code
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);