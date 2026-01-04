all: scheduler prepare

scheduler: src/scheduler.c header_files/task.h
	gcc -Iheader_files src/scheduler.c -o scheduler

prepare: src/prepare.c header_files/task.h
	gcc -Iheader_files src/prepare.c -o prepare

clean:
	rm -f scheduler prepare
