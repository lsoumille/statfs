#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/mount.h>

#include "module.h"

static struct mount * structData;

static int my_show_function(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	if(*ppos != 0)
	{
		struct list_head * pos, * n;
		struct mount * tmp;
		int len = 0;
		//Iterate through the mnt_list
		list_for_each_safe(pos, n, &structData->mnt_list)
		{
			tmp = list_entry(pos, struct mount, mnt_list);
			//get dentry struct to obtain name and permission
			struct dentry * dty = tmp->mnt_mountpoint;
			//get super block to obtain major minor blocksize and type
			struct super_block * sb = dty->d_sb;
			//create message
			if (sb != NULL && sb->s_type != NULL)
			{
				sprintf(buf + len,"[%02d:%02d] name: %s mounted on: %s%s (%s)\n\tFile system type: %s\n\tBlock size: %lu\n",
								MAJOR(sb->s_dev),
								MINOR(sb->s_dev),
								tmp->mnt_devname,
								dty->d_name.name[0] != '/' ? "*/" : "",
								dty->d_name.name,
								IS_RDONLY(dty->d_inode) ? "ro" : "rw",
								sb->s_type->name,
								sb->s_blocksize
								);
				 len += strlen(buf);
			}
		}
		*ppos += len;
		return len;
	}
	return 0;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = my_show_function
};

int init_module(void)
{
	struct proc_dir_entry * proc_entry;
	//Add entry statfs in the proc directory
	proc_entry = proc_create("statfs", 0444, NULL, &fops);
	if(proc_entry == NULL)
	{
		printk("statfs: failed to create proc entry.\n");
		return -ENOMEM;
	}
	//Get information about VFS
	struct path p;
	if (kern_path("/", LOOKUP_FOLLOW | LOOKUP_DIRECTORY, &p))
	{
		return -EIO;
	}
	//Convert vfsmount struct to mout struct
  structData = real_mount(p.mnt);
	printk("statfs : entry created\n");
	return 0;
}

void cleanup_module(void)
{
	remove_proc_entry("statfs", NULL);
	printk("statfs : entry deleted\n");
}

MODULE_LICENSE("GPL");
