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
	pckt = (struct packet*) user_buffer;
	//printk("found pckt using user_buffer\n");
	
	struct pid *pid_struct;
	pid_t id = pckt->pid;
	//printk("found pid_t id in pckt %d\n", id);
	pid_struct = find_get_pid(pckt->pid);
	//printk("found pid_struct using find_get_pid\n");
	
	//task = pid_task(find_get_pid(pckt->pid), PIDTYPE_PID);
	task = pid_task(pid_struct, PIDTYPE_PID);
	//printk("found task using pid_task\n");

	/*unsigned long vpn1, vpn2, vpn3, vpn4, vpo;
	vpn1 = ((pckt->vaddr) >> 39) & 0x1FF;
	vpn2 = ((pckt->vaddr) >> 30) & 0x1FF;
	vpn3 = ((pckt->vaddr) >> 21) & 0x1FF;
	vpn4 = ((pckt->vaddr) >> 12) & 0x1FF;
	vpo = (pckt->vaddr) & 0xFFF;
	printk("found pckt vaddr");*/
	
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	/*pgd = task->mm->pgd;
	pud = (pud_t *)(((pgd + vpn1)->pgd & 0xFFFFFFFFFF000) + PAGE_OFFSET);
	pmd = (pmd_t *)(((pud + vpn2)->pud & 0xFFFFFFFFFF000) + PAGE_OFFSET);
	pte = (pte_t *)(((pmd + vpn3)->pmd & 0xFFFFFFFFFF000) + PAGE_OFFSET);
	pckt->paddr = ((pte + vpn4)->pte & 0xFFFFFFFFFF000) + vpo;*/
	pgd = pgd_offset(task -> mm, pckt -> vaddr);
	pud = pud_offset(pgd, pckt -> vaddr);
	pmd = pmd_offset(pud, pckt -> vaddr);
	pte = pte_offset_map(pmd, pckt -> vaddr);
	struct page* pg;
	pg = pte_page(*pte);
	pckt -> paddr = page_to_phys(pg);
	pte_unmap(pte);
	

	return length;
}

static const struct file_operations dbfs_fops = {
	// Mapping file operations with your functions
	.read = read_output,
};

static int __init dbfs_module_init(void) {

	// Implement init module
	dir = debugfs_create_dir("paddr", NULL);

	/*if (!dir) {
		printk("Cannot create dir\n");
		return -1;
	}
	printk("created dir\n");*/

	// Fill in the arguments below
	output = debugfs_create_file("output", 0444, dir, NULL, &dbfs_fops);
	/*if (!output) {
		printk("Cannot create output\n");
		return -1;
	}
	printk("created output\n");*/
    printk("initialized dbfs_paddr module\n");

	return 0;
}

static void __exit dbfs_module_exit(void) {
	
	// Implement exit module
	debugfs_remove_recursive(dir);
	printk("dbfs_paddr module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);

