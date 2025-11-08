#!/bin/bash
./myshell <<'EOF2'
echo Hello world
ls | wc -l
sleep 2 &
jobs
exit
EOF2
