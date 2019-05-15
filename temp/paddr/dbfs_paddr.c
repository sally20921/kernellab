#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");

static struct dentry *dir, *output;
static struct task_struct *task;
struct packet {
    pid_t pid;
    unsigned long vaddr;
    unsigned long paddr;
};

static ssize_t read_output(struct file *fp,
                           char __user *user_buffer, size_t length, loff_t *position) {
    
    // Implement read file operation
    struct packet *pckt;
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    unsigned long vpn1, vpn2, vpn3, vpn4, vpo;
    
    pckt = (struct packet*) user_buffer;
    
    vpn1 = ((pckt->vaddr) >> 39) & 0x1FF;
    vpn2 = ((pckt->vaddr) >> 30) & 0x1FF;
    vpn3 = ((pckt->vaddr) >> 21) & 0x1FF;
    vpn4 = ((pckt->vaddr) >> 12) & 0x1FF;
    vpo = (pckt->vaddr) & 0xFFF;
    
    task = pid_task(find_get_pid(pckt->pid), PIDTYPE_PID);
    
    pgd = task->mm->pgd;
    pud = (pud_t *)(((pgd + vpn1)->pgd & 0xFFFFFFFFFF000) + PAGE_OFFSET);
    pmd = (pmd_t *)(((pud + vpn2)->pud & 0xFFFFFFFFFF000) + PAGE_OFFSET);
    pte = (pte_t *)(((pmd + vpn3)->pmd & 0xFFFFFFFFFF000) + PAGE_OFFSET);
    pckt->paddr = ((pte + vpn4)->pte & 0xFFFFFFFFFF000) + vpo;
    
    return length;
}

static const struct file_operations dbfs_fops = {
    // Mapping file operations with your functions
    .read = read_output,
};

static int __init dbfs_module_init(void) {
    
    // Implement init module
    dir = debugfs_create_dir("paddr", NULL);
    
    if (!dir) {
        printk("Cannot create paddr dir\n");
        return -1;
    }
    
    // Fill in the arguments below
    output = debugfs_create_file("output", S_IWUSR, dir, NULL, &dbfs_fops);
    
    return 0;
}

static void __exit dbfs_module_exit(void) {
    
    // Implement exit module
    debugfs_remove_recursive(dir);
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
