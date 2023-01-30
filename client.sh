# check if GCC is installed
if command -v gcc >/dev/null 2>&1; then
if [ $# -gt 0 ]; then
    c99 MemoryConnection.c Client.c MemClient.c -D$1 -w -O3 -o /tmp/ndbc-out
else
    c99 MemoryConnection.c Client.c MemClient.c -w -O3 -o /tmp/ndbc-out
fi
if [[ $1 -eq "performance" ]]; then
    time /tmp/ndbc-out
else
    /tmp/ndbc-out
fi
else
    echo -e "\033[1m\033[31mError\033[0m: The GNU Compiler Colection GCC isn't installed on your system."
    echo -e "Run \033[47mapt install -y gcc\033[0m to install it."
    exit 1
fi