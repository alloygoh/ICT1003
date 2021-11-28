# Remote Covert Defensive Operations (RCDO)

## Components of the RCDO suite
1. RCDO Notification Center (RCDON)
    - Web application that the binary will connect to
2. RCDO Device
    - Tiny circuit watch
3. RCDO Binary
    - Application that will be downloaded and executed on the machine

## Deploying the web application (RCDON)
### Setting up telegram bot
https://core.telegram.org/bots#6-botfather

### Configuring .env
Make a copy of [`rcdon/env.example`](rcdon/env.example), naming it `.env`. Fill
in `BOT_TOKEN` with the api key obtained from botfather.

### Running the web application
(Optional) Set up virtual environment
```bash
python -m venv venv
.\venv\Scripts\activate
deactivate  # To deactivate
```

Install dependencies
```bash
python -m pip install -r requirements.txt
```

### Usage
```bash
python app.py
```
## Building the binary (RCDOB)
Go to the [`rcdob`](rcdob) folder and run
```bash
make deploy
```

## Configuration

### IP and port
Deploy the web application, and figure out its IP Address and port to connect
to the web application.

#### RCDOD
For the tinycircuit, go to
[`rcdod/menu_main/keyboard_payloads.ino`](rcdod/menu_main/keyboard_payloads.ino),
and modify the `dlCmd` variable to point to the correct IP and port.

#### RCDOB
For the binary, go to [`rcdob/Makefile`](rcdob/Makefile), and edit the
`SERVER_NAME` and `SERVER_PORT` variable to point to the correct IP and port.

### API Key
The API Key is a self-generated string, it supports all characters that a
C-string supports.

#### RCDOD
For the tinycircuit, go to
[`rcdod/menu_main/keyboard_payloads.ino`](rcdod/menu_main/keyboard_payloads.ino),
and modify the `apiKey` variable to set an API Key.

#### RCDON
For the web application, go to [`rcdon/data/users.csv`](rcdon/data/users.csv),
and add an entry to it. The columns are:
1. username - Telegram username
2. key - **Make sure this key matches the key in the RCDOD**
3. can_kill - Set to 0
4. user_id - Leave blank, the web application will fill this up automatically.

## Using RCDO
1. Send `/start` to the telegram bot
2. Configure the modules on the RCDOD
3. Insert RCDOD into the computer, and press activate
4. Watch the RCDO suite perform its tasks
5. Send `/kill` to kill the bot

## Warning
For the first few times, please only activate one module or the other
(keyboard xor mouse), so that if anything messes up, there is still a
way to interact with the computer.

