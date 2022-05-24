# skiring
Simple CLI for secrets management

## Quick Start

``` bash
cd ~/projects   # Or wherever you keep your project folders
git clone git@github.com:dboitnot/skiring.git
cd skiring

./configure
  # Lots of output here. Check the end for any errors

make
  # Again, check for errors in the output. Warnings are ok.
```

At this point you should have an executable named "skiring" in the project directory. The rest of the installation process will require root access. Skiring requires the following:

1. A dedicated skiring user
2. A skiring database which lives under /etc 3. The skiring file to be placed somewhere on the PATH with the proper
   setuid permission.

Skiring works by having a database file that only the skiring user can read. The skiring binary is flagged setuid and owned by skiring. That means that when a user runs the skiring command, the "effective" user of the process is skiring. Skiring's first task is to decide if the "real"
user has permission to read the requested key and proceed accordingly.
This is something like how sudo works. When you run sudo it always starts out running as root and decides if you're allowed then runs the requested command as the appropriate user.

The RPM package normally takes care of setting these things up but since we're compiling from source, here's the procedure:

```bash
sudo useradd -M skiring
sudo make install
sudo su - skiring
skiring init
```
