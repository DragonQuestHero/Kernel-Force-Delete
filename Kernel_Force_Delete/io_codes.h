#define IOCTL_DeleteFile CTL_CODE(FILE_DEVICE_FILE_SYSTEM,\
													 	  0x31337,\
											  	  METHOD_BUFFERED,\
						           FILE_READ_DATA|FILE_WRITE_DATA)