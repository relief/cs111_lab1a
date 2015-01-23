if cat < /etc/passwd | tr a-z A-Z | sort -u > out
then :
else echo sort failed!
fi