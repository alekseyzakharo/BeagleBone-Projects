//Assignment#4
//By: Aleksey Zakharov 301295506

#include <linux/module.h>
#include <linux/miscdevice.h>		// for misc-driver calls.
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/leds.h>

//LED
DEFINE_LED_TRIGGER(morse_code);

#define DEFAULT_DOTTIME 200
static int dottime = DEFAULT_DOTTIME;
static int dashtime;
#define TIME_BETWEEN_LETTERS dottime
#define TIME_BETWEEN_WORDS (dottime*7)

//Driver
#define MY_DEVICE_FILE  "morse-code"
#define BITS_IN_SHORT 16
#define ARRAYSIZE  8192
static int queuePos;
static char array[ARRAYSIZE];
static unsigned short morsecode_codes[] = {
		0xB800,	// A 1011 1
		0xEA80,	// B 1110 1010 1
		0xEBA0,	// C 1110 1011 101
		0xEA00,	// D 1110 101
		0x8000,	// E 1
		0xAE80,	// F 1010 1110 1
		0xEE80,	// G 1110 1110 1
		0xAA00,	// H 1010 101
		0xA000,	// I 101
		0xBBB8,	// J 1011 1011 1011 1
		0xEB80,	// K 1110 1011 1
		0xBA80,	// L 1011 1010 1
		0xEE00,	// M 1110 111
		0xE800,	// N 1110 1
		0xEEE0,	// O 1110 1110 111
		0xBBA0,	// P 1011 1011 101
		0xEEB8,	// Q 1110 1110 1011 1
		0xBA00,	// R 1011 101
		0xA800,	// S 1010 1
		0xE000,	// T 111
		0xAE00,	// U 1010 111
		0xAB80,	// V 1010 1011 1
		0xBB80,	// W 1011 1011 1
		0xEAE0,	// X 1110 1010 111
		0xEBB8,	// Y 1110 1011 1011 1
		0xEEA0	// Z 1110 1110 101
};

module_param(dottime, int, 0);

//*****functions*****
//*****LED*****
static void led_register(void)
{
	// Setup the trigger's name:
	led_trigger_register_simple(MY_DEVICE_FILE, &morse_code);
}

static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(morse_code);
}

static void led_lightup(int time)
{
	led_trigger_event(morse_code, LED_FULL);
	msleep(time);
	led_trigger_event(morse_code, LED_OFF);
}

//*****Driver*****
static ssize_t my_read(struct file *file, char *buff, size_t count, loff_t *ppos)
{
	int buffer_idx = 0;
	int data_idx = (int) *ppos;
	int bytes_read = 0;

	// Fill buffer 1 byte at a time
	for (buffer_idx = 0; buffer_idx < count; buffer_idx++) {

		if (data_idx == ARRAYSIZE || buffer_idx == queuePos) {
			break;
		}
		// Copy next byte of data into user's buffer.
		// copy_to_user returns 0 for success (# of bytes not copied)
		if (copy_to_user(&buff[buffer_idx], &array[data_idx], 1)) {
			printk(KERN_ERR "Unable to write to buffer.");
			return -EFAULT;
		}
		array[data_idx] = 0;

		data_idx++;
		bytes_read++;
	}
	queuePos = 0;
	// Write to in/out parameters and return:
	*ppos = data_idx;
	return bytes_read;  // # bytes actually read.
}

static ssize_t my_write(struct file *file, const char *buff, size_t count, loff_t *ppos)
{
	short characterMorse;
	int i; 					//iterator
	short mask;
	int dasher;				//this keeps count of how many 1's are in the short morse code
	int buff_idx = 0;
	int data_idx = queuePos;

	// Find min character
	for (buff_idx = 0; buff_idx < count; buff_idx++) {
		char ch;
		if(data_idx == ARRAYSIZE-1)
		{
			break;
		}
		// Get the character
		if (copy_from_user(&ch, &buff[buff_idx], 1)) {
			return -EFAULT;
		}

		// Skip special characters:
		if (ch >= 'A' && ch <= 'Z')
		{
			characterMorse = morsecode_codes[ch - 'A'];

		}
		else if(ch >= 'a' && ch <= 'z')
		{
			characterMorse = morsecode_codes[ch - 'a'];
		}
		else if(ch == ' ')
		{
			array[data_idx++] = ' ';
			array[data_idx++] = ' ';
			array[data_idx++] = ' ';

			continue;
		}
		else //if the character is no
		{
			continue;
		}

		dasher = 0;
		for(i = BITS_IN_SHORT-1; i >= 0 ;i--)
		{

			mask = 1 << i;
			if (data_idx >= ARRAYSIZE)
			{
				break;
			}
			if(mask & characterMorse)
			{
				if(dasher == 2)
				{
					led_lightup(dashtime);
					array[data_idx++] = '-';
					dasher = 0;
				}
				else
				{
					dasher++;
				}
			}
			else
			{
				if(dasher)
				{
					led_lightup(dottime);
					array[data_idx++] = '.';
					dasher = 0;
				}
			}
			msleep(TIME_BETWEEN_LETTERS);
		}
		msleep(TIME_BETWEEN_WORDS);
		array[data_idx++] = ' ';
	}

	array[data_idx++] = '\n';

	// Return # bytes actually written.
	queuePos += data_idx++;
	*ppos += data_idx++;
	return count;
}

// Callbacks:  (structure defined in /linux/fs.h)
struct file_operations my_fops = {
	.owner    =  THIS_MODULE,
	.read     =  my_read,
	.write    =  my_write,
};

// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor    = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name     = MY_DEVICE_FILE,             // /dev/.... file.
		.fops     = &my_fops                    // Callback functions.
};


static int __init testdriver_init(void)
{
	int ret;
	printk(KERN_INFO "----> morsecode driver init(): file: /dev/%s.\n", MY_DEVICE_FILE);
	if(dottime > 2000 || dottime < 1)
	{
		printk(KERN_INFO "Invalid dottime, default set: %d\n", DEFAULT_DOTTIME);
	}
	else if(dottime != DEFAULT_DOTTIME)
	{
		printk(KERN_INFO "Parameter dottime: %d\n", dottime);
	}
	dashtime = dottime*3;
	queuePos = 0;
	//register as misc driver
	ret = misc_register(&my_miscdevice);
	led_register();

	return 0;
}
static void __exit testdriver_exit(void)
{
	printk(KERN_INFO "<---- My test driver exit().\n");
	misc_deregister(&my_miscdevice);
	led_unregister();
}

module_init(testdriver_init);
module_exit(testdriver_exit);
// Information about this module:
MODULE_AUTHOR("Aleksey Zakharov");
MODULE_DESCRIPTION("Morsecode Driver");
MODULE_LICENSE("GPL"); // Important to leave as GPL.

























