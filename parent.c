#include <unistd.h>
#include <sys/wait.h>

int read_line(int fd, char* buf, int max_len) {
    int i = 0;
    char c;
    int res;
    while (i < max_len - 1 && (res = read(fd, &c, 1)) > 0) {
        if (c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    if (res <= 0 && i == 0) return -1;
    return i;
}

void print(const char* s) {
    int len = 0;
    while (s[len] != '\0') len++;
    write(STDOUT_FILENO, s, len);
}

int main(void) {
    char filename[128];

    print("Enter filename: ");
    int file_len = read_line(STDIN_FILENO, filename, sizeof(filename));
    if (file_len <= 0) {
        return 1;
    }

    int pipe1[2];
    int pipe2[2];
    pipe(pipe1);
    pipe(pipe2);

    pid_t pid = fork();

    if (pid == 0) {
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execl("./child", "./child", filename, (char*)0);
        _exit(1);
    }

    close(pipe1[0]);
    close(pipe2[1]);

    char buf[128];
    print("Enter numbers:\n");

    while (1) {
        print("> ");
        int len = read_line(STDIN_FILENO, buf, sizeof(buf));
        if (len == -1) {
            break;
        }

        write(pipe1[1], buf, len);
        write(pipe1[1], "\n", 1);

        char status;
        if (read(pipe2[0], &status, 1) <= 0) {
            break;
        }

        if (status == 'T') {
            print("Child terminated (prime or <= 1). Exiting.\n");
            break;
        }
        else if (status == 'C') {
            print("Composite number recorded in file.\n");
        }
    }

    close(pipe1[1]);
    close(pipe2[0]);

    wait((int*)0);
    return 0;
}