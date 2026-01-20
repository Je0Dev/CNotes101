/*
 * mygit.c
 * A very simple Git-like version control system.
 * Implements: init, add, commit
 *
 * Links with: -lcrypto -lz
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <openssl/sha.h> // For SHA-1
#include <zlib.h>        // For compression
#include <dirent.h>

#define BUFFER_SIZE 4096

// --- Hashing & Compression ---

// Calculate SHA-1 hash of a file, return as hex string
char* hash_file(const char* filepath) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha1;
    SHA1_Init(&sha1);
    
    FILE *fp = fopen(filepath, "rb");
    if (!fp) return NULL;
    
    unsigned char buf[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buf, 1, BUFFER_SIZE, fp))) {
        SHA1_Update(&sha1, buf, bytes_read);
    }
    SHA1_Final(hash, &sha1);
    fclose(fp);

    // Convert raw hash to hex string
    char *hex_hash = (char*)malloc(SHA_DIGEST_LENGTH * 2 + 1);
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(hex_hash + (i * 2), "%02x", hash[i]);
    }
    hex_hash[SHA_DIGEST_LENGTH * 2] = '\0';
    return hex_hash;
}

// Compress and save a file to the .mygit/objects database
void save_object(const char* hash, const char* filepath) {
    char obj_path[256];
    sprintf(obj_path, ".mygit/objects/%s", hash);

    if (access(obj_path, F_OK) == 0) {
        return; // Object already exists
    }

    // Read source file
    FILE *fin = fopen(filepath, "rb");
    fseek(fin, 0, SEEK_END);
    long fsize = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    char *in_buf = (char*)malloc(fsize);
    fread(in_buf, 1, fsize, fin);
    fclose(fin);

    // zlib compression
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    deflateInit(&zs, Z_DEFAULT_COMPRESSION);

    // Allocate output buffer (zlib recommends 1.1x size + 12)
    unsigned long out_size = (unsigned long)(fsize * 1.1) + 12;
    char *out_buf = (char*)malloc(out_size);
    
    zs.avail_in = fsize;
    zs.next_in = (Bytef*)in_buf;
    zs.avail_out = out_size;
    zs.next_out = (Bytef*)out_buf;
    
    deflate(&zs, Z_FINISH);
    deflateEnd(&zs);
    
    // Write compressed data
    FILE *fout = fopen(obj_path, "wb");
    fwrite(out_buf, 1, zs.total_out, fout);
    fclose(fout);
    
    free(in_buf);
    free(out_buf);
}

// --- 'init' Command ---
void cmd_init() {
    if (mkdir(".mygit", 0755) != 0) {
        perror(".mygit");
        return;
    }
    if (mkdir(".mygit/objects", 0755) != 0) {
        perror(".mygit/objects");
        return;
    }
    
    // Create the "index" (staging area)
    FILE *fp = fopen(".mygit/index", "w");
    if (fp) fclose(fp);
    
    // Create HEAD ref
    fp = fopen(".mygit/HEAD", "w");
    if (fp) {
        fprintf(fp, "ref: refs/heads/master\n");
        fclose(fp);
    }
    mkdir(".mygit/refs", 0755);
    mkdir(".mygit/refs/heads", 0755);
    
    printf("Initialized empty mygit repository in .mygit/\n");
}

// --- 'add' Command ---
void cmd_add(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s add <file>...\n", argv[0]);
        return;
    }

    FILE *index_fp = fopen(".mygit/index", "a");
    if (!index_fp) {
        perror("Error: cannot open index. Run 'mygit init' first.");
        return;
    }

    for (int i = 2; i < argc; i++) {
        char *filepath = argv[i];
        
        char *hash = hash_file(filepath);
        if (!hash) {
            fprintf(stderr, "Error: cannot open or hash file: %s\n", filepath);
            continue;
        }

        // 1. Save the compressed "blob" object
        save_object(hash, filepath);
        
        // 2. Add to index (staging area)
        // Format: "blob <hash> <filepath>"
        fprintf(index_fp, "blob %s %s\n", hash, filepath);
        
        printf("Added '%s' (hash: %s)\n", filepath, hash);
        free(hash);
    }
    fclose(index_fp);
}

// --- 'commit' Command ---
void cmd_commit(int argc, char *argv[]) {
    if (argc < 4 || strcmp(argv[2], "-m") != 0) {
        fprintf(stderr, "Usage: %s commit -m \"<message>\"\n", argv[0]);
        return;
    }
    char *message = argv[3];

    // 1. Create the "tree" object from the index
    char tree_data[BUFFER_SIZE * 10] = "";
    char line[BUFFER_SIZE];
    
    FILE *index_fp = fopen(".mygit/index", "r");
    if (!index_fp) {
        perror("No index. Run 'mygit add' first.");
        return;
    }
    
    while (fgets(line, sizeof(line), index_fp)) {
        strcat(tree_data, line);
    }
    fclose(index_fp);
    
    // Hash the *tree data itself* to get the tree hash
    unsigned char tree_hash_raw[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)tree_data, strlen(tree_data), tree_hash_raw);
    char tree_hash[SHA_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(tree_hash + (i * 2), "%02x", tree_hash_raw[i]);
    }
    tree_hash[SHA_DIGEST_LENGTH * 2] = '\0';
    
    // Save the tree object (just as text for simplicity)
    char tree_path[256];
    sprintf(tree_path, ".mygit/objects/%s", tree_hash);
    FILE *tree_fp = fopen(tree_path, "w");
    fprintf(tree_fp, "%s", tree_data);
    fclose(tree_fp);
    
    printf("Created tree object: %s\n", tree_hash);

    // 2. Create the "commit" object
    char commit_data[BUFFER_SIZE * 10];
    // We'll skip parent for simplicity
    sprintf(commit_data, "tree %s\nauthor C Programmer <c@example.com>\n\n%s\n", tree_hash, message);
    
    // Hash the commit data
    unsigned char commit_hash_raw[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)commit_data, strlen(commit_data), commit_hash_raw);
    char commit_hash[SHA_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(commit_hash + (i * 2), "%02x", commit_hash_raw[i]);
    }
    commit_hash[SHA_DIGEST_LENGTH * 2] = '\0';

    // Save the commit object
    char commit_path[256];
    sprintf(commit_path, ".mygit/objects/%s", commit_hash);
    FILE *commit_fp = fopen(commit_path, "w");
    fprintf(commit_fp, "%s", commit_data);
    fclose(commit_fp);

    // 3. Update the 'master' branch ref
    FILE *head_fp = fopen(".mygit/refs/heads/master", "w");
    fprintf(head_fp, "%s\n", commit_hash);
    fclose(head_fp);
    
    // 4. Clear the index
    index_fp = fopen(".mygit/index", "w");
    fclose(index_fp);
    
    printf("Committed! [%s] %s\n", commit_hash, message);
}

// --- Main Driver ---
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <init|add|commit>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        cmd_init();
    } else if (strcmp(argv[1], "add") == 0) {
        cmd_add(argc, argv);
    } else if (strcmp(argv[1], "commit") == 0) {
        cmd_commit(argc, argv);
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

