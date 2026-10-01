#include <unistd.h>
#include <fcntl.h>

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

int str_to_int(const char* s) {
    int res = 0;
    int sign = 1;
    int i = 0;
    if (s[0] == '-') {
        sign = -1;
        i++;
    }
    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        res = res * 10 + (s[i] - '0');
    }
    return res * sign;
}

int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;

    int file_fd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (file_fd < 0) {
        write(STDOUT_FILENO, "T", 1);
        return 1;
    }

    char buf[128];
    int len;

    while ((len = read_line(STDIN_FILENO, buf, sizeof(buf))) != -1) {
        if (len == 0) continue;

        int num = str_to_int(buf);

        if (num <= 1 || is_prime(num)) {
            write(STDOUT_FILENO, "T", 1);
            break;
        }

        write(file_fd, buf, len);
        write(file_fd, "\n", 1);

        write(STDOUT_FILENO, "C", 1);
    }

    close(file_fd);
    return 0;
}