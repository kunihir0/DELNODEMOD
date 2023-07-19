// DeleteNodeModules.c : Defines the entry point for the application.
//

#include "../include/DeleteNodeModules.h"
#include <stdbool.h>

bool delete_dir(const char *path);
double get_dir_size(const char *path);
void print_size(double size_gb);

int main() {
  // Get the list of directories in the current directory
  char **dirs = NULL;
  size_t dirs_count = 0;
  WIN32_FIND_DATA find_data;
  HANDLE find_handle = FindFirstFile(".\\*", &find_data);
  if (find_handle != INVALID_HANDLE_VALUE) {
    do {
      if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        dirs = realloc(dirs, ++dirs_count * sizeof(char *));
        dirs[dirs_count - 1] = _strdup(find_data.cFileName);
      }
    } while (FindNextFile(find_handle, &find_data));
    FindClose(find_handle);
  }

  // Find node_modules directories
  char **node_modules_dirs = NULL;
  size_t node_modules_count = 0;
  for (size_t i = 0; i < dirs_count; i++) {
    char path[MAX_PATH];
    snprintf(path, MAX_PATH, "%s\\node_modules", dirs[i]);
    DWORD attributes = GetFileAttributes(path);
    if (attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
      node_modules_dirs =
          realloc(node_modules_dirs, ++node_modules_count * sizeof(char *));
      node_modules_dirs[node_modules_count - 1] = _strdup(path);
    }
  }

  // Log the results
  printf("\033[1;32mFound %zu node_modules directories:\033[0m\n",
         node_modules_count);
  double total_size = 0;
  for (size_t i = 0; i < node_modules_count; i++) {
    printf("\033[1;34m%s\033[0m (", node_modules_dirs[i]);
    double size_gb =
        get_dir_size(node_modules_dirs[i]) / (1024.0 * 1024.0 * 1024.0);
    total_size += size_gb;
    print_size(size_gb);
    printf(" GB)\n");
  }

  printf("\033[1;33mTotal size: %.2f GB\033[0m\n", total_size);

  // Check if there are any directories to delete
  if (node_modules_count > 0) {
    // Prompt user to confirm deletion
    printf("Do you want to delete these directories? [y/n]: ");
    char response;
    scanf_s(" %c", &response, sizeof(response));

    if (response == 'y' || response == 'Y') {
      for (size_t i = 0; i < node_modules_count; i++) {
        if (delete_dir(node_modules_dirs[i])) {
          printf("Deleted %s\n", node_modules_dirs[i]);
        } else {
          printf("Failed to delete %s\n", node_modules_dirs[i]);
        }
      }

      printf("Deletion complete.\n");
    } else {
      printf("Deletion cancelled.\n");
    }
  } else {
    printf("No directories to delete.\n");
  }

  return 0;
}

bool delete_dir(const char *path) {
  size_t path_len = strlen(path);
  char *double_null_path = malloc(path_len + 2);
  memcpy(double_null_path, path, path_len);
  double_null_path[path_len] = '\0';
  double_null_path[path_len + 1] = '\0';

  SHFILEOPSTRUCT file_op = {NULL,
                            FO_DELETE,
                            double_null_path,
                            "",
                            FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
                            false,
                            NULL,
                            ""};

  int result = SHFileOperation(&file_op);
  free(double_null_path);

  if (result != 0) {
    printf("SHFileOperation failed with error code %d\n", result);
    return false;
  }

  return true;
}

double get_dir_size(const char *path) {
  double size = 0;
  WIN32_FIND_DATA find_data;
  char search_path[MAX_PATH];
  snprintf(search_path, MAX_PATH, "%s\\*.*", path);
  HANDLE find_handle = FindFirstFile(search_path, &find_data);
  if (find_handle != INVALID_HANDLE_VALUE) {
    do {
      if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        ULARGE_INTEGER file_size;
        file_size.LowPart = find_data.nFileSizeLow;
        file_size.HighPart = find_data.nFileSizeHigh;
        size += file_size.QuadPart;
      } else if (strcmp(find_data.cFileName, ".") != 0 &&
                 strcmp(find_data.cFileName, "..") != 0) {
        char sub_path[MAX_PATH];
        snprintf(sub_path, MAX_PATH, "%s\\%s", path, find_data.cFileName);
        size += get_dir_size(sub_path);
      }
    } while (FindNextFile(find_handle, &find_data));
    FindClose(find_handle);
  }
  return size;
}

void print_size(double size_gb) {
  double current_size = 0;
  int prev_size_len = 0;
  while (current_size < size_gb) {
    for (int i = 0; i < prev_size_len; i++) {
      printf("\b \b"); // Move cursor back and clear previous characters
    }
    prev_size_len = printf("%.2f", current_size);
    current_size += 0.01;
    Sleep(1);
  }
  for (int i = 0; i < prev_size_len; i++) {
    printf("\b \b"); // Move cursor back and clear previous characters
  }
  printf("%.2f", size_gb);
}
