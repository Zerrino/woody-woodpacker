/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64_change.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:53:13 by Zerrino           #+#    #+#             */
/*   Updated: 2025/05/14 18:58:28 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"
#include <string.h>
#include <inttypes.h>

void print_segment_type(uint32_t p_type)
{
    switch (p_type) {
        case 0x00000000: printf("0x%08x\tPT_NULL\tProgram header table entry unused.\n", p_type); break;
        case 0x00000001: printf("0x%08x\tPT_LOAD\tLoadable segment.\n", p_type); break;
        case 0x00000002: printf("0x%08x\tPT_DYNAMIC\tDynamic linking information.\n", p_type); break;
        case 0x00000003: printf("0x%08x\tPT_INTERP\tInterpreter information.\n", p_type); break;
        case 0x00000004: printf("0x%08x\tPT_NOTE\tAuxiliary information.\n", p_type); break;
        case 0x00000005: printf("0x%08x\tPT_SHLIB\tReserved.\n", p_type); break;
        case 0x00000006: printf("0x%08x\tPT_PHDR\tSegment containing program header table itself.\n", p_type); break;
        case 0x00000007: printf("0x%08x\tPT_TLS\tThread-Local Storage template.\n", p_type); break;
        default:
            if (p_type >= 0x60000000 && p_type <= 0x6FFFFFFF)
                printf("0x%08x\tPT_OS_SPECIFIC\tOperating system specific.\n", p_type);
            else if (p_type >= 0x70000000 && p_type <= 0x7FFFFFFF)
                printf("0x%08x\tPT_PROC_SPECIFIC\tProcessor specific.\n", p_type);
            else
                printf("0x%08x\tUNKNOWN\tUnrecognized segment type.\n", p_type);
            break;
    }
}

int is_pie(t_elf_file *file)
{
    if (file->elf64_ehdr->e_type == 2)
        return (1);
    return (0);
}

void	elf64_hdr_change(t_elf_file *file)
{
	(void)file;
}

void	elf64_shdr_change(t_elf_file *file)
{
	int	i;

	i = 0;
	while (i < file->elf64_ehdr->e_shnum)
	{
        if (file->elf64_shdr[i]->sh_offset >= file->offset_insert)
        {
            file->elf64_shdr[i]->sh_addr += 64;
            file->elf64_shdr[i]->sh_offset += 64;
        }
		//ft_memset(file->elf64_shdr[i], '\0', sizeof(Elf64_Shdr));
		i++;
	}
}

void xor_crypt(unsigned char *data, unsigned char *key, size_t size_data, size_t size_key)
{
    for (size_t i = 0; i < size_data; i++)
    {
        data[i] ^= key[i % size_key];
    }
}


void generate_key(unsigned char *key, size_t size)
{
    int fd = open("/dev/urandom", O_RDONLY);
    size_t i;
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    if (read(fd, key, size) != (ssize_t)size)
    {
        perror("read");
        close(fd);
        exit(1);
    }
    close(fd);
    i = 0;
    while (i < size)
    {
        key[i] = key[i] % 62;
        if (key[i] < 10)
            key[i] += '0';
        else if (key[i] < 36)
            key[i] += 'A' - 10;
        else
            key[i] += 'a' - 36;
        i++;
    }
}

void check_phdr_align(const t_elf_file *file)
{
    uint16_t phnum = file->elf64_ehdr->e_phnum;

    puts("=== Vérification p_vaddr ≡ p_offset (mod p_align) ===");
    for (uint16_t i = 0; i < phnum; ++i) {
        const Elf64_Phdr *ph = file->elf64_phdr[i];

        uint64_t align      = ph->p_align ? ph->p_align : 1;   /* align==0 → 1 */
        uint64_t vaddr_mod  = ph->p_vaddr  % align;
        uint64_t offset_mod = ph->p_offset % align;
        int      ok         = (vaddr_mod == offset_mod);

        printf("PHDR %-3u │ p_vaddr=%-12" PRIu64
               " p_offset=%-12" PRIu64
               " p_align=%-6" PRIu64
               " │ vaddr%%align=%-3" PRIu64
               " offset%%align=%-3" PRIu64
               " │ %s\n",
               i,
               ph->p_vaddr,
               ph->p_offset,
               align,
               vaddr_mod,
               offset_mod,
               ok ? "OK" : "ERREUR");
    }
    puts("====================================================");
}


const char *get_dynamic_tag_name(uint64_t tag) {
    switch (tag) {
        case DT_NEEDED: return "NEEDED";
        case DT_INIT: return "INIT";
        case DT_FINI: return "FINI";
        case DT_INIT_ARRAY: return "INIT_ARRAY";
        case DT_INIT_ARRAYSZ: return "INIT_ARRAYSZ";
        case DT_FINI_ARRAY: return "FINI_ARRAY";
        case DT_FINI_ARRAYSZ: return "FINI_ARRAYSZ";
        case DT_GNU_HASH: return "GNU_HASH";
        case DT_STRTAB: return "STRTAB";
        case DT_SYMTAB: return "SYMTAB";
        case DT_STRSZ: return "STRSZ";
        case DT_SYMENT: return "SYMENT";
        case DT_DEBUG: return "DEBUG";
        case DT_PLTGOT: return "PLTGOT";
        case DT_RELA: return "RELA";
        case DT_RELASZ: return "RELASZ";
        case DT_RELAENT: return "RELAENT";
        case DT_FLAGS_1: return "FLAGS_1";
        case DT_VERNEED: return "VERNEED";
        case DT_VERNEEDNUM: return "VERNEEDNUM";
        case DT_VERSYM: return "VERSYM";
        case DT_RELACOUNT: return "RELACOUNT";
        case DT_JMPREL: return "JMPREL";
        case DT_NULL: return "NULL";
        default: return "UNKNOWN";
    }
}


int tag_points_to_memory(uint64_t tag) {
    switch (tag) {
        case DT_INIT:
        case DT_FINI:
        case DT_INIT_ARRAY:
        case DT_FINI_ARRAY:
        case DT_GNU_HASH:
        case DT_STRTAB:
        case DT_SYMTAB:
        case DT_PLTGOT:
        case DT_RELA:
        case DT_VERNEED:
        case DT_VERSYM:
        case DT_JMPREL:
            return 1;  // Tous ces tags pointent vers une adresse virtuelle (section .text/.data/.dynstr/etc.)

        case DT_NEEDED:
        case DT_INIT_ARRAYSZ:
        case DT_FINI_ARRAYSZ:
        case DT_STRSZ:
        case DT_SYMENT:
        case DT_RELASZ:
        case DT_RELAENT:
        case DT_FLAGS_1:
        case DT_VERNEEDNUM:
        case DT_RELACOUNT:
        case DT_DEBUG:
        case DT_NULL:
            return 0;  // Ce sont des valeurs entières, tailles, compteurs, flags, etc.

        default:
            return 0;  // Par défaut, on considère que c’est une valeur immédiate
    }
}

void print_dynamic_segment(t_elf_file *file, const Elf64_Phdr *phdr) {
    if (phdr->p_type != PT_DYNAMIC)
        return;

    Elf64_Dyn *dyn = (Elf64_Dyn *)(file->file_map + phdr->p_offset);
    size_t count = phdr->p_filesz / sizeof(Elf64_Dyn);

    printf("Dynamic section at offset 0x%lx contains %zu entries:\n", phdr->p_offset, count);
    printf("  Tag        Type                         Name/Value\n");

    for (size_t i = 0; i < count; i++) {
        uint64_t tag = dyn[i].d_tag;
        const char *name = get_dynamic_tag_name(tag);

        if (tag == DT_NEEDED) {
            // Pas de .dynstr ici → print brut
            printf("  0x%016lx (%-27s) Shared library: 0x%lx\n", tag, name, dyn[i].d_un.d_val);
        } else {
            printf("  0x%016lx (%-27s) 0x%lx\n", tag, name, dyn[i].d_un.d_val);
            if (dyn[i].d_un.d_val >= file->offset_insert && tag_points_to_memory(tag))
            {
                dyn[i].d_un.d_val += 64;
                printf("Augmenter\n");
            }
        }

        if (tag == DT_NULL)
            break;
    }
}


static Elf64_Off virt_to_off(t_elf_file *f, Elf64_Addr v)
{
    for (int i = 0; i < f->elf64_ehdr->e_phnum; i++) {
        Elf64_Phdr *ph = f->elf64_phdr[i];
        if (ph->p_type != PT_LOAD) continue;
        if (v >= ph->p_vaddr && v < ph->p_vaddr + ph->p_filesz)
            return ph->p_offset + (v - ph->p_vaddr);
    }
    return 0;          /* should not happen → sanity check ailleurs */
}

/* — patch proprement dit ------------------------------------------------ */

static void patch_got_jump_slots(t_elf_file *f,
                                 Elf64_Addr pltgot_va,
                                 size_t     jmprel_sz,
                                 size_t     delta,
                                 Elf64_Addr insert_va)
{
    /* 1) Où est le tableau GOT/PLTGOT dans le fichier ? */
    Elf64_Off  got_off = virt_to_off(f, pltgot_va);
    Elf64_Xword *got   = (Elf64_Xword *)(f->file_map + got_off);

    /* 2) Nombre d’entrées JUMP_SLOT   (→ 3 premières entrées réservées)   */
    size_t n_slots = jmprel_sz / sizeof(Elf64_Rela);

    for (size_t i = 3; i < 3 + n_slots; i++)           /* skip 0,1,2       */
    {
        if (got[i] >= insert_va)       /* le stub PLT qu’on pointe a bougé */
            got[i] += delta;           /* même delta que le stub           */
    }
}


static int reloc_addend_is_addr(uint32_t type)
{
    switch (type) {
        case R_X86_64_RELATIVE:
        case R_X86_64_64:
        case R_X86_64_COPY:
            return 1;
        default:
            return 0;
    }
}

static void patch_pointer_array(t_elf_file *f,
                                Elf64_Addr  array_va,
                                size_t      bytes,
                                Elf64_Addr  insert_va,
                                size_t      delta)
{
    if (!array_va || !bytes) return;

    Elf64_Addr *tab = (Elf64_Addr *)(f->file_map + virt_to_off(f, array_va));
    size_t n = bytes / sizeof(Elf64_Addr);

    for (size_t i = 0; i < n; i++)
        if (tab[i] >= insert_va)
            tab[i] += delta;
}

static void patch_rela_table(t_elf_file *f,
                             Elf64_Addr rela_va,
                             size_t     size,
                             size_t     entsz,
                             Elf64_Addr insert_va,
                             size_t     delta)
{
    if (!rela_va || !size) return;

    Elf64_Rela *rela = (Elf64_Rela *)(f->file_map + virt_to_off(f, rela_va));
    size_t count = size / entsz;

    for (size_t i = 0; i < count; i++) {
        Elf64_Rela *r = &rela[i];

        if (r->r_offset >= insert_va)
            r->r_offset += delta;

        if (reloc_addend_is_addr(ELF64_R_TYPE(r->r_info))
            && (Elf64_Addr)r->r_addend >= insert_va)
            r->r_addend += delta;
    }
}




static void patch_dynsym(t_elf_file *f,
                         Elf64_Addr insert_va,
                         size_t     delta,
                         Elf64_Off  dynsym_off,
                         size_t     dynsym_size)
{
    Elf64_Sym *sym = (Elf64_Sym *)(f->file_map + dynsym_off);
    size_t      n = dynsym_size / sizeof(Elf64_Sym);

    for (size_t i = 0; i < n; i++)
        if (sym[i].st_value >= insert_va && sym[i].st_shndx != SHN_UNDEF)
            sym[i].st_value += delta;
}

void elf64_reloc_fix(t_elf_file *file, Elf64_Off insert_off, size_t delta)
{
    /* 1) -- Trouver le PT_DYNAMIC ---------------------------------------- */
    Elf64_Addr insert_va = 0;        /* virtaddr correspondant à insert_off */

    for (int i = 0; i < file->elf64_ehdr->e_phnum; i++) {
        Elf64_Phdr *ph = file->elf64_phdr[i];
        if (insert_off >= ph->p_offset &&
            insert_off <  ph->p_offset + ph->p_filesz)
        {
            insert_va = ph->p_vaddr + (insert_off - ph->p_offset);
            break;
        }
    }
    Elf64_Phdr *dyn_ph = NULL;
    for (int i = 0; i < file->elf64_ehdr->e_phnum; i++) {
        if (file->elf64_phdr[i]->p_type == PT_DYNAMIC) {
            dyn_ph = file->elf64_phdr[i];
            break;
        }
    }
    if (!dyn_ph) {
        fprintf(stderr, "Pas de segment PT_DYNAMIC trouvé.\n");
        return;
    }

    Elf64_Dyn *dyn  = (Elf64_Dyn *)(file->file_map + dyn_ph->p_offset);
    size_t     dcnt = dyn_ph->p_filesz / sizeof(Elf64_Dyn);

    /* 2) -- Récupérer les adresses/sizes des tables ----------------------- */
    Elf64_Addr rela_addr = 0, jmprel_addr = 0;
    size_t     rela_size = 0, jmprel_size = 0, rela_ent = sizeof(Elf64_Rela);
    Elf64_Addr init_arr = 0, fini_arr = 0;
    size_t     init_sz  = 0, fini_sz = 0;
    Elf64_Addr symtab_va = 0, strtab_va = 0;
    //size_t     syment    = sizeof(Elf64_Sym);

    for (size_t i = 0; i < dcnt; i++) {
        switch (dyn[i].d_tag) {
        case DT_RELA:      rela_addr     = dyn[i].d_un.d_ptr; break;
        case DT_RELASZ:    rela_size     = dyn[i].d_un.d_val; break;
        case DT_RELAENT:   rela_ent      = dyn[i].d_un.d_val; break;

        case DT_JMPREL:    jmprel_addr   = dyn[i].d_un.d_ptr; break;
        case DT_PLTRELSZ:  jmprel_size   = dyn[i].d_un.d_val; break;

        case DT_INIT_ARRAY:   init_arr = dyn[i].d_un.d_ptr; break;
        case DT_INIT_ARRAYSZ: init_sz  = dyn[i].d_un.d_val; break;
        case DT_FINI_ARRAY:   fini_arr = dyn[i].d_un.d_ptr; break;
        case DT_FINI_ARRAYSZ: fini_sz  = dyn[i].d_un.d_val; break;

        case DT_SYMTAB:  symtab_va = dyn[i].d_un.d_ptr; break;
        case DT_STRTAB:  strtab_va = dyn[i].d_un.d_ptr; break;

        case DT_NULL:      i = dcnt; break;   /* fin de table */
        default:           break;
        }
    }
    size_t dynsym_size = 0;
    if (symtab_va && strtab_va && strtab_va > symtab_va)
        dynsym_size = strtab_va - symtab_va;
    if (dynsym_size)
        patch_dynsym(file,
                    insert_va,                 /* VA où tu as inséré 0x40 */
                    delta,                     /* 0x40 dans ton cas      */
                    virt_to_off(file, symtab_va),
                    dynsym_size);
    /* 3) -- Patch des relocations ---------------------------------------- */

    patch_pointer_array(file, init_arr, init_sz,  insert_va, delta);
    patch_pointer_array(file, fini_arr, fini_sz,  insert_va, delta);
    patch_rela_table(file, rela_addr,   rela_size,   rela_ent,
                     insert_va, delta);
    patch_rela_table(file, jmprel_addr, jmprel_size, rela_ent,
                     insert_va, delta);
    Elf64_Addr pltgot_addr = 0;
    /* on connaît déjà jmprel_addr / jmprel_size */

    for (size_t i = 0; i < dcnt; i++) {
        switch (dyn[i].d_tag) {
            case DT_PLTGOT:  pltgot_addr  = dyn[i].d_un.d_ptr; break;
            /* … comme avant … */
        }
    }

    /* 3b) -- Patch du contenu GOT pour tous les JUMP_SLOT ------------- */
    if (pltgot_addr && jmprel_size)
        patch_got_jump_slots(file,
                             pltgot_addr,
                             jmprel_size,
                             delta,
                             insert_va);

    Elf64_Addr got_va = 0;

    //Elf64_Dyn *dyn = ...;         /* déjà récupéré dans elf64_reloc_fix */
    for (size_t i = 0; dyn[i].d_tag != DT_NULL; i++)
        if (dyn[i].d_tag == DT_PLTGOT)
            got_va = dyn[i].d_un.d_ptr;

    if (!got_va) return;          /* improbable mais… */

    Elf64_Addr *got0 = (Elf64_Addr *)(file->file_map + virt_to_off(file, got_va));

    if (got0[0] >= insert_va)     /* l’ancienne valeur pointe après l’insert → patch */
        got0[0] += delta;
}



void elf64_phdr_change(t_elf_file *file)
{
    int i;
    Elf64_Phdr *target_phdr = NULL;
    Elf64_Off max_end = 0;
    //check_phdr_align(file);
    if (is_pie(file))
    {
        printf("Is no-pie !\n");
        for (i = 0; i < file->elf64_ehdr->e_phnum; i++)
        {
            print_segment_type(file->elf64_phdr[i]->p_type);
            if (file->elf64_phdr[i]->p_offset >= file->offset_insert)
            {
                //file->elf64_phdr[i]->p_align

                if (file->elf64_phdr[i]->p_type == PT_LOAD)
                {
                    file->elf64_phdr[i]->p_offset += 64;
                    if (file->elf64_phdr[i]->p_align >= 64)
                        file->elf64_phdr[i]->p_align = 64;
                }
                //file->elf64_phdr[i]->p_vaddr += 64;
                //file->elf64_phdr[i]->p_paddr += 64;
                //file->elf64_phdr[i]->p_offset += 64;
            }
            if (file->elf64_phdr[i]->p_type == PT_DYNAMIC)
            {
                print_dynamic_segment(file, file->elf64_phdr[i]);
            }
            if (file->elf64_phdr[i]->p_offset == 0)
            {
                printf("ICI!\n");
                file->elf64_phdr[i]->p_filesz += 64;
                file->elf64_phdr[i]->p_memsz += 64;
            }
        }
        elf64_reloc_fix(file, file->offset_insert, 64);
    }
    else
    {
        printf("Is pie !\n");
        for (i = 0; i < file->elf64_ehdr->e_phnum; i++)
        {
            print_segment_type(file->elf64_phdr[i]->p_type);
            if (file->elf64_phdr[i]->p_offset >= file->offset_insert)
            {
                file->elf64_phdr[i]->p_vaddr += 64;
                file->elf64_phdr[i]->p_paddr += 64;
                file->elf64_phdr[i]->p_offset += 64;
            }
            if (file->elf64_phdr[i]->p_type == PT_DYNAMIC)
            {
                print_dynamic_segment(file, file->elf64_phdr[i]);
            }
            if (file->elf64_phdr[i]->p_type == PT_LOAD)
            {
                if (file->elf64_phdr[i]->p_offset == 0)
                {
                    printf("I : %d\n", i);
                    file->elf64_phdr[i]->p_filesz += 64;
                    file->elf64_phdr[i]->p_memsz += 64;
                }
                Elf64_Off end = file->elf64_phdr[i]->p_offset + file->elf64_phdr[i]->p_filesz;
                if (end > max_end)
                {
                    max_end = end;
                    target_phdr = file->elf64_phdr[i];
                }
            }
        }
        if (!target_phdr)
        {
            fprintf(stderr, "Aucun segment LOAD trouvé\n");
            return;
        }
        elf64_reloc_fix(file, file->offset_insert, 64);
        target_phdr->p_flags |= PF_X;
    }
    //off_t stub_offset = max_end;
    check_phdr_align(file);



    unsigned char key[16];
    generate_key(key, 16);
    write(1, "key : ", 6);
    write(1, key, 16);
    write(1, "\n", 1);

    //_ft_encrypt((unsigned char *)(file->file_map + file->text_offset), key, file->text_size, sizeof(key));


    unsigned char stub[] = {
        // mov		r15, [rsp + 16] donc argc[1]
        0x48, 0x8B, 0x0C, 0x24, 0x48, 0x83, 0xF9, 0x02, 0x73, 0x10, 0x48, 0xC7, 0xC0, 0x3C, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x05,
        0x4c, 0x8b, 0x7c, 0x24, 0x10,
        0x49, 0xc7, 0xc2, 0x00, 0x00, 0x00, 0x00, //    mov    r10,0x0
        0x4c, 0x89, 0x54, 0x24, 0x10,             //    mov    QWORD PTR [rsp+0x10],r10
        // jmp short +0x0e
        0xeb, 0x4e,
        // ....WOODY....\n
        0x2e, 0x2e, 0x2e, 0x2e,
        0x57, 0x4f, 0x4f, 0x44, 0x59,
        0x2e, 0x2e, 0x2e, 0x2e,
        0x0a,
        // 64
        // 16 free byte
        // /proc/self/exe   2F 70 72 6F 63 2F 73 65 6C 66 2F 65 78 65 00 -> 15
        // poulet           70 6F 75 6C 65 74 00                         -> 7
        // /proc/self/fd/   2F 70 72 6F 63 2F 73 65 6C 66 2F 66 64 2F 00 -> 15
        // free zone
        // 16 free byte
        0x70, 0x6f, 0x75, 0x6c, 0x65, 0x74, 0x00, 0x00, // 8 byte
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 8 byte

        0x2f, 0x70, 0x72, 0x6f, 0x63, 0x2f, 0x73, 0x65, // 8 byte
        0x6c, 0x66, 0x2f, 0x65, 0x78, 0x65, 0x00, 0x00, // 8 byte
        // 16 free byte
        0x2f, 0x70, 0x72, 0x6f, 0x63, 0x2f, 0x73, 0x65, // 8 byte
        0x6c, 0x66, 0x2f, 0x66, 0x64, 0x2f, 0x00, 0x00, // 8 byte
        // 16 free byte
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 8 byte
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 8 byte

        // mov eax, 1
        0xB8, 0x01, 0x00, 0x00, 0x00,
        // mov edi, 1
        0xBF, 0x01, 0x00, 0x00, 0x00,
        // lea rsi, [rip - 0]
        0x48, 0x8D, 0x35, 0xa1, 0xFF, 0xFF, 0xFF,
        // mov edx, 14
        0xBA, 0x0e, 0x00, 0x00, 0x00,
        // syscall
        0x0f, 0x05,
        // mov rcx, [rsp]           ; rcx = argc
        0x48, 0x8b, 0x0c, 0x24,
        // cmp rcx, 2               ; if (argc < 2)
        0x48, 0x83, 0xf9, 0x02,
        // jge exit                  ; jump to exit
        0x0f, 0x8d, 0x00, 0x00, 0x00, 0x00,


        // lea rdi, [rip - 0]
        0x48, 0x8D, 0x3D, 0x93, 0xFF, 0xFF, 0xFF,
        // xor rsi, rsi
        0x48, 0x31, 0xF6,
        // mov eax, 319
        0xB8, 0x3f, 0x01, 0x00, 0x00,
        // syscall
        0x0f, 0x05,

        // test rax, rax
        0x48, 0x85, 0xc0,
        // js error
        0x0f, 0x88, 0x00, 0x00, 0x00, 0x00,
        // mov r8, rax
        0x49, 0x89, 0xc0,

        // lea rdi, [rip - 0]
        0x48, 0x8D, 0x3D, 0x86, 0xFF, 0xFF, 0xFF,
        // mov eax, 1
        0xB8, 0x02, 0x00, 0x00, 0x00,
        // xor rsi, rsi
        0x48, 0x31, 0xF6,
        // xor rdx, rdx
        0x48, 0x31, 0xd2,
        // syscall
        0x0f, 0x05,
        // test rax, rax
        0x48, 0x85, 0xc0,
        // js error
        0x0f, 0x88, 0x00, 0x00, 0x00, 0x00,

        // mov r9, rax
        0x49, 0x89, 0xc1,

        // looop:
        // lea rsi, [rip - 0]
        0x48, 0x8D, 0x35, 0x86, 0xFF, 0xFF, 0xFF,
        // xor rax, rax
        0x48, 0x31, 0xc0,
        // mov rdi, r9
        0x4c, 0x89, 0xcf,
        // mov rdx, 0x10
        0x48, 0xc7, 0xc2, 0x10, 0x00, 0x00, 0x00,
        // syscall
        0x0f, 0x05,

        // test rax, rax
        0x48, 0x85, 0xc0,
        // je done_read (nit define)
        0x0f, 0x84, 0x1c, 0x00, 0x00, 0x00,
        // js error (not defined)
        0x0f, 0x88, 0x00, 0x00, 0x00, 0x00,

        // lea rsi, [rip - 0]
        0x48, 0x8D, 0x35, 0x61, 0xFF, 0xFF, 0xFF,
        // mov rdx, rax
        0x48, 0x89,  0xc2,
        // mov eax, 1
        0xB8, 0x01, 0x00, 0x00, 0x00,
        // mov rdi, r8
        0x4c, 0x89, 0xc7,
        // syscall
        0x0f, 0x05,
        // jmp looop
        0xeb, 0xc7,
        // done_read:


        0x48, 0x8B, 0x04, 0x24, 0x48, 0x83, 0xF8, 0x02, 0x7E, 0x34, 0x48, 0x89, 0xC1, 0x48, 0x83, 0xE9, 0x02, 0x48, 0xC1, 0xE0, 0x03, 0x49, 0x89, 0xC2, 0x4A, 0x8D, 0x34, 0x14, 0x4E, 0x8B, 0x1C, 0x14, 0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0x3E, 0x49, 0x83, 0xEA, 0x08, 0x4A, 0x8D, 0x34, 0x14, 0x4C, 0x89, 0xD8, 0x4E, 0x8B, 0x1C, 0x14, 0x48, 0x89, 0x06, 0xE2, 0xEC, 0x48, 0x8D, 0x74, 0x24, 0x08, 0x4C, 0x89, 0xC0, 0x48, 0x31, 0xC9, 0x48, 0x31, 0xD2, 0x49, 0xC7, 0xC3, 0x0A, 0x00, 0x00, 0x00, 0x49, 0xF7, 0xF3, 0x80, 0xC2, 0x30, 0x52, 0x48, 0xFF, 0xC1, 0x48, 0x85, 0xC0, 0x75, 0xE7,
        // lea r11, [rip - 0]
        0x4c, 0x8D, 0x1d, 0xD9, 0xFE, 0xFF, 0xFF,

        0x4C, 0x89, 0xDF, 0x48, 0x83, 0xC7, 0x0E, 0x5A, 0x88, 0x17, 0x48, 0xFF, 0xC7, 0xE2, 0xF8, 0x4D, 0x31, 0xD2, 0x44, 0x88, 0x17,


        // juste avant
        0x41, 0x50, 0x56, 0x41, 0x53,
        // mov    rax,0x10a0 ENTRY ADDRESSE ICI!! //
        0x48, 0xC7, 0xC0, 0xAB, 0xAB, 0x40, 0x00,
        //
        0x50, 0x48, 0x8D, 0x34, 0x24, 0x48, 0xC7, 0xC2, 0x08, 0x00, 0x00, 0x00, 0x4C, 0x89, 0xC7, 0x49, 0xC7, 0xC2, 0x18, 0x00, 0x00, 0x00, 0x4D, 0x31, 0xC0, 0x48, 0xC7, 0xC0, 0x12, 0x00, 0x00, 0x00, 0x0F, 0x05, 0x58, 0x41, 0x5B, 0x5E, 0x41, 0x58,


        // Ici on va devoir gerer le decryptage



        0x4d, 0x89, 0xc4,                         //mov    r12,r8
        0x4d, 0x89, 0xdd,                         //mov    r13,r11
        0x56,                                     // push   rsi
        0x48, 0x81, 0xec, 0xDD, 0xDD, 0x00, 0x00, //   sub    rsp,0x1b6

        0x4C, 0x89, 0xC7, 0x48, 0x8D, 0x34, 0x24, 0x48, 0xC7, 0xC2, 0xDD, 0xDD, 0x00, 0x00, 0x49, 0xC7, 0xC2,
        0xAA, 0xAA,
        0x00, 0x00, 0x4D, 0x31, 0xC0, 0x48, 0xC7, 0xC0, 0x11, 0x00, 0x00, 0x00, 0x0F, 0x05, 0x48, 0x8D, 0x3C, 0x24, 0x4C, 0x89, 0xFE, 0x48, 0xC7, 0xC2, 0xDD, 0xDD, 0x00, 0x00,


        // only part left decryptage

        0x4D, 0x31, 0xC0,
        // xor r8, r8
        0x4D, 0x31, 0xC9,
        // xor r9, r9



        0x4D, 0x31, 0xD2,
        // xor r10 r10
        0x4D, 0x31, 0xDB,
        // xor r11 r11

        // BON BON BON ICI LE DEBUT
        //
        // LE WRITE
        //0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x4C, 0x89, 0xFE, 0x48, 0xC7, 0xC2, 0x10, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x05,

        //
        0x48, 0x8d, 0x34, 0x24,                     // lea    rsi,[rsp]
        //0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00,   // mov    rax,0x1
        //0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,   // mov    rdi,0x1
        //0x48, 0xc7, 0xc2, 0x0a, 0x00, 0x00, 0x00,   // mov    rdx,0xa
        //0x0f, 0x05,                                       // syscall

        //mov    rcx,0x1b6 -> la taille dans rcx
        0x48, 0xc7, 0xc1, 0xDD, 0xDD, 0x00, 0x00,
        0x4c, 0x89, 0xf8,  //              mov    rax,r15 la clef !
        // Donc rcx, la taille totale, rax la clef et rsi le stack

        0x49, 0x39, 0xc9,             // cmp r9, rcx
        0x74, 0x23,                         //  je     28 <fin>
        0x46, 0x8A, 0x54, 0x00, 0x00, // mov    r10b,BYTE PTR [rax+r8*1+0x0] rax clef
        0x46, 0x8A, 0x5C, 0x0E, 0x00, // mov    r11b,BYTE PTR [rsi+r9*1+0x0] rsi stack
        0x45, 0x30, 0xd3,           //  xor    r11b,r10b
        //0x41, 0xb3, 0x00,                      // mov    r11b,0x0
        0x46, 0x88, 0x5c, 0x0e, 0x00,               //mov     byte [rsi + r9], r11b ON ECRASE ICI

        0x49, 0xff, 0xc0,                    //inc    r8
        0x49, 0xff, 0xc1,                    //inc    r9
        0x49, 0x83, 0xf8, 0x10,             //cmp    r8,0x10
        0x75, 0xdd,                     //jne
        0x4d, 0x31, 0xc0,              // xor r8 r8
        0xeb, 0xd8,                   // jmp

        // LE WRITE
        //0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x4C, 0x89, 0xFE, 0x48, 0xC7, 0xC2, 0x10, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x05,

        //
       // 0x48, 0x8d, 0x34, 0x24,                     // lea    rsi,[rsp]
        //0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00,   // mov    rax,0x1
        //0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,   // mov    rdi,0x1
        //0x48, 0xc7, 0xc2, 0x0a, 0x00, 0x00, 0x00,   // mov    rdx,0xa
        0x90, 0x90,                                       // syscall




        0x4C, 0x89, 0xE7, 0x48, 0x8D, 0x34, 0x24, 0x48, 0xC7, 0xC2, 0xDD, 0xDD, 0x00, 0x00, 0x49, 0xC7, 0xC2,

        0xAA, 0xAA,

        0x00, 0x00, 0x4D, 0x31, 0xC0, 0x48, 0xC7, 0xC0, 0x12, 0x00, 0x00, 0x00, 0x0F, 0x05,


        0x48, 0x81, 0xc4,0xDD, 0xDD, 0x00, 0x00, //    add    rsp,0x1b6
        0x5e,                                     //    pop    rsi
        0x4d, 0x89, 0xeb,                         //    mov    r11,r13



        // decryptage!!





        0x48, 0x8B, 0x04, 0x24,
        0x48, 0x89, 0xC1,
        0x48, 0x83, 0xC1, 0x02,
        0x48, 0xC1, 0xE1, 0x03,
        0x48, 0x8D, 0x14, 0x0C,
        0x4C, 0x89, 0xDF,
        0x48, 0xC7, 0xC0, 0x3B, 0x00, 0x00, 0x00, 0x0F,
        0x05,
    };

    unsigned char pattern[] = { 0xAA, 0xAA};
    size_t stub_len = sizeof(stub);
    size_t pattern_len = sizeof(pattern);


    for (size_t i = 0; i <= stub_len - pattern_len; ++i) {
        if (memcmp(&stub[i], pattern, pattern_len) == 0) {
            ft_memcpy(&stub[i], &file->text_offset, sizeof(uint32_t));
        }
    }
    unsigned char pattern2[] = { 0xDD, 0xDD};
    for (size_t i = 0; i <= stub_len - pattern_len; ++i) {
        if (memcmp(&stub[i], pattern2, pattern_len) == 0) {
            ft_memcpy(&stub[i], &file->text_size, sizeof(uint32_t));
        }
    }

    unsigned char pattern3[] = { 0xAB, 0xAB};
    for (size_t i = 0; i <= stub_len - pattern_len; ++i) {
        if (memcmp(&stub[i], pattern3, pattern_len) == 0) {
            ft_memcpy(&stub[i], &file->elf64_ehdr->e_entry, sizeof(uint32_t));
        }
    }

    //Elf64_Addr stub_vaddr = (target_phdr->p_vaddr + (stub_offset - target_phdr->p_offset));
    //ft_memcpy(file->file_map + stub_offset, stub, sizeof(stub));
    //if ((long unsigned int)file->file_len < (stub_offset + sizeof(stub)))
    //    file->file_len = stub_offset + sizeof(stub);
    //target_phdr->p_filesz += 4096;
    //target_phdr->p_memsz  += 4096;
    //(void)stub_vaddr;
    if (is_pie(file))
        printf("Is no-pie !\n");
    else
        file->elf64_ehdr->e_entry += 64;
    file->elf64_ehdr->e_shoff += 64;
}


void	elf64_woody(t_elf_file *file)
{
	elf64_hdr_change(file);
	elf64_shdr_change(file);
	elf64_phdr_change(file);
	create_new_file_from_map(file);
}
