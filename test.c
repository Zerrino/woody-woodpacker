#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage : %s <fichier_elf>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // 1. Ouvrir le fichier ELF
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    // 2. Obtenir la taille du fichier
    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }
    size_t file_size = st.st_size;

    // 3. Mmapper le fichier en mémoire avec les droits lecture et exécution
    void *mapped = mmap(NULL, file_size, PROT_READ | PROT_EXEC, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);

    // 4. Vérifier la validité du fichier ELF
    Elf64_Ehdr *elf_header = (Elf64_Ehdr *)mapped;
    if (memcmp(elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "Le fichier n'est pas un ELF valide\n");
        munmap(mapped, file_size);
        return EXIT_FAILURE;
    }

    // (Optionnel) Afficher quelques informations de l'en-tête ELF
    printf("ELF entry point: 0x%lx\n", elf_header->e_entry);

    // 5. Calculer le pointeur vers le point d'entrée
    // ATTENTION : Dans un ELF classique, e_entry est une adresse virtuelle,
    // et il faut mapper chaque segment à l’adresse correcte (et réaliser les relocations).
    // Ici, on suppose que le fichier est un exécutable position-independent (PIE)
    // et que l’intégralité de l’ELF a été mappée de telle sorte que ce calcul fonctionne.
    void (*entry_point)() = (void (*)())((char *)mapped + elf_header->e_entry);

    // 6. Exécuter le code à partir du point d’entrée
    printf("Saut vers le point d'entrée et exécution du code...\n");
    entry_point();  // L’exécution du code mappé commence ici.

    // Après exécution, on pourrait nettoyer le mapping (ceci ne sera probablement pas atteint)
    munmap(mapped, file_size);
    return EXIT_SUCCESS;
}
