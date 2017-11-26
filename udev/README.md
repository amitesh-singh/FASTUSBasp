## udev rules for fastusbasp

Copy [fastusbasp.rules](fastusbasp.rules) to `/etc/udev/rules.d/` and then restart the udev service,
     by running below command.  
### Ubuntu
     `sudo /etc/init.d/udev restart`
### Arch linux
     `udevadm control --reload`


