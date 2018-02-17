#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#define BUFF_SIZE 1024
#define MAJOR_NUMBER 789
#define DEVICE_NAME "simple_character_device"

static char *device_buff;
static int open_Count = 0;
static int close_Count = 0;
//static int posit = 0;
//  file headers 

// loff_t (*llseek) (struct file *, loff_t, int);
// ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
// ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
// int (*open) (struct inode *, struct file *);
// int (*release) (struct inode *, struct file *);

int simple_char_driver_open(struct inode *pinode, struct file *pfile){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	open_Count++;
	printk("Number of devices opened: %d\n", open_Count);
	return 0; /* indicates success */
}

int simple_char_driver_close(struct inode *pinode, struct file *pfile){
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	close_Count++;
	printk("Number of devices closed: %d\n", close_Count);
	return 0; /* 0 for success */
}

loff_t simple_char_driver_seek(struct file *pfile, loff_t offset, int whence){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	loff_t new_offset = 0;
	switch(whence) {
		case 0: /* SEEK_SET */
			new_offset = offset;
			break;
		case 1: /* SEEK_CUR */
			new_offset = pfile->f_pos + offset;
			break;
		case 2: /* SEEK_END */
			new_offset = BUFF_SIZE - offset;
			break;
	}
	if(new_offset > BUFF_SIZE){
		printk(KERN_ALERT "ERROR: cannot seek past BUFF_SIZE");
	} if(new_offset < 0){
		printk(KERN_ALERT "ERROR: cannot seek before buffer start");
	}
	pfile->f_pos = new_offset;
	return new_offset;

}

ssize_t simple_char_driver_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	/* max num of bytes that can be read from offset to BUFF_SIZE */
	int MAX_BYTES = BUFF_SIZE - *offset;
	/*number of bytes to read */
	int BYTES_TO_READ;
	/*number of bytes that we passed through */
	int BYTES_READ;
	/* logical statements about length of reading buffer */
	if(MAX_BYTES > length){
		BYTES_TO_READ = length;
	} else {
		BYTES_TO_READ = MAX_BYTES;
	}
	/* test for empty file */
	if(BYTES_TO_READ == 0){
		printk(KERN_ALERT "END OF DEVICE \n");
	}
	/* how many bytes did we read */
	BYTES_READ = BYTES_TO_READ - copy_to_user(buffer, device_buff + *offset, BYTES_TO_READ);
	printk(KERN_ALERT "DEVICE READ: %d BYTES\n", BYTES_READ);
	/* move the offset hoewver many bytes we read */
	*offset += BYTES_READ;
	/* when we get to the end */
	printk(KERN_ALERT "DEVICE FINISH READING");
	/*return how many bytes read*/
	return BYTES_READ; /* 0 for empty file */
}

ssize_t simple_char_driver_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	/* max num of bytes that can be WRITE from offset to BUFF_SIZE */
	int MAX_BYTES = BUFF_SIZE - *offset;
	/*number of bytes to WRITE */
	int BYTES_TO_WRITE;
	/*number of bytes WRITTEN */
	int BYTES_WROTE;
	/* logical statements about length of writing and buffer */	
	if(MAX_BYTES > length){
		BYTES_TO_WRITE = length;
	} else {
		BYTES_TO_WRITE = MAX_BYTES;
	}
	/* the bytes we wrote is equal with the total number of bytes to write, minus the remaining but we have yet read
		this remaaing bit comees from user space*/
	BYTES_WROTE = BYTES_TO_WRITE - copy_from_user(device_buff + *offset, buffer, BYTES_TO_WRITE);
	/* when we say howmany bytes we wrote */
	printk(KERN_ALERT "DEVICE WROTE: %d BYTES\n", BYTES_WROTE);
	/* move the offset by however many bytes we wrote*/
	*offset += BYTES_WROTE;
	/* alert kernel that we are finished */
	printk(KERN_ALERT "DEVICE FINISHED WRITING\n");

	return BYTES_WROTE; /*returning the length of the bytes.*/
}

/* file ops on this device */
struct file_operations simple_char_driver_file_operations ={
	.owner		= THIS_MODULE,					/* in the file op struct */
	.open 		= simple_char_driver_open, 		/* opens */
	.release	= simple_char_driver_close, 	/* closes. NO .close, use release instead? */
	.llseek 	= simple_char_driver_seek, 		/* seeks */
	.read 		= simple_char_driver_read, 		/* reads */
	.write 		= simple_char_driver_write, 	/* writes */
};

int simple_char_driver_init(void){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	/* register w/ kern and indicate we are doing the char_device_drv*/
	register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &simple_char_driver_file_operations);

	device_buff = kmalloc(BUFF_SIZE, GFP_KERNEL);
	return 0;
}

void simple_char_driver_exit(void){
	/* copy from helloModule where I am */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);

	/* unregister the char dev drv */
	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);

	void kfree(device_buff);
	return 0;
}

module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);