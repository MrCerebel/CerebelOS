#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* SFO */
/* Delivery - CerebelOS Package Manager */
/* Version: 0.0.1a */

#define REPO_URL "https://raw.githubusercontent.com/MrCerebel/CerebelOS-packages/main/"
#define DB_PATH "/var/delivery/installed"
#define TMP_PATH "/tmp/delivery"

void print_usage() {
    printf("Delivery - CerebelOS Package Manager\n");
    printf("Usage:\n");
    printf("  delivery --d <package>  Install a package\n");
    printf("  delivery --r <package>  Remove a package\n");
    printf("  delivery --l            List installed packages\n");
}

void ensure_dirs() {
    system("mkdir -p " DB_PATH);
    system("mkdir -p " TMP_PATH);
}

int is_installed(const char *package) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", DB_PATH, package);
    FILE *f = fopen(path, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

void mark_installed(const char *package) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", DB_PATH, package);
    FILE *f = fopen(path, "w");
    if (f) {
        fprintf(f, "installed\n");
        fclose(f);
    }
}

void mark_removed(const char *package) {
    char path[256];
    snprintf(path, sizeof(path), "rm -f %s/%s", DB_PATH, package);
    system(path);
}

void list_installed() {
    printf("Installed packages:\n");
    system("ls " DB_PATH " 2>/dev/null || echo 'No packages installed'");
}

void install_package(const char *package) {
    if (is_installed(package)) {
        printf("Package '%s' is already installed.\n", package);
        return;
    }

    printf("Downloading %s...\n", package);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "wget -O %s/%s.cpkg %s%s.cpkg",
             TMP_PATH, package, REPO_URL, package);

    if (system(cmd) != 0) {
        printf("Error: Could not download package '%s'.\n", package);
        return;
    }

    printf("Installing %s...\n", package);

    snprintf(cmd, sizeof(cmd), "mkdir -p %s/%s && tar -xf %s/%s.cpkg -C %s/%s",
             TMP_PATH, package, TMP_PATH, package, TMP_PATH, package);
    system(cmd);

    snprintf(cmd, sizeof(cmd), "cat %s/%s/manifest 2>/dev/null", TMP_PATH, package);
    system(cmd);

    snprintf(cmd, sizeof(cmd), "cp -r %s/%s/files/* / 2>/dev/null", TMP_PATH, package);
    system(cmd);

    snprintf(cmd, sizeof(cmd), "rm -rf %s/%s %s/%s.cpkg", TMP_PATH, package, TMP_PATH, package);
    system(cmd);

    mark_installed(package);
    printf("Package '%s' installed successfully!\n", package);
}

void remove_package(const char *package) {
    if (!is_installed(package)) {
        printf("Package '%s' is not installed.\n", package);
        return;
    }

    printf("Removing %s...\n", package);
    mark_removed(package);
    printf("Package '%s' removed.\n", package);
    printf("Note: Files installed by this package were not removed yet.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    ensure_dirs();

    if (strcmp(argv[1], "--d") == 0) {
        if (argc < 3) {
            printf("Error: No package specified.\n");
            return 1;
        }
        install_package(argv[2]);
    } else if (strcmp(argv[1], "--r") == 0) {
        if (argc < 3) {
            printf("Error: No package specified.\n");
            return 1;
        }
        remove_package(argv[2]);
    } else if (strcmp(argv[1], "--l") == 0) {
        list_installed();
    } else {
        print_usage();
        return 1;
    }

    return 0;
}
