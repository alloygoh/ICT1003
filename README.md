# Remote Covert Defensive Operations (RCDO)

## Cloning the repo
The submodule will not be cloned when the repo is initially cloned,
please run this command to clone the RCDON repo.
```
git submodule init
git submodule update --remote --merge
```

## Configuration
### RCDON
Read
[rcdon/README.md](https://github.com/georgeneokq/RCDON-Bot/blob/248eca993fb81165e27b6336ce0fb1e9e53df605/README.md).

### RCDOD
#### API Key
Locate the [rcdod/menu_main/keyboard_payloads.ino](rcdod/menu_main/keyboard_payloads.ino),
and edit the `apiKey` variable.

### RCDOB
Run the command
```
make deploy
```
The command will build the binary, and subsequently move it to the
appropriate location within the RCDON.

## RCDO in action
1. Configure the modules on the RCDOD
1. Insert RCDOD into the computer, and press activate
1. Watch the RCDO suite perform its tasks

## Warning
For the first few times, please only activate one module or the other
(keyboard xor mouse), so that if anything messes up, there is still a
way to interact with the computer.

