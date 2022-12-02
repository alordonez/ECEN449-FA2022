#include <linux/module.h> /* needed by all modules */
#include <linux/kernel.h> /* needed for KERN_* and printk */
#include <linux/init.h> /* needed for __init and __exit macros */
#include <asm/io.h> /* needed for IO reads and writes */
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#include "xparameters.h" /* needed fr physical address of multiplier */

/* from xparameters.h */
#define PHY_ADDR XPAR_MULTIPLY1_0_S00_AXI_BASEADDR
/* size of physical address range for multiply */
#define MEMSIZE XPAR_MULTIPLY1_0_S00_AXI_HIGHADDR - XPAR_MULTIPLY1_0_S00_AXI_BASEADDR+1
/* define device name */
#define DEVICE_NAME "multiplier"
#define BUF_LEN 80

void* virt_addr; // virtual address pointing to multiplier

static int Major; // Major number assigned to device driver;

static int my_init(void);
static void  my_exit(void);
static int device_open(struct inode* inode, struct file* file);
static int device_release(struct inode* inode, struct file* file);
static ssize_t device_read(struct file* filp, char* buffer, size_t length, loff_t offset);
static ssize_t device_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset);

static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};


/* This function is run upon module load. This is where you setup data structures and reserve resources used by the module */
static int __init my_init(void)
{
  printk(KERN_INFO "Mapping virtual address...\n"); // Linux kernel's version of printf
  virt_addr = ioremap(PHY_ADDR, MEMSIZE);

  // register character device driver
  // device name = multiplier
  // let linux dynamically assign your device driver a major number and specify a minor number of 0
  dev_t dev;
  Major = register_chrdev(0, DEVICE_NAME, &fops);

  // handle device registration error
  if (Major < 0) {
    printk(KERN_ALERT "Device Registration Error %d\n", Major);
    return Major;
  }

  // print major number to kernel buffer
  else {
    printk(KERN_INFO "Registering device with dynamic Major Number %d\n", Major);
    printk(KERN_INFO "Create a device file for this device with 'mknod /dev/%s c %d 0'\n", DEVICE_NAME, Major);
  }
  
  printk("Physical address: 0x%x\n", PHY_ADDR);
  printk("Virtual address: 0x%x\n", virt_addr);
  
  return 0; // a non 0 return means init_module failed: module can't be loaded
}


/* This function is run just prior to the module's removal from the system. You should release _ALL_ resources used by your module here (otherwise be prepared for a reboot) */
static void __exit my_exit(void)
{
  // unregister device
  unregister_chrdev(Major, DEVICE_NAME);

  printk(KERN_ALERT "unmapping virtual address space....\n");
  iounmap((void*)virt_addr);
}

static int device_open(struct inode* inode, struct file* file)
{
  printk("Device opened");
  return 0;
}

static int device_release(struct inode* inode, struct file* file)
{
  printk("Device closed");
  return 0;
}

static ssize_t device_read(struct file* filp, char* buffer, size_t length, loff_t offset)
{
  // read bytes 0-11 within peripheral's address range -> put into user space buffer
  // "length" = num bytes user is requesting (valid 0-12)
  
  int bytes_transferred = 0;
  int* kernelBuffer = (int*)kmalloc(length * sizeof(int), GFP_KERNEL);
  kernelBuffer[0] = ioread8(virt_addr);
  kernelBuffer[1] = ioread8(virt_addr + 4);
  kernelBuffer[2] = ioread8(virt_addr + 8);
  char* kernel_buff = (char*)kernelBuffer;
  
  // error messages
  if (length < 0 || length > 12) {
    printk("Invalid length of %d, must be 0-12", length);
    return 1;
  }

  // put data into buffer
  int i;
  for (i = 0; i < length; i++) {
    put_user(*(kernel_buff++), buffer++); // read one char at a time to user space
    bytes_transferred++;
  }

  kfree(kernelBuffer);

  // return num bytes actually transferred to user space
  return bytes_transferred;
}

static ssize_t device_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset)
{
  // copy bytes from user buffer to kernel space using get_user & length
  char* kernelBuffer = (char*)kmalloc((length+1) * sizeof(char), GFP_KERNEL);

  int i = 0;

  for (i = 0; i < length; i++)
  {
    get_user(kernelBuffer[i], buffer++);
  }

  kernelBuffer[length] = '\0';
  int* int_buff = (int*)kernelBuffer;

  printk(KERN_INFO "writing %d to reg 0\n", int_buff[0]);
  iowrite8(int_buff[0], virt_addr + 0);

  printk(KERN_INFO "writing %d to reg 1\n", int_buff[1]);
  iowrite8(int_buff[1], virt_addr + 4);

  kfree(int_buff);
  kfree(kernelBuffer);

  // write bytes to multiplication peripheral (mem locations 0-7)
  return i;
}

// These define info that can be displayed by modinfo
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alana Ordonez (and others)");
MODULE_DESCRIPTION("Simple multiplier module");

// Here we define which functions we want to use for initialization and cleanup
module_init(my_init);
module_exit(my_exit)
