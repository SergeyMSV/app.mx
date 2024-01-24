==============
Apps Structure
==============

1. `Platforms etc. [TBD]`
2. `Config files for all 'mx'-applications`_
3. `WebUI`_

--------------
1. Platforms etc. [TBD]
--------------

Platform IDs
==============
::

 win	Windows desktop
 linx	Linux desktop
 
 mx53	i.mx53
 opio	Orange PI One
 vsom	VisionSOM

--------------
2. Config files for all 'mx'-applications
--------------

mx.conf.json
==============
::

 This file must be by all means.

/etc/mx.conf.json	-rw-r--r--	(chmod 644)

Contains:
1. platform
2. pipes

::

 {
   "platform": {
     "id": "win"
   },
   "pipe": {
     "card_mifare_win": "\\\\.\\pipe\\mx_card_mifare",
     "card_mifare_linx": "/tmp/mx_card_mifare",
     "card_mifare_vsom": "/tmpram/mx_card_mifare",
     "card_mifare_opio": "/tmpram/mx_card_mifare"
   }
 }

mxprivate.conf.json
==============

/root/

--------------
3. WebUI
--------------

3.1 Add user "user1" (it's mentioned in mxwebstate.service)

3.2. Copy all scripts, other files and npm-packages into /var/www

/var/www
::

 drwxr-xr-x  4 root root  4096 Nov  8 08:44 .
 drwxr-xr-x 12 root root  4096 Oct  9 15:13 ..
 drwxr-xr-x 61 root root  4096 Oct 10 16:59 node_modules
 -rw-r--r--  1 root root 40788 Oct 10 16:59 package-lock.json
 -rw-r--r--  1 root root    86 Oct 10 16:59 package.json
 drwxr-xr-x  3 root root  4096 Nov  8 08:44 web_state

/var/www/web_state
::

 drwxr-xr-x 3 root root 4096 Jan 24 14:29 .
 drwxr-xr-x 4 root root 4096 Jan 24 14:19 ..
 drwxr-xr-x 5 root root 4096 Jan 24 14:19 public
 -rw-r--r-- 1 root root  259 Jan 24 14:19 server.conf.json
 -r-xr-xr-x 1 root root 4037 Jan 24 14:19 server.js
 -rw-r--r-- 1 root root    0 Jan 24 14:29 server.log
 -rw-r--r-- 1 root root 2155 Jan 24 14:19 server_cpu.js
 -rw-r--r-- 1 root root 1263 Jan 24 14:19 server_data.js
 -r-xr-xr-x 1 root root  133 Jan 24 14:29 start.sh

Have the executable mode turned on chmod +x (555) server.js

3.3. Copy mxwebstate.service to "/usr/lib/systemd/system"

mxwebstate.service
::
 [Unit]
 Description=MX WEB STATE

 [Service]
 ExecStart=/var/www/web_state/server.js
 User=user1
 #User=nobody
 # Note Debian/Ubuntu uses 'nogroup', RHEL/Fedora uses 'nobody'
 Group=nogroup
 Environment=PATH=/usr/bin:/usr/local/bin
 Environment=NODE_ENV=production
 # If the user is not the root, available ports are > 1024.
 #Environment=PORT=80
 Environment=PORT=8080
 WorkingDirectory=/var/www/web_state
 Restart=always
 RestartSec=15

 [Install]
 WantedBy=multi-user.target

3.4. systemctl enable mxwebstate

3.5. systemctl start mxwebstate



 root@opio:~# ls -la /var/www

 drwxr-xr-x  4 root root  4096 Nov  8 08:44 .
 drwxr-xr-x 12 root root  4096 Oct  9 15:13 ..
 drwxr-xr-x 61 root root  4096 Oct 10 16:59 node_modules
 -rw-r--r--  1 root root 40788 Oct 10 16:59 package-lock.json
 -rw-r--r--  1 root root    86 Oct 10 16:59 package.json
 drwxr-xr-x  3 root root  4096 Nov  8 08:44 web_state
 
 
 1. server.js - Unix (LF) for node.js
/var/www/myapp/server.js should have #!/usr/bin/env node on the very first line and have the executable mode turned on chmod +x (555) server.js.

2. mx config in /etc/mx.conf.json