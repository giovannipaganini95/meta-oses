# PPG Kernel Module
This Kernel Module offers supports for standard Linux operations to communicate with a 
Virtual PPG Sensor. The module is loaded during boot and automatically creates a virtual device, called `/dev/ppg_dev`.
In this way it is possible to use the following Linux functions:
* **Open** - to access the device in an exclusive way. All other processes that will try this operation, will get an error.
* **Close** - to realease it and let other apps to get its file descriptor.
* **Read** - in order to read data from the device. Typically it manages unsigned integer data reads, but multiple data operations are supported, too. This can be achieved simply passing a suitable buffer and the desidered length value (that must be a multiple of 4 bytes). It will return the number of the bytes read in case of success or a negative value otherwise. 
  
Due to the nature and behaviour of the device, all **write** operations are not permitted and will end with an error.

By default only **warning and error messages** will be shown in the kernel ring buffer (accessible with `dmesg` command), but it is possible to change this behaviour in order to show info messages, too. This can be done simply setting to 1 the `#define DEBUG` in the `myppg.c` and recompiling the embedded image.
