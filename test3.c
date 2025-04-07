#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

// Typedefs ELF64
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Xword;

// Fonction qui ajuste p_offset, p_vaddr et p_align de façon à ce que pour tout V :
//    file_offset = p_offset + (V - p_vaddr)
// soit augmenté de x par rapport à l'ancienne relation.
// On choisit de laisser p_vaddr inchangé et de faire new_p_offset = old_p_offset + x.
// Ensuite, on calcule new_p_align pour que new_p_offset mod new_p_align = p_vaddr mod new_p_align.
void adjust_segment_fields(Elf64_Off old_p_offset, Elf64_Addr old_p_vaddr,
                           Elf64_Off x,
                           Elf64_Off *new_p_offset, Elf64_Addr *new_p_vaddr,
                           Elf64_Off *new_p_align)
{
    // On laisse p_vaddr inchangé (et de même p_paddr si nécessaire)
    *new_p_vaddr = old_p_vaddr;
    // On augmente p_offset de x pour obtenir le nouvel offset dans le fichier.
    *new_p_offset = old_p_offset + x;

    // Calcul de la différence : elle doit être un multiple de new_p_align.
    // Note : On calcule le module de la différence en valeur absolue.
    int64_t diff = (int64_t)(*new_p_offset) - (int64_t)(*new_p_vaddr);
    if(diff < 0)
        diff = -diff;

    // new_p_align doit diviser exactement diff.
    // La formule "diff & (-diff)" donne la plus grande puissance de 2 qui divise diff.
    *new_p_align = (Elf64_Off) diff & (-(Elf64_Off) diff);
}

int main(void)
{
    // Exemple de valeurs initiales :
    // Dans un ELF classique, souvent :
    //   p_offset est aligné sur 0 (ou un multiple de 0x1000)
    //   p_vaddr est une adresse élevée (ex. 0x400000) alignée sur 0x1000
    // Ainsi, dans un ELF valide, on a : p_offset % 0x1000 == p_vaddr % 0x1000 (souvent 0).
    Elf64_Off old_p_offset = 0x1000;       // par exemple
    Elf64_Addr old_p_vaddr = 0x401000;    // par exemple
    Elf64_Off x = 0x38;                 // on veut augmenter file_offset de 0x38

    Elf64_Off new_p_offset;
    Elf64_Addr new_p_vaddr;
    Elf64_Off new_p_align;

    adjust_segment_fields(old_p_offset, old_p_vaddr, x,
                          &new_p_offset, &new_p_vaddr, &new_p_align);

    printf("Anciennes valeurs :\n");
    printf("  p_offset = 0x%lx\n", old_p_offset);
    printf("  p_vaddr  = 0x%lx\n", old_p_vaddr);
    printf("\nAprès ajustement :\n");
    printf("  new_p_offset = 0x%lx\n", new_p_offset);
    printf("  new_p_vaddr  = 0x%lx\n", new_p_vaddr);
    printf("  new_p_align  = 0x%lx\n", new_p_align);

    // Pour vérifier, pour toute adresse virtuelle V, le file_offset vaut :
    //    new_file_offset = new_p_offset + (V - new_p_vaddr)
    // qui est exactement x de plus que l'ancien file_offset.
    // Par ailleurs, la règle d'alignement est :
    //    new_p_offset % new_p_align == new_p_vaddr % new_p_align
    printf("\nVérification de l'alignement :\n");
    printf("  new_p_offset mod new_p_align = 0x%lx\n", new_p_offset % new_p_align);
    printf("  new_p_vaddr  mod new_p_align = 0x%lx\n", new_p_vaddr % new_p_align);

    return 0;
}


