useradd -M -r -s /bin/bash skiring || echo "skiring user already exists"
chown skiring /usr/local/bin/skiring
chmod 4755 /usr/local/bin/skiring
mkdir -p /etc/skiring
chown skiring /etc/skiring
chmod 700 /etc/skiring
[ -f /etc/skiring/ring ] || /usr/local/bin/skiring init
chmod 600 /etc/skiring/ring
