# Dump script for gitter.im

This script is based on @dale3h https://gist.github.com/dale3h/002b172393f8092296c154ffdbe4890e

The original script dumps every day of messages in one html file. 

This modified version of the script also creates a total.html file that contains all messages for the time period you specified. This is very handy to search in gitter conversations.

### Requirements:
xmllint

### Usage:
Get usage with:
```
./gitter-dumper.sh -h
```

If you want to dump https://gitter.im/nRF51822-Arduino-Mbed-smart-watch/Lobby from January 1, 2018 up until the last message, in an output directory named *gitter*, do:

```
./gitter-dumper.sh -f -r "nRF51822-Arduino-Mbed-smart-watch/Lobby" -o gitter -s 2018/01/01
```

### Credits
Thanks to @dale3h for his original version of the script! (https://gist.github.com/dale3h/002b172393f8092296c154ffdbe4890e)