if cat < /etc/passwd | tr a-z A-Z | sort -u > out
then echo 1
else echo sort failed!
fi