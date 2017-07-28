#requires libjpeg-dev
#to view images, install libcaca-dev and caca-utils and use cacaview.

gcc capture.c -ljpeg -DIO_READ -DIO_MMAP -DIO_USERPTR -g
