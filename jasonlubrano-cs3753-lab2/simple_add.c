#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>

/* take two numbers, add them, and tehn put them in a result*/
asmlinkage long sys_simple_add(int number1, int number2, int *result)
{
	int x;
	x = number1 + number2;
	//*result = number1 + number2;
	copy_to_user(result, &x, sizeof(int));
	printk(KERN_ALERT "number one: %d : number2: %d : \n", number1, number2);
	printk(KERN_ALERT "Result: %p \n", result);
	return 0;
}

