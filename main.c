#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>


#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

void print_file(const char *name, int is_dir, int indent){
    if (indent){
        printf("%*c", indent * 2, ' ');
    }
    if (is_dir){
        printf("%s%s%s\n", COLOR_BLUE, name, COLOR_RESET);
    }
    else {
        printf("%s\n", name);
    }
}

int walk(const char *path, int indent) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return 1;
    }
    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL) {
        const char *name = dp -> d_name;

        // ignore files (current and parent)
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
            continue;
        }
        
        size_t len = strlen(path) + strlen(name) + 2;
        char sub_path[len];
        snprintf(sub_path, len, "%s/%s", path, name);
        
        int is_dir = 0;

        if (dp -> d_type == DT_UNKNOWN) {
            struct stat st;
            if (stat(sub_path, &st) == 0) {
                is_dir = S_ISDIR(st.st_mode);
            }
        } else {
            is_dir = (dp -> d_type == DT_DIR);
        }

        if (is_dir) {
            print_file(name, 1, indent);
            // walk to the sub_path
            walk(sub_path, indent + 1);
        }
        else {
            print_file(name, 0, indent);
        }

    }
    closedir(dir);
    return 0;
}

int main(int argc, char **argv){
    const char *path = "."; // default path is current dir

    if (argc > 1) {
        path = argv[1];
    }

    int err = walk(path, 0);

    return err;
}
