// ...

#define MAX_BUFFER 1024                        // max line buffer
#define MAX_FILENAME 257                       // max length of file path

// ...

#define ERASE 1
#define MIMIC 2
#define MORPH 4

// ...

struct filemanip_st {
  char src[MAX_FILENAME]; // This is the source file or the only file in case of an erase;
  char dst[MAX_FILENAME];
  unsigned int op; // 0 = erase; 1 = mimic; 2 = morph;
};
typedef struct filemanip_st filemanip;


// ...

int is_directory(const char *path); // Checks for directory
int dofileoperation(filemanip *); // perform file operations

// ...

// Check if a file is a directory
int is_directory(const char *path) {
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}

// ...

int main (int argc, char * argv[]) {

  // ...

  // mimic

                  if (!strcmp(args[0],"mimic")) {
                    if (!args[1] || !args[2]) {
                        syserrmsg("error mimic takes two or three parameters", NULL);
                    }
                    else {
                        strcpy(fileops.src, args[1]);
                        strcpy(fileops.dst, args[2]);
                        fileops.op = MIMIC;
                        dofileoperation(args, &fileops);
                    }
                  }
                  // ...
                  if (!strcmp(args[0],"morph")) {
                    if (!args[1] || !args[2]) {
                        syserrmsg("error mimic takes two or three parameters", NULL);
                    }
                    else {
                        strcpy(fileops.src, args[1]);
                        strcpy(fileops.dst, args[2]);
                        fileops.op = MORPH;
                        dofileoperation(args, &fileops);
                    }
                  }
  // ...


  return 0;
}
// ...



int dofileoperation(filemanip *fileops ) {

  if ( ((fileops->op & MORPH) == MORPH) ||
      ((fileops->op & MIMIC) == MIMIC)) {

    unsigned int src_flags = O_RDONLY;
    unsigned int dst_flags = O_CREAT | O_WRONLY | O_TRUNC;
    unsigned int dst_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-

    int src_fd = open(fileops->src, src_flags);
    if (src_fd == -1) {
      if (fileops->op & MIMIC) {
        syserrmsg("mimic [src]", NULL);
        return EXIT_FAILURE;
      }
      else {
        syserrmsg("mimic [src]", NULL);
        return EXIT_FAILURE;
      }
    }

    // Check to make sure that the src and dest are correct
    int is_src_dir = is_directory(fileops->src);
    if (is_src_dir) {
      syserrmsg("[src] is a directory, this operation is not supported", NULL);
      return EXIT_FAILURE;
    }

    int is_dst_dir = is_directory(fileops->dst);

    // If the dst is a file, that will be the file name

    // If the dst is a valid directory, give the dst the file name of the src
    if (is_dst_dir) {
      char dst_name[MAX_FILENAME/2];
      char dst_path[MAX_FILENAME/2];
      char dst_dir[MAX_FILENAME/2];

      // Get the name from src
      strcpy(dst_name,basename(fileops->src));

      // The last folder name
      strcpy(dst_dir, basename(fileops->dst));

      // Get the path from dst
      strcpy(dst_path, dirname(fileops->dst));

      // concatenate the new file together
      fileops->dst[0] = '\0'; // zero out the string
      strcat(fileops->dst, dst_path);
      strcat(fileops->dst, "/");
      strcat(fileops->dst, dst_dir);
      strcat(fileops->dst, "/");
      strcat(fileops->dst, dst_name);
    }

    int dst_fd = open(fileops->dst, dst_flags, dst_perms);
    if (dst_fd == -1) {
      if (fileops->op & MIMIC) {
        syserrmsg("mimic [dst]", NULL);
        return EXIT_FAILURE;
      }
      else {
        syserrmsg("mimic [dst]", NULL);
        return EXIT_FAILURE;
      }
    }

    ssize_t num_read;
    char buf[MAX_BUFFER];
    while ((num_read = read(src_fd, buf, MAX_BUFFER)) > 0) {
      if (write(dst_fd, buf, num_read) != num_read) {
        if (fileops->op & MIMIC) {
          syserrmsg("mimic write error", NULL);
          return EXIT_FAILURE;
        }
        else {
          syserrmsg("morph write error", NULL);
          return EXIT_FAILURE;
        }
      }
    }
    if (num_read == -1) {
      if (fileops->op & MIMIC) {
        syserrmsg("mimic error reading", NULL);
        return EXIT_FAILURE;
      }
      else { syserrmsg("morph error reading", NULL);
        return EXIT_FAILURE;
      }
    }

    if (close(src_fd) == -1) {
      if (fileops->op & MIMIC) {
        syserrmsg("mimic: Error closing [src] file", NULL);
        return EXIT_FAILURE;
      }
      else {
        syserrmsg("morph: Error closing [src] file", NULL);
        return EXIT_FAILURE;
      }
    }
    if (close(dst_fd) == -1) {
      if (fileops->op & MIMIC) {
        syserrmsg("mimic: Error closing [dst] file", NULL);
        return EXIT_FAILURE;
      }
      else {
        syserrmsg("mimic: Error closing [dst] file", NULL);
        return EXIT_FAILURE;
      }
    }
  }

  // Check to see if the file needs to be removed
  // This happenes in the morph and erase case
  if ( ((fileops->op & MORPH) == MORPH) ||
      ((fileops->op & ERASE) == ERASE)) {

    // Removes file or directory
    // https://linux.die.net/man/3/remove
    if (remove(fileops->src)) {
      if ((fileops->op & MORPH) == MORPH) {
        syserrmsg("morph", NULL);
        return EXIT_FAILURE;
      }
      else {
        syserrmsg("erase", NULL);
        return EXIT_FAILURE;
      }
    }
  }

  return EXIT_SUCCESS;
}
