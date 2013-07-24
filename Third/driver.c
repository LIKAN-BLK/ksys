#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <asm/generic/l4lib.h>
#include <linux/string.h>
#include <l4/log/log.h>
#include <l4/re/env.h>
#include <l4/sys/ipc.h>
#include <l4/sys/vcon.h>
enum Protocols {Crypt};
enum Opcodes {
  func_enc, func_dec
};
static dev_t first;
static struct cdev c_dev;
static char str[1024];

static int l4crypt_open(struct inode *i, struct file *f)
{
  return 0;
}

static int l4crypt_close(struct inode *i, struct file *f)
{
  return 0;
}


static ssize_t l4crypt_write(struct file *f, char __user *buf,
                              size_t len, loff_t *off)
{
  l4_msgtag_t tag,ret;
  l4_msg_regs_t *mr = l4_utcb_mr();
  int size;
  char* str_in=(char *) kmalloc(len *sizeof(char), GFP_KERNEL);
  if (copy_from_user(str_in, buf, len) != 0)
    return -EFAULT;
  printk(KERN_INFO "Len=%d\n",len);
  printk(KERN_INFO "Last char %d\n",(char)str_in[len-1]);
  str_in[len-1]='\0';
  mr->mr[0] = func_enc;
  mr->mr[1] = len;
  memcpy(&mr->mr[2],str_in,len);
  size=len/sizeof(int)+3;
  tag = l4_msgtag(Crypt,size,0,0);
  ret = l4_ipc_call(l4re_env_get_cap("enc_server"),l4_utcb(),tag,L4_IPC_NEVER);
  if(l4_msgtag_has_error(ret))
        printk(KERN_INFO "Catn't send the message3\n");
  printk(KERN_INFO "Whole string %s\n",(char*)&l4_utcb_mr()->mr[0]);
  printk(KERN_INFO "Ret=%d\n",l4_msgtag_words(ret));
  return len;
}

static struct file_operations l4crypt_fops = 
{
  .owner   = THIS_MODULE,
  .open    = l4crypt_open,
  .release = l4crypt_close,
  .write   = l4crypt_write
};

static int __init l4crypt_init(void)
{
  int major;
  if (alloc_chrdev_region(&first, 0, 1, "l4crypt") < 0)
  {
    printk(KERN_INFO "[L4CRYPT] its bad, i don't alloc numbers\n");
    goto de_alloc;
  }
  major = MAJOR ( first );
  printk(KERN_INFO "MAJOR=%d\n",major);
  cdev_init(&c_dev, &l4crypt_fops);
  if (cdev_add(&c_dev, first, 1) == -1)
  {
    goto de_register;
  }

  printk(KERN_INFO "[L4CRYPT] module inited\n");
  return 0;
 de_register:
  unregister_chrdev_region(first, 1);
 de_alloc:
  return -1; 
}

static void __exit l4crypt_exit(void)
{
  cdev_del(&c_dev);
  unregister_chrdev_region(first, 1);
  if (str !=  NULL) 
    kfree(str);
}

module_init(l4crypt_init);
module_exit(l4crypt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KozyrskiyB / likan26.09@rambler.ru");
MODULE_DESCRIPTION("test drier");
