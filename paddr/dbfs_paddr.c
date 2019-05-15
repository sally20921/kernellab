#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");

static struct dentry *dir, *output;
static struct task_struct *task;
struct packet{
	pid_t pid;
	unsigned long vaddr;
	unsigned long paddr;
}*pckt;
static void vaTopa(void){
	pgd_t* pgd;
	pud_t* pud;
	pmd_t* pmd;
	pte_t* pte;
	struct page* pg;
	pgd = pgd_offset(task -> mm, pckt -> vaddr);
	pud = pud_offset(pgd, pckt -> vaddr);
	pmd = pmd_offset(pud, pckt -> vaddr);
	pte = pte_offset_map(pmd, pckt -> vaddr);
	pg = pte_page(*pte);
	pckt -> paddr = page_to_phys(pg);
	pte_unmap(pte);
}

static ssize_t read_output(struct file *fp,
                      	  char __user *user_buffer,
                        size_t length,
                        loff_t *position)
{
	unsigned long *paddr;
	pckt = (void *) user_buffer;
	task = pid_task(find_get_pid(pckt -> pid), PIDTYPE_PID);
	vaTopa();
	return length;
        // Implement read file operation
}

static const struct file_operations dbfs_fops = {
	.read = read_output,        // Mapping file operations with your functions
};

static int __init dbfs_module_init(void)
{
        // Implement init module

        dir = debugfs_create_dir("paddr", NULL);

        if (!dir) {
                printk("Cannot create paddr dir\n");
                return -1;
        }
        // Fill in the arguments below
        output = debugfs_create_file("output",0777,dir ,NULL ,&dbfs_fops );
        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);