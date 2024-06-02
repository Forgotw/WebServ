#!/bin/bash

mkdir -p var/php-fpm.d/tmp var/php-fpm.d/log var/php-fpm.d/pool.d
chown $USER:$GROUP var/php-fpm.d/tmp/

USER=$(whoami)
GROUP=$(id -gn $USER)

PHP_FPM_CONF="var/php-fpm.d/php-fpm.conf"
WWW_CONF="var/php-fpm.d/pool.d/www.conf"

cat <<EOL > $PHP_FPM_CONF
[global]
pid = $(pwd)/var/php-fpm.d/tmp/php-fpm.pid
error_log = $(pwd)/var/php-fpm.d/log/php-fpm.log
include=$(pwd)/var/php-fpm.d/pool.d/*.conf
EOL

cat <<EOL > $WWW_CONF
[www]
user = $USER
group = $GROUP
listen = $(pwd)/var/php-fpm.d/tmp/php-fpm.sock
listen.owner = $USER
listen.group = $GROUP
listen.mode = 0660
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOL

php-fpm --fpm-config $PHP_FPM_CONF --nodaemonize
