all:
	gcc anonymize.c -o anonymize.so -fPIC -shared -ldl -D_GNU_SOURCE
